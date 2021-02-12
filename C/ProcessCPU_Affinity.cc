#include <stdio.h>
#include <math.h>
#include <sched.h>
 
double waste_time(long n)
{
    double res = 0;
    long i = 0;
    while(i < n * 200000) {
        i++;
        res += sqrt (i);
    }
    return res;
}
 
int main(int argc, char **argv) {
    cpu_set_t mask; /* processor 0 */
    CPU_ZERO(&mask);
    CPU_SET(0, &mask);
    if (sched_setaffinity(0, sizeof(mask), &mask) < 0) {
        perror("sched_setaffinity");
    }
 
    printf ("result: %f\n", waste_time (2000));
 
    CPU_ZERO(&mask);
    CPU_SET(1, &mask); /* process switches to processor 1 now */
    if (sched_setaffinity(0, sizeof(mask), &mask) < 0) {
        perror("sched_setaffinity");
    }
 
    printf ("result: %f\n", waste_time (2000));
}
