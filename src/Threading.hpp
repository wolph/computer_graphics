/*
 * Threading.hpp
 *
 *  Created on: 20 Jun 2014
 *      Author: rick
 *
 * Based on: http://progsch.net/wordpress/?p=81
 */

#ifndef THREADING_HPP_
#define THREADING_HPP_

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

class ThreadPool{
public:
    ThreadPool(size_t);
    template<class F, class ... Args>
    auto enqueue(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type>;
    ~ThreadPool();
    inline bool running(){return !stop_;};
    inline void stop(){stop_ = true;};
private:
    // need to keep track of threads so we can join them
    std::vector<std::thread> workers;
    // the task queue
    std::queue<std::function<void()> > tasks;

    // synchronization 
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop_;
};

// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool(size_t threads) :
stop_(false){
    for(size_t i = 0;i < threads;++i)
        workers.emplace_back([this]{
         for(;;){
             std::unique_lock<std::mutex> lock(this->queue_mutex);
             while(!this->stop_ && this->tasks.empty()){
                 this->condition.wait(lock);
             }
             if(this->stop_ && this->tasks.empty()){
                 return;
             }
             std::function<void()> task(this->tasks.front());
             this->tasks.pop();
             lock.unlock();
             task();
         }
     });
}

// add new work item to the pool
template<class F, class ... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
-> std::future<typename std::result_of<F(Args...)>::type>{
    typedef typename std::result_of<F(Args...)>::type return_type;

    // don't allow enqueueing after stopping the pool
    if(stop_)
        throw std::runtime_error("enqueue on stopped ThreadPool");

    auto task = std::make_shared < std::packaged_task<return_type()>
    > (std::bind(std::forward < F > (f), std::forward<Args>(args)...));

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock < std::mutex > lock(queue_mutex);
        tasks.push([task](){(*task)();});
    }
    condition.notify_one();
    return res;
}

// the destructor joins all threads
inline ThreadPool::~ThreadPool(){
    {
        std::unique_lock < std::mutex > lock(queue_mutex);
        stop_ = true;
    }
    condition.notify_all();
    for(size_t i = 0;i < workers.size();++i)
        workers[i].join();
}

#endif /* THREADING_HPP_ */
