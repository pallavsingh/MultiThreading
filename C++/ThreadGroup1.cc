#include <iostream>
#include <string>
#include <mutex>
#include <atomic>
#include <future>
#include <thread>
#include <climits>

using namespace std;

/*******************************************************************************************

Flow Graph for Thread Scheduling


                    _ _ _ (T2, T3)_ _ _
                   |                  |             |-----( T7 )----|
                   |                  |             |               |
---> Sender(T1 ) --|                  |---( T6 ) ---|               |----( T9 ) ----  
       ^           |                  |             |               |              |
       |           |                  |             |------( T8 )----              |
       |            -----( T4  T5)-----                                            |
       |                                                                           |
        ----------------------------------------------------------------------------


********************************************************************************************/

static constexpr unsigned int predicateExpr1 = 1;
static constexpr unsigned int predicateExpr2 = 2;
static constexpr unsigned int predicateExpr3 = 3;
static constexpr unsigned int predicateExpr4 = 4;
static constexpr unsigned int predicateExpr5 = 5;
static constexpr unsigned int predicateExpr6 = 6;
static constexpr unsigned int predicateExpr7 = 7;
static constexpr unsigned int predicateExpr8 = 8;
static constexpr unsigned int predicateExpr9 = 9;

bool  predicateExprG1 = false;
bool  predicateExprG2 = false;
bool  predicateExprG3 = false;
bool  predicateExprG4 = false;

unsigned int  condition = predicateExpr1;
unsigned int  conditionG1 = INT_MAX;
unsigned int  conditionG2 = INT_MAX;
unsigned int  conditionG3 = INT_MAX;
unsigned int  conditionG4 = INT_MAX;

atomic<int> counterG1G2 (0);
atomic<int> counterG3G4 (0);

mutex  gmutex;
condition_variable condvar;

int main (int argc, char * argv[]){

    thread T1([](){
		      while(true){
                          unique_lock<mutex> lck(gmutex);
                          condvar.wait(lck, [](){
					           return condition == predicateExpr1; 
						});                    

                          cout << "Main Thread (T1) is geting executed" << endl;
                          conditionG1 = predicateExpr2;
                          conditionG2 = predicateExpr4;
                           
			  predicateExprG1 = true;
                          predicateExprG2 = true;
			  condition = INT_MAX;

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
                           conditionG1 = predicateExpr3;
			   ++counterG1G2;

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

			  predicateExprG1 = false;
			  ++counterG1G2;

			  if (counterG1G2 == 4){
                              counterG1G2 = 0;
                              condition = predicateExpr6;
			      cout << "<<<<<<<< Syncpoint for G1G2 >>>>>>>>>" << endl;
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
			   conditionG2 = predicateExpr5;
			   ++counterG1G2;

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
                           predicateExprG2 = false;
			   ++counterG1G2;

			   if (counterG1G2 == 4){
                               counterG1G2 = 0;
                               condition = predicateExpr6;
			       cout << "<<<<<<<< Syncpoint for G1G2 >>>>>>>>>" << endl;
			   }

                           lck.unlock();
                           condvar.notify_all();
		      }  
		  });



    thread T6([](){
		      while(true){
                          unique_lock<mutex> lck(gmutex);
                          condvar.wait(lck, [](){
					           return condition == predicateExpr6; 
						});                    

                          cout << "Processing Thread T6 " << endl;
			  predicateExprG3 = true;
                          predicateExprG4 = true;
			  conditionG3 = predicateExpr7;
			  conditionG4 = predicateExpr8;
			  condition = INT_MAX;

                          lck.unlock();
                          condvar.notify_all();
		      }  
		  });



    thread T7([](){
		      while(true){
                          unique_lock<mutex> lck(gmutex);
                          condvar.wait(lck, [](){
					           return predicateExprG3 && conditionG3 == predicateExpr7; 
						});                    

                           cout << "Processing Thread T7 " << endl;
			   predicateExprG3 = false;
			   condition = INT_MAX;
			   ++counterG3G4;

			   if (counterG3G4 == 2){
                               counterG3G4 = 0; 
			       condition = predicateExpr9;
			       cout << "<<<<<<<< Syncpoint for G3G4 >>>>>>>>>" << endl;
			   }

                           lck.unlock();
                           condvar.notify_all();
		      }  
		  });



    thread T8([](){
		      while(true){
                          unique_lock<mutex> lck(gmutex);
                          condvar.wait(lck, [](){
					           return predicateExprG4 && conditionG4 == predicateExpr8; 
						});                    

                          cout << "Processing Thread T8 " << endl;
                          predicateExprG4 = false;
			  condition = INT_MAX;
			   ++counterG3G4;

			  if (counterG3G4 == 2){
                               counterG3G4 = 0; 
			       condition = predicateExpr9;
			       cout << "<<<<<<<< Syncpoint for G3G4 >>>>>>>>>" << endl;
			  }

                          lck.unlock();
                          condvar.notify_all();
		      }  
		  });



    thread T9([](){
		      while(true){
                          unique_lock<mutex> lck(gmutex);
                          condvar.wait(lck, [](){
					           return condition == predicateExpr9; 
						});                    

                           cout << "Processing Thread T9 " << endl;
                           condition = predicateExpr1;

                           lck.unlock();
                           condvar.notify_all();
		      }  
		  });




    T1.join();
    T2.join();
    T3.join();
    T4.join();
    T5.join();
    T6.join();
    T7.join();
    T8.join();
    T9.join();



    return 0;
}
