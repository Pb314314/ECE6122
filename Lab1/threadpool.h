/*
Author: Bo Pang
Class:  ECE6122  2023-09-23
Description: This contains the implementation of Lab1.
*/
#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>

using namespace std;

// build a threadpool class, which will autoly do the task in the taskqueue.
class threadpool{
    public:
        threadpool(int num);
        ~threadpool();
        vector<thread> threads;                 // detect and do the tasks          
        void add_task(function<void()> task);   // insert tasks    
        void fetch_task();
        bool task_empty();
    private:
        // shared resources
        queue<function<void()>> tasks;
        bool stop;
        int  num_threads;
        mutex mtx;
        condition_variable cv;
};