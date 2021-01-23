#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <mutex>

#include <tbb/parallel_for.h>
#include <tbb/parallel_reduce.h>

using namespace std;

int main(int argc, char **argv) {
    auto values = std::vector<double>(10000);
    tbb::parallel_for( tbb::blocked_range<int>(0, values.size()), [&](tbb::blocked_range<int> r)
                                                                    {
                                                                       for (int i = r.begin(); i < r.end(); ++i)
                                                                            values[i] = std::sin(i * 0.001);
                                                                    });

    mutex m;
    auto total = tbb::parallel_reduce( tbb::blocked_range<int>(0,values.size()), 0.0, [&](tbb::blocked_range<int> r, double running_total)
                                                                                          {
                                                                                              for (int i = r.begin(); i < r.end(); ++i)
                                                                                                   running_total += values[i];

                                                                                              m.lock();
                                                                                              cout << "Total now = " << running_total
                                                                                                   << " from iterations " << r.begin()
                                                                                                   << " to " << r.end() << endl;
                                                                                              m.unlock();

                                                                                              return running_total;
                                                                                          }, plus<double>() );

    cout << "Total : " << total << endl;

    return 0;
}
