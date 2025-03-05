#include <iostream>
#include <string>

#ifdef WIN32
#include <Windows.h>
std::string Utf8ToGbk(const std::string& utf8Str)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, nullptr, 0);
    std::wstring wstr(len, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, &wstr[0], len);

    len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string gbkStr(len, 0);
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, &gbkStr[0], len, nullptr, nullptr);

    return gbkStr;
}
#endif

int main() {
    std::string utf8Str = "南北绿灯亮";
    std::cout << utf8Str << std::endl;  // 在 GBK 终端可能显示正常

    //
    //QString strQT = "南北绿灯亮";
    //std::string utf8Str1 = strQT.toUtf8().toStdString();
#ifdef WIN32
    std::string gbkStr = Utf8ToGbk(utf8Str);
    std::cout << gbkStr << std::endl;  // 在 GBK 终端可能显示正常
#endif
}
