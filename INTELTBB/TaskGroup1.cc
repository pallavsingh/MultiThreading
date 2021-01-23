#include <tbb/task.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/tick_count.h>
#include <iostream>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds

using namespace std;

class task1 : public tbb::task {
    public :
      tbb::task* execute(){
	  std::this_thread::sleep_for (std::chrono::seconds(10));
          cout << "task1 finished ... " << endl;
	  return nullptr;
      }
};


class task2 : public tbb::task {
    public :
      tbb::task* execute(){
	  std::this_thread::sleep_for (std::chrono::seconds(1));
          cout << "task2 finished ... " << endl;
	  return nullptr;
      }
};


class task3 : public tbb::task {
    public :
      tbb::task* execute(){
	  std::this_thread::sleep_for (std::chrono::seconds(5));
          cout << "task3 finished ... " << endl;
	  return nullptr;
      }
};


struct taskRoot : public tbb::task {
    public :
      tbb::task* execute(){
          cout << "Creating TaskList at TaskA " << endl;
          
          tbb::task_list tasklist;
          task1 * taskptr1 = new(tbb::task::allocate_child())task1;
          task2 * taskptr2 = new(tbb::task::allocate_child())task2;
          task3 * taskptr3 = new(tbb::task::allocate_child())task3;
         
	  tasklist.push_back(*taskptr1);
	  tasklist.push_back(*taskptr2);
	  tasklist.push_back(*taskptr3);
            
	  // Number of children we span include the wait
	  tbb::task::set_ref_count(4);

	  tbb::task::spawn_and_wait_for_all(tasklist);
	  cout << " Here we have Finished all the Tasks" << endl;


	  return nullptr;
      }
};


int main(int argc, char * argv[]){
    int n = tbb::task_scheduler_init::default_num_threads();
    // Construct task scheduler with p threads
    tbb::task_scheduler_init init(10);
    tbb::tick_count t0 = tbb::tick_count::now();

    taskRoot * ptr = new(tbb::task::allocate_root())taskRoot;
    tbb::task::spawn_root_and_wait(*ptr);

    return 0;
}
