#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <utility>
#include <vector>

using Func = std::function<void()>;
using namespace std::chrono_literals;
class ConcurrentQueue{
    std::queue<std::function<void()>>m_queue;
    std::mutex queue_mutex;
public:
    
    ConcurrentQueue()=default;
    void enqueue(std::function<void()>);
    std::pair<std::shared_ptr<std::function<void()>>, bool>dequeue();
    bool empty();
    size_t size();
};

class ThreadPool{
public:
    //std::thread::hardware_concurrency返回硬件支持的最大并发数
    ThreadPool(unsigned int pool_size=std::thread::hardware_concurrency());
    ~ThreadPool();
    ThreadPool(const ThreadPool&)=delete;
    ThreadPool(ThreadPool&&)=delete;
    template<class Func, class...Args>
    inline auto submit(Func&&func, Args&&...args);
private:
    std::vector<std::thread>workers;
    ConcurrentQueue tasks;
    std::condition_variable condition;
    std::mutex condition_mutex;
    std::mutex stop_mutex;
    // 只在线程池析构时为true 不需要互斥访问
    bool stop;
};


template<class Callable, class...Args>
auto ThreadPool::submit(Callable&&func, Args&&...args) {
    using reture_type = decltype(func(args...));
    //function->std::function<return_type()>
    auto function = std::bind(std::forward<Callable>(func), std::forward<Args>(args)...);

    auto task = std::make_shared<std::packaged_task<reture_type()>>(function);
    std::function<void()>wrap_func = [task](){
        (*task)();
    };
    if(this->stop)
        throw std::runtime_error{"Thread Pool has stopped, can not enqueue more tasks"};
    this->tasks.enqueue(wrap_func);
    // 提交一个任务 唤醒一个线程
    condition.notify_one();
    return task->get_future();

}
