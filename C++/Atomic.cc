#include <iostream>
#include <memory>
#include <cstring>
#include <thread>
#include <cassert>
#include <atomic>
#include <vector>

using namespace std;

int data = 100;

struct S{
    int a;
    string * str;

    S(int arg1, string * ptr) : a(arg1), str(ptr) {}
};

atomic<struct S *>  atomicptr;

atomic<bool> x{false};
atomic<bool> y{false};
atomic<int>  z{0};


int main(int argc, char * argv[]){
  
    thread T1([](){
		      string   * sptr = new string("PALLAVSINGH");
		      struct S * ptr  = new S(1, sptr); 
		      data = 200;
		      atomicptr.store(ptr, memory_order_release);
		  });

    thread T2([](){
		      struct S * ptr  = nullptr;
		      while(!(ptr = atomicptr.load(memory_order_acquire)))
		         ;
                      assert(data == 200);
                      assert(ptr->str && *(ptr->str) == "PALLAVSINGH");

		      cout << "value *(ptr->str) =  " << *(ptr->str) << endl;
		      cout << "value of data : " << data << endl;
		  });

    T1.join();
    T2.join();

    /////////////////////////////////////////////////////////////////////////////
    
    thread T3([](){
                        x.store(true, memory_order_seq_cst);		    
		  });
    
    thread T4([](){
                        y.store(true, memory_order_seq_cst);		    
		  });
    

    thread T5([](){  
                        while(!x.load(memory_order_seq_cst))
                          ;

                        if (y.load(memory_order_seq_cst))
                            ++z;			
		  });
    

    thread T6([](){  
                        while(!y.load(memory_order_seq_cst))
                          ;

                        if (x.load(memory_order_seq_cst))
                            ++z;			
		  });
    

    T3.join();
    T4.join();
    T5.join();
    T6.join();

    assert(z.load() != 0); // Will Never Happen
    cout << "value of Z(memory_order_seq_cst) : " << z << endl;


    return 0;
}
