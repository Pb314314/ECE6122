/*
Author: Bo Pang
Class:  ECE6122  2023-11-6
Description: This contains the implementation of Lab4.
*/
#include <iostream>
#include <ctime>
#include <curand_kernel.h>
#include <iostream>
#include <cmath>
#include <chrono>

#define NUM_BLOCKS 256
#define THREADS_PER_BLOCK 256

#define RADIUS 1.0
using namespace std;
__global__ void calculate_distance(float* results, unsigned int seed, int step_num) 
{
    int tid = threadIdx.x;
    curandState state;
    curand_init(seed, tid, 0, &state);

    int x = 0;
    int y = 0;
    for (int i = 0; i < step_num; ++i) {
        float random = curand_uniform(&state);
        if(random < 0.25) x++;
        else if(random < 0.5) x--;
        else if(random < 0.75) y++;
        else if(random < 1.00) y--;
    }

    results[tid] = x*x + y*y;
}

void cudaMalloc_random_walk(int user_num, int step_num){
    auto start_time  = std::chrono::high_resolution_clock::now();

    float* d_results;
    float* h_results = new float[user_num];

    cudaMalloc((void**)&d_results, sizeof(float) * user_num);

    calculate_distance<<<1, user_num>>>(d_results, time(NULL), step_num); // every thread simulate one random walk
    cudaMemcpy(h_results, d_results, sizeof(float) * user_num, cudaMemcpyDeviceToHost);

    float distance = 0;
    for (int i = 0; i < user_num; ++i) {
        //cout<< sqrt(h_results[i]) <<endl;
        distance += sqrt(h_results[i]);
    }
    distance /= user_num;

    
    cudaFree(d_results);
    delete[] h_results;

    auto end_time = std::chrono::high_resolution_clock::now();

    auto duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "    Time to calculate:(microseconds): " << duration_us.count() << std::endl;
    std::cout << "    Average distance from origin: " << distance << std::endl;
    return;
}

void cudaMallocHost_random_walk(int user_num, int step_num){
    auto start_time  = std::chrono::high_resolution_clock::now();

    float* d_results;
    float* h_results;
    cudaMallocHost(&h_results, sizeof(float) * user_num);
    cudaMallocHost(&d_results, sizeof(float) * user_num);

    calculate_distance<<<1, user_num>>>(d_results, time(NULL), step_num); // every thread simulate one random walk
    cudaMemcpy(h_results, d_results, sizeof(float) * user_num, cudaMemcpyDeviceToHost);

    float distance = 0;
    for (int i = 0; i < user_num; ++i) {
        //cout<< sqrt(h_results[i]) <<endl;
        distance += sqrt(h_results[i]);
    }
    distance /= user_num;

    
    cudaFreeHost(d_results);
    cudaFreeHost(h_results);

    auto end_time = std::chrono::high_resolution_clock::now();

    auto duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "    Time to calculate:(microseconds): " << duration_us.count() << std::endl;
    std::cout << "    Average distance from origin: " << distance << std::endl;
    return;
}


void cudaMallocManaged_random_walk(int user_num, int step_num){
    auto start_time  = std::chrono::high_resolution_clock::now();

    float* d_results;
    float* h_results;
    cudaMallocManaged(&h_results, sizeof(float) * user_num);
    cudaMallocManaged(&d_results, sizeof(float) * user_num);

    calculate_distance<<<1, user_num>>>(d_results, time(NULL), step_num); // every thread simulate one random walk
    cudaMemcpy(h_results, d_results, sizeof(float) * user_num, cudaMemcpyDeviceToHost);

    float distance = 0;
    for (int i = 0; i < user_num; ++i) {
        //cout<< sqrt(h_results[i]) <<endl;
        distance += sqrt(h_results[i]);
    }
    distance /= user_num;

    
    cudaFree(d_results);
    cudaFree(h_results);

    auto end_time = std::chrono::high_resolution_clock::now();

    auto duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "    Time to calculate:(microseconds): " << duration_us.count() << std::endl;
    std::cout << "    Average distance from origin: " << distance << std::endl;
    return;
}

int main(int argc, char** argv) 
{
    int user_num = 1000;
    int step_num = 1000;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-W") == 0 && i + 1 < argc) {
            user_num = atoi(argv[i + 1]);
        } else if (strcmp(argv[i], "-I") == 0 && i + 1 < argc) {
            step_num = atoi(argv[i + 1]);
        }
    }
    //cout<< "Users: "<< user_num <<" Steps: "<< step_num <<endl;

    int deviceCount;
    cudaGetDeviceCount(&deviceCount);

    if (deviceCount == 0) {
        std::cerr << "No CUDA devices found." << std::endl;
        return 1;
    }

    int integratedDevice = -1; // Initialize with an invalid device index

    for (int device = 0; device < deviceCount; ++device) {
        cudaDeviceProp deviceProps;
        cudaGetDeviceProperties(&deviceProps, device);

        if (!deviceProps.integrated) {
            integratedDevice = device;
            break; // Found an not integrated device
        }
    }
    if (integratedDevice == -1) {
        std::cerr << "No integrated GPU devices found." << std::endl;
        return 1;
    }
    cudaSetDevice(integratedDevice);

    cout<<"Normal CUDA memory Allocation:"<<endl;
    cudaMalloc_random_walk(user_num,step_num);

    cout<<"Pinned CUDA memory Allocation:"<<endl;
    cudaMallocHost_random_walk(user_num,step_num);
    
    cout<< "Managed CUDA memory Allocation:"<<endl;
    cudaMallocManaged_random_walk(user_num,step_num);
    return 0;
}
