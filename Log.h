#pragma once
#include <iostream>
#include <fstream>
#include <cstring>
#include <thread>
#include <chrono>
#include <mutex>
#include <cstdio>

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
	std::string m_strLogFile;
	std::ofstream m_ofLogFile;
	std::mutex m_lockFile, m_lockExit;
	std::thread m_threadMain;
	BlockingQueue<LogMsg> m_msgQueue;
	short m_levelLog, m_levelPrint;
	bool m_isExit;

	std::string getTime();
	std::string level2str(short level, bool character_only);
	void logThread();

public:
	Log(short default_loglevel = LEVEL_WARN, short default_printlevel = LEVEL_INFO);
	~Log();
	void push(std::string msg, short level = LEVEL_INFO);
	bool open(std::string filename);
	bool close();
};

