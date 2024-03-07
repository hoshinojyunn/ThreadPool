#include "thread_pool.h"
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>

void ConcurrentQueue::enqueue(std::function<void()>f){
    std::unique_lock<std::mutex>lock{this->queue_mutex};
    m_queue.emplace(f);
}

std::pair<std::shared_ptr<Func>, bool> 
ConcurrentQueue::dequeue(){
    std::unique_lock<std::mutex>lock{this->queue_mutex};
    if(m_queue.empty())
        return {nullptr, false};
    auto func_ptr = std::make_shared<Func>(std::move(m_queue.front()));
    m_queue.pop();
    return {func_ptr, true};
}

size_t ConcurrentQueue::size(){
    std::unique_lock<std::mutex>lock{this->queue_mutex};
    return m_queue.size();
}

bool ConcurrentQueue::empty(){
    std::unique_lock<std::mutex>lock{this->queue_mutex};
    return m_queue.empty();
}

ThreadPool::ThreadPool(unsigned int pool_size){
    this->workers.resize(pool_size);
    this->stop = false;
    auto predict = [this](){
        return this->stop || !this->tasks.empty();
    };
    auto func = [this, predict](){    
        while(true){
            std::unique_lock<std::mutex>lock{this->condition_mutex};
            // 若没有任务则阻塞
            
            // 如果运行到这里 会判断predict是否为ture
            // 为true(有任务)则不会等待 继续往下运行
            // 为false(任务队列空) 则解锁互斥量lock 并阻塞当前线程
            // 若第二个参数不填则则与填false一样
            this->condition.wait(lock, predict);
            // 处理完所有任务才能退出
            if(this->stop && this->tasks.empty()){
                return;
            }
            auto [func_ptr, ok] = this->tasks.dequeue();
            lock.unlock();
            if(ok)
                (*func_ptr)();
        }
    };
    for(unsigned int i=0;i<pool_size; ++i){

        this->workers[i] = std::thread(func);
    }
}

ThreadPool::~ThreadPool(){
    this->stop = true;
    // 将所有阻塞的线程唤醒
    this->condition.notify_all();
    // std::this_thread::sleep_for(200ms);
    // 所有线程合并 回收资源
    for(auto&worker : this->workers){
        std::cout << "dtor thread id:" << worker.get_id() << '\n';
        if(worker.joinable())
            worker.join();
    }
    
}



