#include "tbb/concurrent_queue.h"
#include "tbb/blocked_range.h"
#include "tbb/parallel_for.h"
#include <tbb/task_scheduler_init.h>
#include <tbb/task.h>
#include <tbb/tick_count.h>
#include <iostream>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <string>

using namespace tbb;
using namespace std;

struct  Node{
     public :
        string  str;
        long    value;

        Node(string arg1="", long arg2 = 0) : str(arg1), value(arg2){}
};

// A concurrent Queue containing Struct Node Elements
concurrent_queue<struct Node> concQueue;

void InsertToQueue(struct Node obj){
     cout << "INSERT ::: Task Name :  " << obj.str << "   value :  " << obj.value << endl;  
     concQueue.push(obj);
} 


void RemoveFromQueue(){
	struct Node obj;
        while (! concQueue.empty()) {
            if (concQueue.try_pop(obj))
                cout << "REMOVE FROM QUEUE ::: Task Name :  " << obj.str << "   value :  " << obj.value << endl;  
	    else
		cout << "Object is Not present " << endl;
        }

        cout << endl;
} 


class task1 : public tbb::task {
    public :
      tbb::task* execute(){
          cout << "Task1 started ... " << endl;

	  long counter = 0;
	  while(true){
	      struct Node obj("Task1" , counter);
	      InsertToQueue(obj);
	      counter++;
	      std::this_thread::sleep_for (std::chrono::seconds(1));
	  }
	  return nullptr;
      }
};


class task2 : public tbb::task {
    public :
      tbb::task* execute(){
          cout << "Task2 started ... " << endl;

	  long counter = 0;
	  while(true){
	      struct Node obj("Task2" , counter);
	      InsertToQueue(obj);
	      counter++;
	      std::this_thread::sleep_for (std::chrono::seconds(1));
	  }
	  return nullptr;
      }
};


class task3 : public tbb::task {
    public :
      tbb::task* execute(){
          cout << "Task3 started ... " << endl;

	  long counter = 0;
	  while(true){
	      struct Node obj("Task3" , counter );
	      InsertToQueue(obj);
	      counter++;
	      std::this_thread::sleep_for (std::chrono::seconds(1));
	  }
	  return nullptr;
      }
};


class task4 : public tbb::task {
    public :
      tbb::task* execute(){
          cout << "Task4 started ... " << endl;

	  long counter = 0;
	  while(true){
	 	RemoveFromQueue();
	        std::this_thread::sleep_for (std::chrono::seconds(5));
	  }
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
          task4 * taskptr4 = new(tbb::task::allocate_child())task4;
         
	  tasklist.push_back(*taskptr1);
	  tasklist.push_back(*taskptr2);
	  tasklist.push_back(*taskptr3);
	  tasklist.push_back(*taskptr4);
            
	  // Number of children we span include the wait
	  tbb::task::set_ref_count(5);

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
