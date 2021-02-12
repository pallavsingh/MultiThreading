#include <tbb/flow_graph.h>
#include <iostream>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds

using namespace std;
using namespace tbb::flow;

/****************************************************
 
 Flow Graph  ::  A ---> B ---> C

***************************************************/

void FlowGraph() {
     graph g;

     continue_node<continue_msg> A(g, [](const continue_msg &)
		                        {
					  cout << "A -->";
					}); 

     continue_node<continue_msg> B(g, [](const continue_msg &)
		                        {
					  cout << "B -->";
					}); 

     continue_node<continue_msg> C(g, [](const continue_msg &)
		                        {
					  cout << "C" << endl;
					}); 

    make_edge(A, B);
    make_edge(B, C);

    // Here we send messafe to Node A to spwan task and start computation.
    // when task is complete it send message to Node B for computation
    A.try_put(continue_msg());

    // Here we wait for All the Spawn task to be completed.
    g.wait_for_all();

    return;
}

/**********************************************************************
           --> A --> E
          |     \
  start---|      \
          |       \
           --> B --> C --> D
**********************************************************************/

struct Body {
    string name1;
    Body(const char * name) : name1(name){}
    void operator()(continue_msg) const {
         cout << name1.c_str() << "-->";
    }
};

void FlowGraph1() {
     graph g;

     broadcast_node<continue_msg> start(g);
     continue_node<continue_msg> A(g, Body("A"));
     continue_node<continue_msg> B(g, Body("B"));
     continue_node<continue_msg> C(g, Body("C"));
     continue_node<continue_msg> D(g, Body("D"));
     continue_node<continue_msg> E(g, Body("E"));
     
     make_edge(start, A);
     make_edge(start, B);
     make_edge(A, C);
     make_edge(B, C);
     make_edge(C, D);
     make_edge(A, E);

     // Here we send messafe to Node A to spwan task and start computation.
     // when task is complete it send message to Node B for computation
     start.try_put(continue_msg());

     // Here we wait for All the Spawn task to be completed.
     g.wait_for_all();

     return;
}


int main(int argc, char * argv[]){
    FlowGraph();
    FlowGraph1();

    return 0;
}
