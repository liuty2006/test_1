#include <QCoreApplication>
#include <string>
#include <QDebug>
#include <plog/Log.h>
#include <plog/Init.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Appenders/ColorConsoleAppender.h>
#include "plog/Initializers/RollingFileInitializer.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    std::string logFilePath;
    logFilePath = "log.txt";

    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    //static plog::RollingFileAppender<plog::TxtFormatter> fileAppender(logFilePath.c_str(), 1024 * 1024, 5);   // 1024 * 1024 - 1MB
    //static plog::RollingFileAppender<plog::TxtFormatter> fileAppender(logFilePath.c_str(), 1024 * 1024 * 50, 10); // 1024 * 1024 - 50MB *10 =20分钟
    static plog::RollingFileAppender<plog::TxtFormatter> fileAppender(logFilePath.c_str(), 1024 * 1024 * 200, 10); // 1024 * 1024 - 200MB *10 =80分钟 ；
        // 5 -指定保留的日志文件数量。当日志文件达到最大大小时，会滚动创建新的日志文件，并保留指定数量的旧日志文件
    plog::Severity sev = (plog::Severity)5;
    plog::init(sev, &consoleAppender);
    plog::init(sev, &fileAppender);

    //
    std::string strMsg = "南北红灯_关";
    qDebug() << strMsg.c_str();
    PLOG_INFO << "std::string - msgid : " << strMsg;

    //
    QString strQMsg = "南北红灯_关";
    qDebug() << strQMsg;

    PLOG_INFO << "QString : " << strQMsg;
    return a.exec();
}
