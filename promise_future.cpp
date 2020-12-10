/* 并发编程使用面向对象的标准模板库的promise和future, 较少对锁和条件变量的使用， 降低复杂度
 *.
 */

#include <iostream>
#include <future>
#include <thread>
#include <chrono>

using namespace std;

struct Data {
    int value = 0;
    bool ready = false;
};

void DoSomething(promise<Data>& myPromise)
{
    std::this_thread::sleep_for(std::chrono::seconds(2));
    myPromise.set_value({2, true});
}

void GetValue(shared_future<Data>& future)
{
    // 注意future的get函数只能使用一次， 想要多次调用， 使用shared_future
    cout << future.get().value << ' ' << future.get().ready << endl; // 2 terminate called after throwing an instance of 'std::future_error'
                                                                     // what():  std::future_error: No associated state
}

int main()
{
    promise<Data> myPromise;
    shared_future<Data> future_ = myPromise.get_future();

    //thread t1(DoSomething, ref(myPromise));
    thread t1([](promise<Data>& myPromise) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        myPromise.set_value({2, true});
        }, ref(myPromise)); // 也可以直接使用lambda表达式来启动新的线程

    thread t2(GetValue, ref(future_));

    // if (t1.joinable()) {
    //     t1.join(); // 在线程对象销毁之前， 要指定线程是等待完成还是后台执行
    // }
    // if (t2.joinable()) {
    //     t2.join();
    // }
    t1.detach();
    t2.join();
    return 0;
}