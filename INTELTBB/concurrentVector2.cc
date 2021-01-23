#include <iostream>
#include <tbb/concurrent_vector.h>

using namespace std;
using namespace tbb;

void init_vec(concurrent_vector<int>  &cv){
    for (int i=0; i<cv.size(); i++) {
        cv[i] = i;
    }
}

void print_vec(concurrent_vector<int> &cv){
    for (int i=0; i<cv.size(); i++) {
        cout << i << " = " << cv[i] << endl;
    }
    cout << endl;
}


int main(){
    concurrent_vector<int> cv;

    cout << "default size: " << cv.size() << endl;

    cv.grow_by(10);
    cout << "grow_by 10: " << cv.size() << endl;

    cv.grow_to_at_least(5);
    cout << "grow_to_at_least 5: " << cv.size() << endl;

    init_vec(cv);
    cout << "initilize vector" << endl;

    cv.grow_to_at_least(15);
    cout << "grow_to_at_least 15: " << cv.size() << endl;

    cv.grow_by(5);
    cout << "grow_by 5: " << cv.size() << endl;

    print_vec(cv);

    cout << "push_back 5 times" << endl;
    for (int i=0; i<5; i++) {
        cout << "   pushing: " << 100+i << endl;
        cv.push_back(100+i);
    }
    print_vec(cv);

    return 0;
}
