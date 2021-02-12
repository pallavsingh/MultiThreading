#include <iostream>
#include <string>
#include <mutex>
#include <atomic>
#include <future>
#include <thread>
#include <climits>

using namespace std;


/*******************************************************************************************

////////////////////////////  Thread Execution Control Order /////////////////////////

                           |---- A
                           |
                |---- G1 --|---- B
                |          |
                |          |---- C
                |
-----> Sender ---
                |
                |          |---- X
                |          |
                |---- G2 --|---- Y
                           |
                           |---- Z

Generate Level Order Printing (Order of Threads Execution on Same Level donot depend)

> Sender
>>  G1, G2
>>>  A, B, C, D, E, F

********************************************************************************************/

static constexpr unsigned int predicateExpr  = 0;
static constexpr unsigned int predicateExpr1 = 1;
static constexpr unsigned int predicateExpr2 = 2;
static constexpr unsigned int predicateExpr3 = 3;
static constexpr unsigned int predicateExpr4 = 4;
static constexpr unsigned int predicateExpr5 = 5;
static constexpr unsigned int predicateExpr6 = 6;

bool  predicateExprG1 = false;
bool  predicateExprG2 = false;

unsigned int  condition = predicateExpr;
unsigned int  conditionG1 = INT_MAX;
unsigned int  conditionG2 = INT_MAX;

atomic<int> counterG1 (0);
atomic<int> counterG2 (0);

mutex  gmutex;
condition_variable condvar;

int main (int argc, char * argv[]){

    thread T([](){
		      while(true){
                          unique_lock<mutex> lck(gmutex);
                          condvar.wait(lck, [](){
					           return condition == predicateExpr; 
						});                    

                          cout << "<<<<<<<<<<< Main Thread  is geting executed >>>>>>>>>>>" << endl;
                          conditionG1 = predicateExpr1;
                          conditionG2 = predicateExpr4;
                           
			  predicateExprG1 = true;
                          predicateExprG2 = true;
			  condition = INT_MAX;

                          lck.unlock();
                          condvar.notify_all();
		      }  
		  });



    thread T1([](){
		      while(true){
                          unique_lock<mutex> lck(gmutex);
                          condvar.wait(lck, [](){
					           return predicateExprG1 && conditionG1 == predicateExpr1; 
						});                    

                          cout << "Processing Thread T1 " << endl;
			  ++counterG1;
                          conditionG1 = predicateExpr2;

                          lck.unlock();
                          condvar.notify_all();
		      }  
		  });


    thread T2([](){
		      while(true){
                          unique_lock<mutex> lck(gmutex);
                          condvar.wait(lck, [](){
					           return predicateExprG1 && conditionG1 == predicateExpr2; 
						});                    

                           cout << "Processing Thread T2 " << endl;
			   ++counterG1;
                           conditionG1 = predicateExpr3;

                           lck.unlock();
                           condvar.notify_all();
		      }  
		  });



    thread T3([](){
		      while(true){
                          unique_lock<mutex> lck(gmutex);
                          condvar.wait(lck, [](){
					           return predicateExprG1 && conditionG1 == predicateExpr3; 
						});                    

                          cout << "Processing Thread T3 " << endl;
			  ++counterG1;
			  predicateExprG1 = false;

			  if (counterG2 == 3){
                              counterG1 = 0;
                              counterG2 = 0;
                              condition = predicateExpr;
			      cout << "<<<<<<<< Syncpoint for G1 & G2 ::::: T3 >>>>>>>>>" << endl;
			  }

                          lck.unlock();
                          condvar.notify_all();
		      }  
		  });


    thread T4([](){
		      while(true){
                           unique_lock<mutex> lck(gmutex);
                           condvar.wait(lck, [](){
					           return predicateExprG2 && conditionG2 == predicateExpr4; 
						});                    

                           cout << "Processing Thread T4 " << endl;
			   ++counterG2;
			   conditionG2 = predicateExpr5;

                           lck.unlock();
                           condvar.notify_all();
		      }  
		  });


    thread T5([](){
		      while(true){
                           unique_lock<mutex> lck(gmutex);
                           condvar.wait(lck, [](){
					           return predicateExprG2 && conditionG2 == predicateExpr5; 
						});                    

                           cout << "Processing Thread T5 " << endl;
			   ++counterG2;
			   conditionG2 = predicateExpr6;

                           lck.unlock();
                           condvar.notify_all();
		      }  
		  });



    thread T6([](){
		      while(true){
                          unique_lock<mutex> lck(gmutex);
                          condvar.wait(lck, [](){
					           return predicateExprG2 && conditionG2 == predicateExpr6; 
						});                    

                          cout << "Processing Thread T6 " << endl;
			  ++counterG2;
			  predicateExprG2 = false;

			  if (counterG1 == 3){
                              counterG1 = 0;
                              counterG2 = 0;
                              condition = predicateExpr;
			      cout << "<<<<<<<< Syncpoint for G1 & G2 ::::: T6 >>>>>>>>>" << endl;
			  }

                          lck.unlock();
                          condvar.notify_all();
		      }  
		  });


    T.join();
    T1.join();
    T2.join();
    T3.join();
    T4.join();
    T5.join();
    T6.join();



    return 0;
}
