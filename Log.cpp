#include "Log.h"

std::string Log::getTime() {
	using sc = std::chrono::system_clock;
	std::time_t t = sc::to_time_t(sc::now());
	char buf[20];
#ifdef _WIN32
	std::tm timeinfo;
	localtime_s(&timeinfo,&t);
	sprintf_s(buf, "%04d.%02d.%02d-%02d:%02d:%02d", 
		timeinfo.tm_year + 1900,
		timeinfo.tm_mon + 1,
		timeinfo.tm_mday,
		timeinfo.tm_hour,
		timeinfo.tm_min,
		timeinfo.tm_sec
	);
#else
	strftime(buf, 20, "%Y.%m.%d-%H:%M:%S", localtime(&t));
#endif
	return buf;
}

std::string Log::level2str(short level, bool character_only)
{
	switch (level) {
	case LEVEL_VERBOSE: 
		return character_only ? "V" : "Verbose";
	case LEVEL_WARN:
		return character_only ? "W" : "Warning";
	case LEVEL_ERROR:
		return character_only ? "E" : "Error";
	case LEVEL_FATAL:
		return character_only ? "F" : "Fatal";
	}
	return character_only ? "I" : "Info";
}

void Log::logThread() {
	while (true) {
		m_lockExit.lock();
		if (m_isExit) {
			m_lockExit.unlock();
			return;
		}
		m_lockExit.unlock();

		LogMsg front = m_msgQueue.front();
		if (front.m_LogLevel >= m_levelLog) {
			m_lockFile.lock();
			if (m_ofLogFile)
				m_ofLogFile << front.m_strTimestamp << ' ' << level2str(front.m_LogLevel, true) << ": " << front.m_strLogMsg << std::endl;
			m_lockFile.unlock();
		}
		if (front.m_LogLevel >= m_levelPrint)
			printf("%s %s: %s\n", front.m_strTimestamp.c_str(), level2str(front.m_LogLevel, true).c_str(), front.m_strLogMsg.c_str());
		
		m_msgQueue.pop();
	}
	return;
}

Log::Log(short default_loglevel, short default_printlevel) {
	m_levelLog = default_loglevel;
	m_levelPrint = default_printlevel;
	m_isExit = false;

	m_threadMain = std::thread(&Log::logThread, this);
}

Log::~Log() {
	m_lockExit.lock();
	m_isExit = true;
	m_lockExit.unlock();

	m_msgQueue.push({ LEVEL_INFO,getTime(),"Exit." });

	if (m_threadMain.joinable())
		m_threadMain.join();
}

void Log::push(std::string msg, short level) {
	m_msgQueue.push({level,getTime(),msg});
}

bool Log::open(std::string filename) {
	m_lockFile.lock();
	m_ofLogFile.open(filename.c_str(), std::ios::out);
	m_lockFile.unlock();
	return (bool)m_ofLogFile;
}

bool Log::close() {
	m_ofLogFile.close();
	return false;
}
