#pragma once
#include <iostream>
#include <fstream>
#include <cstring>
#include <thread>
#include <chrono>
#include <mutex>
#include <cstdio>
#include <cstdarg>
#include <atomic>

#include "BlockingQueue.h"

enum LogLevel {
	LEVEL_VERBOSE,LEVEL_INFO,LEVEL_WARN,LEVEL_ERROR,LEVEL_FATAL,LEVEL_OFF
};

struct LogMsg {
	short m_LogLevel;
	std::string m_strTimestamp;
	std::string m_strLogMsg;
};

class Log {
private:
	std::ofstream m_ofLogFile;        // 日志文件输出流
	std::mutex m_lockFile;            // 文件操作互斥锁
	std::thread m_threadMain;         // 后台日志处理线程
	BlockingQueue<LogMsg> m_msgQueue; // 线程安全阻塞队列
	short m_levelLog, m_levelPrint;   // 文件和控制台日志级别阈值
	std::atomic<bool> m_exit_requested{ false }; // 线程退出标志

	std::string getTime();            // 获取当前时间戳
	std::string level2str(short level, bool character_only); // 级别转字符串
	void logThread();                 // 后台线程函数

public:
	Log(short default_loglevel = LEVEL_WARN, short default_printlevel = LEVEL_INFO);
	~Log();
	void push(short level, const char* msg, ...); // 添加日志（支持格式化）
	void set_level(short loglevel, short printlevel); // 设置日志级别
	bool open(std::string filename);   // 打开日志文件
	bool close();                      // 关闭日志文件
};

