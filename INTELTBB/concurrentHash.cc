#include "tbb/concurrent_hash_map.h"
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

// Structure that defines hashing and comparison operations for user's type.
struct MyHashCompare {
    static size_t hash( const string& x ) {
        size_t h = 0;
        for( const char* s = x.c_str(); *s; ++s )
            h = (h*17)^*s;
        return h;
    }

    //! True if strings are equal
    static bool equal( const string& x, const string& y ) {
        return x==y;
    }
};
 

// A concurrent hash table that maps strings to ints.
concurrent_hash_map<string, int, MyHashCompare> Hash;

void InsertToHash(int i){
     concurrent_hash_map<string, int, MyHashCompare>::accessor ac;
     string str = "key" + to_string(i);
     Hash.insert(ac, str);   // creates by default if not exists, acquires lock
     ac->second += i;        // new or old entry, add to vector anyway
     ac.release();

     // Display the occurrences
     for(auto iter = Hash.begin(); iter != Hash.end(); ++iter )
         cout << "First Value :  " << iter->first.c_str() << "   Second value :   "  << iter->second << endl;

    
} // the accessor's lock is released here


void RemoveFromHash(int i){
     string str = "key" + to_string(i);
     concurrent_hash_map<string, int, MyHashCompare>::accessor ac;
     bool found = Hash.find(ac, str); // so get the lock
     if (found){
	 Hash.erase(ac); // erase by accessor
     }

     ac.release();

     // Display the occurrences
     for(auto iter = Hash.begin(); iter != Hash.end(); ++iter )
         cout << "First Value :  " << iter->first.c_str() << "   Second value :   "  << iter->second << endl;
    
} // the accessor's lock is released here


class task1 : public tbb::task {
    public :
      tbb::task* execute(){
	  std::this_thread::sleep_for (std::chrono::seconds(7));
	  InsertToHash(1);
          cout << "task1 finished ... " << endl;
	  return nullptr;
      }
};


class task2 : public tbb::task {
    public :
      tbb::task* execute(){
	  std::this_thread::sleep_for (std::chrono::seconds(1));
	  InsertToHash(2);
          cout << "task2 finished ... " << endl;
	  return nullptr;
      }
};


class task3 : public tbb::task {
    public :
      tbb::task* execute(){
	  std::this_thread::sleep_for (std::chrono::seconds(5));
	  InsertToHash(3);
          cout << "task3 finished ... " << endl;
	  return nullptr;
      }
};

class task4 : public tbb::task {
    public :
      tbb::task* execute(){
	  std::this_thread::sleep_for (std::chrono::seconds(10));
          RemoveFromHash(3);
          cout << "task4 finished ... " << endl;
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
