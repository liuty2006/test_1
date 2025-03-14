#include <iostream>
#include <future>

// 一个异步执行的函数
int slowFunction() {
    std::cout << "slowFunction - start" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "slowFunction - end" << std::endl;
    return 42;
}

int main() {
    std::future<int> fut = std::async(std::launch::async, slowFunction);

    std::cout << "Waiting for result..." << std::endl;
    std::cout << "Result: " << fut.get() << std::endl;  // 获取值并输出

    std::cout << "end" << std::endl;
    return 0;
}

