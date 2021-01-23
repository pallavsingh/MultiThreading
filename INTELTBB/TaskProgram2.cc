#include <tbb/task.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/tick_count.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

class task1 : public tbb::task {
    public :
      tbb::task* execute(){
          cout << "task1 is called " << endl;
	  return nullptr;
      }
};


class task2 : public tbb::task {
    public :
      tbb::task* execute(){
          cout << "task2 is called " << endl;
	  return nullptr;
      }
};


class task3 : public tbb::task {
    public :
      tbb::task* execute(){
          cout << "task3 is called " << endl;
	  return nullptr;
      }
};


int main(int argc, char * argv[]){
    int n = tbb::task_scheduler_init::default_num_threads();
    // Construct task scheduler with p threads
    tbb::task_scheduler_init init(10);
    tbb::tick_count t0 = tbb::tick_count::now();

    task1 obj1;
    task2 obj2;
    task3 obj3;

    obj1.execute();
    obj2.execute();
    obj3.execute();

    return 0;
}
