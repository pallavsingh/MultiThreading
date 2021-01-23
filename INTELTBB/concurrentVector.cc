/***************************************************************************************
This example shows adding elements with the push_back operation using inbuilt safety of the container.
Several threads are simultaneously trying to push_back while concurrent_vector takes care of safety. 

Of course, the real proof of concurrency lies in the disorder of the output. :-)

***************************************************************************************/

#include <iostream>
#include <tbb/task_scheduler_init.h>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/concurrent_vector.h>
#include <tbb/blocked_range.h>

#define N 24

using namespace std;
using namespace tbb;

class FObject {
    private:
        concurrent_vector<int> &cv;

    public:
        FObject( concurrent_vector<int> & arg) : cv(arg) {}

        void operator( )(const blocked_range<size_t> & r) const {
             printf("%08d - %08d\n", r.begin(), r.end());
             for (size_t i = r.begin(); i != r.end( ); ++i ) {
                  cv.push_back(i);
             }
        }
};


void par_func(int x, int y, concurrent_vector<int> &cv){
    FObject ob(cv);
    parallel_for(blocked_range<size_t>(x, y, N/4), FObject(ob));
}


int main() {
    task_scheduler_init init(2);
    concurrent_vector<int> cv;

    par_func(0,N,cv);
    cout << cv.size() << endl;

    for ( int i=0; i<cv.size(); i++ ) {
        printf("cv[%8d] = %8d\n",0,cv[i]);
    }

    return 0;
}

