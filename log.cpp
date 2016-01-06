#include "log.h"

#include <sys/file.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <libgen.h>

LogWriter WARN_W;
LogWriter INFO_W;
__thread char LogWriter::m_buffer[M_LOG_BUFFSIZE];

bool logInit(LogLevel l, const char* modulename, const char* logdir, bool verbose) {
	if (access(logdir, 0) == -1) {
		if (mkdir(logdir, S_IRWXU) < 0) {
			fprintf(stderr, "%s create folder failed\n", logdir);
			return false;
		}
	}

	time_t now = time(&now);;
	struct tm vtm;
    localtime_r(&now, &vtm);
	char location_str[LogWriter::M_LOG_PATH_LEN] = {0};
	snprintf(location_str, LogWriter::M_LOG_PATH_LEN, "%s/%s.info.%d%02d%02d-%02d",logdir,modulename,1900+vtm.tm_year,vtm.tm_mon + 1,vtm.tm_mday,vtm.tm_hour);
	INFO_W.loginit(l, location_str, verbose);
	snprintf(location_str, LogWriter::M_LOG_PATH_LEN, "%s/%s.error.%d%02d%02d-%02d", logdir,modulename,1900+vtm.tm_year,vtm.tm_mon + 1,vtm.tm_mday,vtm.tm_hour);
	if(l > LL_WARNING)
		WARN_W.loginit(l, location_str, verbose);
	else
		WARN_W.loginit(LL_WARNING, location_str, verbose);
	return true;
}

LogWriter::LogWriter() {
	m_system_level = LL_NOTICE;
	fp = NULL;
	m_issync = false;
	m_isappend = true;
	m_filelocation[0] ='\0';
	pthread_mutex_init(&m_mutex, NULL);
}

LogWriter::~LogWriter(){
	logclose();
}

const char* LogWriter::logLevelToString(LogLevel l) {
	switch ( l ) {
	case LL_DEBUG:
		return "DEBUG";
	case LL_TRACE:
		return "TRACE";
	case LL_NOTICE:
		return "NOTICE";
	case LL_WARNING:
		return "WARN" ;
	case LL_ERROR:
		return "ERROR";
	default:
		return "UNKNOWN";
	}
}

bool LogWriter::checklevel(LogLevel l) {
	if(l >= m_system_level)
		return true;
	else
		return false;
}

bool LogWriter::loginit(LogLevel l, const  char *filelocation, bool verbose, bool append, bool issync) {
	MACRO_RET(NULL != fp, false);
    m_system_level = l;
    m_isappend = append;
    m_issync = issync;
	m_verbose = verbose;
	if(strlen(filelocation) >= (sizeof(m_filelocation) -1)) {
		fprintf(stderr, "the path of log file is too long:%lu limit:%lu\n", strlen(filelocation), sizeof(m_filelocation) -1);
		exit(0);
	}

	strncpy(m_filelocation, filelocation, sizeof(m_filelocation));
	m_filelocation[sizeof(m_filelocation) -1] = '\0';

	if('\0' == m_filelocation[0]) {
		fp = stdout;
		fprintf(stderr, "now all the running-information are going to put to stderr\n");
		return true;
	}

	fp = fopen(m_filelocation, append ? "a":"w");
	if(fp == NULL) {
		fprintf(stderr, "cannot open log file,file location is %s\n", m_filelocation);
		exit(0);
	}
	//setvbuf (fp, io_cached_buf, _IOLBF, sizeof(io_cached_buf)); //buf set _IONBF  _IOLBF  _IOFBF
	setvbuf (fp,  (char *)NULL, _IOLBF, 0);
	fprintf(stderr, "now all the running-information are going to the file %s\n", m_filelocation);
	return true;
}

int LogWriter::premakestr(char* buffer, LogLevel l) {
    time_t now;
	now = time(&now);;
	struct tm vtm;
    localtime_r(&now, &vtm);
    return snprintf(buffer, M_LOG_BUFFSIZE, "[%d-%02d-%02d %02d:%02d:%02d] %s ",
            1900+vtm.tm_year, vtm.tm_mon + 1, vtm.tm_mday, vtm.tm_hour, vtm.tm_min, vtm.tm_sec, logLevelToString(l));
}

bool LogWriter::log(LogLevel l, const char* logformat,...) {
	MACRO_RET(!checklevel(l), false);

	char * start = m_buffer;
	int size = premakestr(start, l);

	va_list args;
	va_start(args, logformat);
	size += vsnprintf(start+size, M_LOG_BUFFSIZE-size, logformat, args);
	va_end(args);

	if(m_verbose)
		fprintf(stderr, "%s", m_buffer);

	if(fp)
		write(m_buffer, size);
	return true;
}

bool LogWriter::write(char *pbuffer, int len) {
	if(0 != access(m_filelocation, W_OK)) {
		pthread_mutex_lock(&m_mutex);
		if(0 != access(m_filelocation, W_OK)) {
			logclose();
			loginit(m_system_level, m_filelocation, m_isappend, m_issync);
		}
		pthread_mutex_unlock(&m_mutex);
	}

	if(1 == fwrite(pbuffer, len, 1, fp)) {
		if(m_issync)
          	fflush(fp);
		*pbuffer='\0';
    }
    else {
        int x = errno;
	    fprintf(stderr, "Failed to write to logfile. errno:%s    message:%s", strerror(x), pbuffer);
	    return false;
	}
	return true;
}

LogLevel LogWriter::getlevel() {
	return m_system_level;
}

bool LogWriter::logclose() {
	if(fp == NULL)
		return false;
	fflush(fp);
	fclose(fp);
	fp = NULL;
	return true;
}

void LogWriter::removeConsole() {
	m_verbose = false;
}
