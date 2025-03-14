#include <iostream>
#include <future>

int compute() {
	std::cout << "compute ..." << std::endl;
    return 100;
}

int main() {
    std::future<int> result = std::async(std::launch::deferred, compute);
    
    // 任务尚未执行
    std::cout << "Before get()" << std::endl;

    // 调用 get() 时才执行 compute()
    std::cout << "Result: " << result.get() << std::endl;

    return 0;
}

