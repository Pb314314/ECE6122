/*
Author: Bo Pang
Class:  ECE6122  2023-09-23
Description: This contains the definitions of class: threadpool.
*/
#include"threadpool.h"
// The constructor of threadpool;
threadpool::threadpool(int num_threads):stop(false), num_threads(num_threads){
    for(int i=0;i<num_threads;i++){
        this->threads.emplace_back([this](){
            this->fetch_task();
        });
    }
}
// Set stop to true and nodify all thread to get the mtx and stop;
threadpool::~threadpool(){
    {
    unique_lock<mutex> my_lock(mtx);
    stop = true;
    }
    cv.notify_all(); 
    for(auto &thr : threads){
        if(thr.joinable()) thr.join();
    }
}

// Pending task on the task vector and notify thread to do it;
void threadpool::add_task(function<void()> task){
    {// inside is the critical section
    unique_lock<mutex> my_lock(mtx);
    tasks.push(move(task));
    }
    cv.notify_one();
    
}
// The thread function of the threads;
void threadpool::fetch_task(){
    while(1){
        function<void()> new_task;
        {// inside is the critical section
        unique_lock<mutex> my_lock(mtx);
        cv.wait(my_lock,[this](){
            return tasks.size() || stop; // if has task continue to fetch
        } );
        //cout<< "Do the test!!" <<endl;
        if(stop && tasks.empty()) return;// if stop and no tasks end the thread
        new_task = move(tasks.front());
        tasks.pop();
        }
        new_task();
    }
}
// Check whether all the tasks are finished;
bool threadpool::task_empty(){
    {
        unique_lock<mutex> my_lock(mtx);
        return tasks.empty();
    }
    
} 