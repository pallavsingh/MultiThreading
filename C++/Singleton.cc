#include <atomic>
#include <iostream>
#include <future>
#include <mutex>
#include <thread>

using namespace std;

class singleton1{
  protected :
      singleton1() = default;
     ~singleton1() = default;
  private :
      singleton1(const singleton1 &) = delete;
      singleton1 & operator == (const singleton1 &) = delete;

  public :

      static singleton1 & getInstance(){
           static singleton1 obj;
           return obj;
      }

};


class singleton2{
  protected :
      singleton2() = default;
     ~singleton2() = default;
  private :
      singleton2(const singleton2 &) = delete;
      singleton2 & operator == (const singleton2 &) = delete;

      static atomic<struct singleton2 *> atomicptr;
      static mutex mymutex;

  public :

      static singleton2 * getInstance(){
	   struct singleton2 * ptr =  atomicptr.load(memory_order_acquire);
	   if (ptr == nullptr){
               lock_guard<mutex> lck(mymutex);
	       ptr =  atomicptr.load(memory_order_relaxed);
	       if (ptr == nullptr){
	           ptr = new singleton2();
	           atomicptr.store(ptr, memory_order_release);
               }
	   }

	   return ptr;
      }
};


atomic<struct singleton2 *> singleton2::atomicptr;
mutex singleton2::mymutex;


int main() {
    thread thread1 = thread([]() 
    			{
       				singleton1 & ptr = singleton1::getInstance();
       				cout <<"Address of Singleton1 in T1 : " << &ptr << endl;
    			});

    thread thread2 = thread([]() 
    			{
       				singleton1 & ptr = singleton1::getInstance();
       				cout <<"Address of Singleton1 in T2 : " << &ptr << endl;
    			});


    thread thread3 = thread([]() 
    			{
       				singleton1 & ptr = singleton1::getInstance();
       				cout <<"Address of Singleton1 in T3 : " << &ptr << endl;
    			});

    thread1.join();
    thread2.join();
    thread3.join();

    /////////////////////////////////////////////////////////////////////////////

    thread thread21 = thread([]() 
    			{
       				singleton2 * ptr = singleton2::getInstance();
       				cout <<"Address of Singleton1 in T1 : " << ptr << endl;
    			});

    thread thread22 = thread([]() 
    			{
       				singleton2 * ptr = singleton2::getInstance();
       				cout <<"Address of Singleton2 in T2 : " << ptr << endl;
    			});


    thread thread23 = thread([]() 
    			{
       				singleton2 * ptr = singleton2::getInstance();
       				cout <<"Address of Singleton2 in T3 : " << ptr << endl;
    			});

    thread21.join();
    thread22.join();
    thread23.join();

    return 0;
}
