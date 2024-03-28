/*
Author: Bo Pang
Class:  ECE6122  2023-11-29
Description: This contains the implementation of Lab6.
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"
#include <string.h>
#include <cmath>

#define MASTER 0

int main(int argc, char *argv[]) {
    int i=0;
    double result = 0;
    double aggregate, num_result;
    int	taskid, numtasks, fun_id, point_num;

    MPI_Init(&argc, &argv); 
    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "-P") == 0) {
            fun_id = atoi(argv[i + 1]);
        } else if (strcmp(argv[i], "-N") == 0) {
            point_num = atoi(argv[i + 1]);
        }
    }
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
    printf("MPI task %d has started...\n", taskid);
    srandom(taskid);

    int point_process = point_num/numtasks;     //divide point to each process
    while(i<point_process){
        // Generate a random number between 0 and RAND_MAX
        int randomValue = rand();
        // Convert the random number to a value between 0 and 1
        double randomFraction = (double)randomValue / RAND_MAX;
        if(fun_id == 1)result += randomFraction*randomFraction; //x**2
        else if(fun_id == 2) result += exp(-pow(randomFraction, 2));    //second e**(-x**2)
        i++;
    }
    result/=point_process;
    int rc = MPI_Reduce(&result, &aggregate, 1, MPI_DOUBLE, MPI_SUM, MASTER, MPI_COMM_WORLD);

    // Print the result
    //printf("Random value: %d\n", randomValue);
    //printf("Random fraction: %f\n", randomFraction);
    if (taskid == MASTER)
    {
        num_result = aggregate / numtasks;
        printf("result: %f\n", num_result);
    }

    MPI_Finalize();
    return 0;
}
