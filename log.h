#ifndef   _MACRO_LogModule
#define   _MACRO_LogModule
#include <stdio.h>
#include <pthread.h>
#include "macro_define.h"

typedef enum LogLevel {
	LL_DEBUG = 1,
	LL_TRACE = 2,
	LL_NOTICE = 3,
	LL_WARNING = 4,
	LL_ERROR = 5,
}LogLevel;

class LogWriter {
	public:
		LogWriter();
		~LogWriter();
		bool loginit(LogLevel l, const  char *filelocation, bool verbose, bool append = true, bool issync = false);
		bool log(LogLevel l,const char *logformat,...);
		LogLevel getlevel();
		bool logclose();

	public:
		const static unsigned int M_LOG_PATH_LEN = 250;

	private:
		const char* logLevelToString(LogLevel l);
		bool checklevel(LogLevel l);
		int premakestr(char* m_buffer, LogLevel l);
		bool write(char *pbuffer, int len);

	private:
		const static unsigned int M_LOG_BUFFSIZE = 1024*1024*4;
		const static unsigned int M_SYS_BUFFSIZE = 1024*1024*8;
		const static unsigned int M_LOG_MODULE_LEN = 32;
		enum LogLevel m_system_level;
		FILE* fp;
		bool m_issync;
		bool m_isappend;
		bool m_verbose;
		char m_filelocation[M_LOG_PATH_LEN];
		pthread_mutex_t m_mutex;
		static __thread char m_buffer[M_LOG_BUFFSIZE];
};

extern LogWriter WARN_W;
extern LogWriter INFO_W;

bool log_init(LogLevel l, const char* modulename, const char* logdir, bool verbose);
#endif
