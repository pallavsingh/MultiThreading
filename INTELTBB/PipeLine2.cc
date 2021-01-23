#include "tbb/concurrent_queue.h"
#include "tbb/blocked_range.h"
#include "tbb/parallel_for.h"
#include <tbb/task_scheduler_init.h>
#include <tbb/task.h>
#include <tbb/tick_count.h>
#include <iostream>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <string>

using namespace tbb;
using namespace std;


/******************************************************************************************
 
Pipelines are a simple execution concept. Tasks to be executed come in a linear sequence, much like an assembly line. Each incoming piece of data is processed by the first element of the pipeline, then the second and so on. Obviously this is a serial process for any data element. Parallelism arises because we can have many pieces of data moving through the pipeline at the one time.

+---------+
|  Start  | <- data1, data2, data3, ..., dataN
+---------+
    |
    \/
+---------+
| Stage 1 |
+---------+
    |
    \/
+---------+
| Stage 2 |
+---------+
    |
    \/
+---------+
|  Stop   | -> outN, ... , out3, out2, out1
+---------+
In TBB each stage can either be serial or parallel. For serial stages no more than one element will be processed by this stage at a time. Serial stages can be in order or out of order. For parallel stages, multiple elements can be processed at the same time and the ordering is obviously not guaranteed.

Considering the performance one might hope to get with a pipeline, obviously the serial stages are bottlenecks (especially serial in order), so keeping these stages short will help a lot.


********************************************************************************************/

class DataReader {
private:
    FILE *my_input;

public:
    DataReader(FILE* in): my_input{in} {};
    DataReader(const DataReader& a): my_input{a.my_input} {};
   ~DataReader() {};

    double operator()(tbb::flow_control& fc) const {
        double number;
        int rc = fscanf(my_input, "%lf\n", &number);
        if (rc != 1) {
            fc.stop();
            return 0.0;
        }
        return number;
    }
};


class Transform {
public:
    double operator()(double const number) const {
        double answer=0.0;
        if (number > 0.0)
            answer = some_expensive_calculation(number)
        return answer;
    }
};


class DataWriter {
    private:
        FILE* my_output;
    
    public:
        DataWriter(FILE* out) : my_output{out} {};
    
        void operator()(double const answer) const {
            fprintf(my_output, "%lf\n", answer);
        }
};


void RunPipeline(int ntoken, FILE* input_file, FILE* output_file) {
    tbb::parallel_pipeline(
        			ntoken,
        			tbb::make_filter<void,double>(tbb::filter::serial_in_order, DataReader(input_file)) &
        			tbb::make_filter<double,double>(tbb::filter::parallel, Transform()) &
        			tbb::make_filter<double,void>(tbb::filter::serial_in_order, DataWriter(output_file)
			  );
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


string InputFileName  = "input.txt";
string OutputFileName = "output.txt";

int run_pipeline( int nthreads )
{
    FILE * input_file = fopen(InputFileName.c_str(), "r" );
    if (!input_file) {
        throw std::invalid_argument(("Invalid input file name: " + InputFileName).c_str());
        return 0;
    }


    FILE * output_file = fopen( OutputFileName.c_str(), "w" );
    if (!output_file) {
        throw std::invalid_argument( ("Invalid output file name: " + OutputFileName).c_str());
        return 0;
    }


    // Create the pipeline
    tbb::pipeline pipeline;


    // Create file-reading writing stage and add it to the pipeline
    MyInputFilter input_filter( input_file );
    pipeline.add_filter( input_filter );


    // Create squaring stage and add it to the pipeline
    MyTransformFilter transform_filter;
    pipeline.add_filter( transform_filter );


    // Create file-writing stage and add it to the pipeline
    MyOutputFilter output_filter( output_file );
    pipeline.add_filter( output_filter );


    // Run the pipeline
    tbb::tick_count t0 = tbb::tick_count::now();
    // Need more than one token in flight per thread to keep all threads
    // busy; 2-4 works
    pipeline.run( nthreads*4 );
    tbb::tick_count t1 = tbb::tick_count::now();

    fclose( output_file );
    fclose( input_file );

    if (!silent) 
	 printf("time = %g\n", (t1-t0).seconds());

    return 1;
}




int main(int argc, char * argv[]){
    int n = tbb::task_scheduler_init::default_num_threads();
    // Construct task scheduler with p threads
    tbb::task_scheduler_init init(10);
    tbb::tick_count t0 = tbb::tick_count::now();

    taskRoot * ptr = new(tbb::task::allocate_root())taskRoot;
    tbb::task::spawn_root_and_wait(*ptr);

    return 0;
}
