#include <iostream>
#include <string>
#include <mutex>
#include <future>
#include <thread>

using namespace std;

static constexpr unsigned int predicateExpr  = 0;
static constexpr unsigned int predicateExpr1 = 1;
static constexpr unsigned int predicateExpr2 = 2;
static constexpr unsigned int predicateExpr3 = 3;

mutex  gmutex1;
mutex  gmutex2;
mutex  gmutex3;
condition_variable condvar;
unsigned int  condition = predicateExpr;
unsigned int  counter = 1;

int main (int argc, char * argv[]){

    thread T([](){
		      while(true){
                            cout << "Main Thread is geting executed" << endl;
			    gmutex1.lock();
			    std::this_thread::sleep_for(std::chrono::seconds(3));
			    gmutex2.lock();
		      }  
		  });


    thread T1([](){
		      while(true){

                            cout << "Processing Thread T1 " << endl;
		      }  
		  });



    thread T2([](){
		      while(true){
                            cout << "Processing Thread T2 " << endl;
			    gmutex3.lock();
			    std::this_thread::sleep_for(std::chrono::seconds(1));
		      }  
		  });


    thread T3([](){
		      while(true){
                            cout << "Processing Thread T3 " << endl;
			    gmutex2.lock();
			    std::this_thread::sleep_for(std::chrono::seconds(3));
			    gmutex1.lock();
		      }  
		  });


    T.join();
    T1.join();
    T2.join();
    T3.join();

    return 0;
}
