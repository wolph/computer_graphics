/*
 * Threading.hpp
 *
 *  Created on: 20 Jun 2014
 *      Author: rick
 *
 * Based on: http://progsch.net/wordpress/?p=81
 */

#pragma once
#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
using namespace std;

class ThreadPool {
public:
    ThreadPool(size_t);
    template<class F, class ... Args>
    auto enqueue(F&& f, Args&&... args)
    -> future<typename result_of<F(Args...)>::type>;
    ~ThreadPool();
	inline bool running() { return !stop_; }
	inline void stop() { stop_ = true; }

private:
    // need to keep track of threads so we can join them
    vector<thread> workers;
    // the task queue
    queue<function<void()> > tasks;

    // synchronization 
    mutex queue_mutex;
    condition_variable condition;
    bool stop_;
};

// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool(size_t threads) :
stop_(false) {
    for(size_t i = 0; i < threads; ++i)
        workers.emplace_back([this]{
         while(1) {
             unique_lock<std::mutex> lock(queue_mutex);
             while(!stop_ && tasks.empty()){
                 this->condition.wait(lock);
             }
             if(this->stop_ && tasks.empty()){
                 return;
             }
             function<void()> task(tasks.front());
             tasks.pop();
             lock.unlock();
             task();
         }
     });
}

// add new work item to the pool
template<class F, class ... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
-> std::future<typename result_of<F(Args...)>::type>{
    typedef typename result_of<F(Args...)>::type return_type;

    // don't allow enqueueing after stopping the pool
    if (stop_)
        throw runtime_error("enqueue on stopped ThreadPool");

    auto task = make_shared < packaged_task<return_type()>
    > (bind(forward < F > (f), forward<Args>(args)...));

    future<return_type> res = task->get_future();
    {
        unique_lock < mutex > lock(queue_mutex);
        tasks.push([task](){(*task)();});
    }
    condition.notify_one();
    return res;
}

// the destructor joins all threads
inline ThreadPool::~ThreadPool(){
    {
        unique_lock < mutex > lock(queue_mutex);
        stop_ = true;
    }

    condition.notify_all();
    for(size_t i = 0; i < workers.size(); ++i)
        workers[i].join();
}
