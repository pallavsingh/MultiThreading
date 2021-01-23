#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <utility>
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <thread>
#include <type_traits>
#include <vector>
#include <iostream>

using namespace std;

template <typename T>
class ThreadSafeQueue {
    public:
        ~ThreadSafeQueue(void) { invalidate(); }

        bool tryPop(T & out) {
            std::lock_guard<std::mutex> lock{m_mutex};
            if (m_queue.empty() || !m_valid) {
                return false;
            }
           
            out = std::move(m_queue.front());
            m_queue.pop();
           
            return true;
        }


        bool waitPop(T& out) {
            std::unique_lock<std::mutex> lock{ m_mutex };
            m_condition.wait(lock, [this]() {
                return !m_queue.empty() || !m_valid;
            });

            if (!m_valid) {
                return false;
            }
           
            out = std::move(m_queue.front());
            m_queue.pop();
            return true;
        }

        void push(T value) {
            std::lock_guard<std::mutex> lock{m_mutex};
            m_queue.push(std::move(value));
            m_condition.notify_one();
        }

        bool empty(void) const {
            std::lock_guard<std::mutex> lock{m_mutex};
            return m_queue.empty();
        }

        void clear(void) {
            std::lock_guard<std::mutex> lock{m_mutex};
            while(!m_queue.empty()) {
                m_queue.pop();
            }
           
            m_condition.notify_all();
        }

        void invalidate(void) {
            std::lock_guard<std::mutex> lock{m_mutex};
            m_valid = false;
            m_condition.notify_all();
        }

        bool isValid(void) const {
            std::lock_guard<std::mutex> lock{m_mutex};
            return m_valid;
        }

    private:
   
        std::atomic_bool m_valid{true};
        mutable std::mutex m_mutex;
        std::queue<T> m_queue;
        std::condition_variable m_condition;
};


class ThreadPool{
    private:

        // Interface Class used by ThreadSafeQueue to hold which type of Task
        class IThreadTask {
            public:
            IThreadTask(void) = default;
            virtual ~IThreadTask(void) = default;
            IThreadTask(const IThreadTask& rhs) = delete;
            IThreadTask& operator=(const IThreadTask& rhs) = delete;
            IThreadTask(IThreadTask&& other) = default;
            IThreadTask& operator=(IThreadTask&& other) = default;

            virtual void execute() = 0;
        };

        // Here we accept the Functor which need to be Executed as Template Parameter
        template <typename Func>
        class ThreadTask: public IThreadTask {
        public:
            ThreadTask(Func&& func) : m_func{std::move(func)}{ }

            ~ThreadTask(void) override = default;
            ThreadTask(const ThreadTask& rhs) = delete;
            ThreadTask& operator=(const ThreadTask& rhs) = delete;
            ThreadTask(ThreadTask&& other) = default;
            ThreadTask& operator=(ThreadTask&& other) = default;

            void execute() override {
                m_func();
            }

        private:
       
            Func m_func;
        };

    public:
   
        /**
         * A wrapper around a std::future that adds the behavior of futures returned from std::async.
         * Specifically, this object will block and wait for execution to finish before going out of scope.
         */
        template <typename T>
        class TaskFuture {
            public:
                TaskFuture(std::future<T>&& future) :m_future{std::move(future)} { }
                TaskFuture(const TaskFuture& rhs) = delete;
                TaskFuture& operator=(const TaskFuture& rhs) = delete;
                TaskFuture(TaskFuture&& other) = default;
                TaskFuture& operator=(TaskFuture&& other) = default;
           
                ~TaskFuture(void) {
                    if (m_future.valid()) {
                        m_future.get();
                    }
                }

                auto get(void) { return m_future.get(); }
           
            private:
       
                std::future<T>  m_future;
        };

        ThreadPool(void) :ThreadPool{std::max(std::thread::hardware_concurrency(), 2u) - 1u} {
            /*
             * Always create at least one thread.  If hardware_concurrency() returns 0,
             * subtracting one would turn it to UINT_MAX, so get the maximum of
             * hardware_concurrency() and 2 before subtracting 1.
             */
        }

        explicit ThreadPool(const std::uint32_t numThreads) :m_done{false}, m_workQueue{}, m_threads{} {
            try {
                for(std::uint32_t i = 0u; i < numThreads; ++i) {
                    // Here we Register w.r.t Thread the Starting Func and Its Arguments
                    m_threads.emplace_back(&ThreadPool::worker, this);
                }
            } catch(...) {
                destroy();
                throw;
            }
        }

        ThreadPool(const ThreadPool& rhs) = delete;
        ThreadPool& operator=(const ThreadPool& rhs) = delete;
       ~ThreadPool(void) {
            destroy();
        }

        template <typename Func, typename... Args>
        auto submit(Func&& func, Args&&... args) {
            auto boundTask = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);
            using ResultType = std::result_of_t<decltype(boundTask)()>;
            using PackagedTask = std::packaged_task<ResultType()>;
            using TaskType = ThreadTask<PackagedTask>;
           
            PackagedTask task{std::move(boundTask)};
           
            // Here we Store the return type of functional
            TaskFuture<ResultType> result { task.get_future() };
           
            // Here we Push Task into the workQueue (ThreadSafeQueue)
            m_workQueue.push(std::make_unique<TaskType>(std::move(task)));
           
            return result;
        }

    private:

        void worker(void) {
            // Here we Keep Poping Task and executing it.
            //   --- Keep Doing till Queue is Valid
           
            while(!m_done) {
                std::unique_ptr<IThreadTask> pTask{nullptr};
                if(m_workQueue.waitPop(pTask)) {
                    pTask->execute();
                }
            }
        }

        void destroy(void) {
            m_done = true;
            m_workQueue.invalidate();
           
            for (auto & thread : m_threads) {
                if (thread.joinable()) {
                    thread.join();
                }
            }
        }

    private:
   
        std::atomic_bool m_done;
        ThreadSafeQueue<std::unique_ptr<IThreadTask>> m_workQueue;
        std::vector<std::thread> m_threads;
};

// Here we Create Single Instance of Default ThreadPool object
inline ThreadPool& getThreadPool(void) {
    static ThreadPool defaultPool;
    return defaultPool;
}


// Submit a job ----->>>> default thread pool.
template <typename Func, typename... Args>
inline auto submitJob(Func&& func, Args&&... args) {
    return getThreadPool().submit(std::forward<Func>(func), std::forward<Args>(args)...);
}


int main() {

    std::vector<ThreadPool::TaskFuture<void>> v;
   
    for(std::uint32_t i = 0u; i < 10u; ++i) {
        v.push_back(submitJob([i](){
            cout << "Submitting job for id : " << i << endl;
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }));
    }

    for(auto & item: v){
        item.get();
    }

    return 0;
}
