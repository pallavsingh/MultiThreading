#include <iostream>
#include <mutex>
#include <memory>
#include <future>
#include <atomic>
#include <queue>
#include <chrono>   // sleep_for this thread.

using namespace std;

// Here we are trying to Solve problem with 2 Producer and 1 Consumer

condition_variable condvar;
mutex gmutex;

bool  isQFULL  = false;
bool  isQEMPTY = true;

int QSIZE = 30;
queue<int> q;

int main()
{
    auto prod1 = [](){
        while(true){
            unique_lock<mutex> lck(gmutex);
            condvar.wait(lck, [](){
                return (isQFULL != true);
            });
       
            int data = 1;
            cout << "Producer - 1 Send data : " << data << endl;
            q.push(data);
       
            std::this_thread::sleep_for (std::chrono::seconds(2));
           
            if (q.size() == QSIZE)
                isQFULL = true;
           
            isQEMPTY = false;
       
            lck.unlock();
            condvar.notify_all();
        }
    };


    auto prod2 = [](){
        while(true){
            unique_lock<mutex> lck(gmutex);
            condvar.wait(lck, [](){
                return (isQFULL != true);
            });
       
            int data = 2;
            cout << "Producer - 2 Send data : " << data << endl;
            q.push(data);
       
            if (q.size() == QSIZE)
                isQFULL = true;
           
            isQEMPTY = false;
       
            lck.unlock();
            condvar.notify_all();
        }
    };


    auto consumer = [](){
        while(true) {
            unique_lock<mutex> lck(gmutex);
            condvar.wait(lck, [](){
                return !isQEMPTY;
            });
       
            int data = q.front(); q.pop();
            if (q.empty())
               isQEMPTY = true;
                   
            isQFULL = false;
       
            cout << "RECIEVED DATA from : " << data << endl;

            lck.unlock();
            condvar.notify_all();
        }
    };

    thread t1(prod1);
    thread t2(prod2);
    thread t3(consumer);
   
    t1.join();
    t2.join();
    t3.join();
   
    return 0;
}

