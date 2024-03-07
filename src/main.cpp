#include "thread_pool.h"
#include <iostream>
#include <memory>
#include <thread>
// #include <chrono>


int main(){
    ThreadPool pool{4};
    auto func = [](int MAX){
        for(int i=0;i<MAX;++i){
            std::this_thread::sleep_for(200ms);
            std::cout << "thread" <<
            std::this_thread::get_id() << ":" << i << '\n'; 
        }
    };
    auto f1 = pool.submit(func, 10);
    auto f2 = pool.submit(func, 20);
    auto f3 = pool.submit(func, 30);
    auto f4 = pool.submit(func, 40);
    f1.get();
    f2.get();
    f3.get();
    f4.get();
    // std::shared_ptr<ThreadPool>ptr{&pool};
    // ptr.use_count()

}