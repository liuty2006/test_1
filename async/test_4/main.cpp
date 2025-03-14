#include <iostream>
#include <future>
#include <thread>

void compute(std::promise<int> p) {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    p.set_value(42);  // 设置值
}

int main() {
    std::promise<int> p;
    std::future<int> fut = p.get_future();

    std::thread t(compute, std::move(p));

    std::cout << "Waiting for result..." << std::endl;
    std::cout << "Result: " << fut.get() << std::endl;  // 阻塞直到获取结果

    t.join();
    return 0;
}

