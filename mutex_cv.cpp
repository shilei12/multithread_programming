#include <thread>
#include <mutex>
#include <iostream>
#include <chrono>
#include <memory>
#include <condition_variable>

using namespace std;

struct _data
{
    bool ready;
    int32_t value;
};

_data data = { false, 0 };
std::mutex data_mutex;
std::condition_variable data_con;
 
int main()
{
    //生产数据的线程
    std::thread prepare_data_thread([](){
        std::this_thread::sleep_for(std::chrono::seconds(2));    //模拟生产过程
        // 注意构造ulock的过程实际上是持锁的过程
        std::unique_lock<std::mutex> ulock(data_mutex); // 和lock_guard相似， 都属于智能锁， 离开作用域之后就释放 实际上调用的是析构函数
        data.ready = true;                              // 但是持锁是会降低系统的性能的
        data.value = 1;
        ulock.unlock(); // 提前释放
        data_con.notify_one();
    });

    //消费数据的线程
    std::thread process_data_thread([](){
        cout << "heiehei" << endl;
        std::unique_lock<std::mutex> ulock(data_mutex); // 锁在26行被锁住， 等待持锁
        cout << "hahahahah" << endl;
        data_con.wait(ulock, [](){ return data.ready; }); // 先unlock释放锁， 等待notify之后在持锁

        std::cout << data.value << ' ' << data.ready << std::endl;
    });

    prepare_data_thread.join();
    process_data_thread.join();

    return 0;
}
