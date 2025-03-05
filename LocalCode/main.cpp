#include <iostream>
#include <string>
int main() {
    std::string utf8Str = "南北绿灯亮";
    //std::string gbkStr = Utf8ToGbk(utf8Str);
    
    //std::cout << gbkStr << std::endl;  // 在 GBK 终端可能显示正常
    std::cout << utf8Str << std::endl;  // 在 GBK 终端可能显示正常
}
