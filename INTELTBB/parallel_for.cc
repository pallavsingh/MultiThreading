#include <iostream>
#include <vector>
#include <cmath>
#include <mutex>

#include <tbb/parallel_for.h>

using namespace std;

int main(int argc, char **argv){
    auto values = vector<double>(10000);
    mutex m;
    tbb::parallel_for(tbb::blocked_range<int>(0, values.size()), [&](tbb::blocked_range<int> r) {
                                                                          m.lock();
                                                                          cout << "Range size " << (r.end() - r.begin())
                                                                               << " from " << r.begin() << " to " << r.end() << endl;
                                                                          m.unlock();    
                                                             
                                                                          for (int i = r.begin(); i < r.end(); ++i) {
                                                                               values[i] = sin(i * 0.001);
                                                                        }
                                                                     });



    double total = 0;
    for (double value : values){
         total += value;
    }

    cout << "Value of total : " << total << endl;
    return 0;
}
