#ifndef  _MACRO_DEFINE
#define  _MACRO_DEFINE
//============basic===================

#define ZERROR(log_fmt, log_arg...) \
    do{ \
        WARN_W.log(LL_ERROR,   "[%s:%d] => " log_fmt "\n", \
                     __FILE__, __LINE__, ##log_arg); \
    } while (0)

#define ZWARN(log_fmt, log_arg...) \
    do{ \
        WARN_W.log(LL_WARNING,   "[%s:%d] => " log_fmt "\n", \
                     __FILE__, __LINE__, ##log_arg); \
    } while (0)

#define ZNOTICE(log_fmt, log_arg...) \
    do{ \
        INFO_W.log(LL_NOTICE,   "[%s:%d] => " log_fmt "\n", \
                     __FILE__, __LINE__, ##log_arg); \
    } while (0)

#define ZTRACE(log_fmt, log_arg...) \
    do{ \
        INFO_W.log(LL_TRACE,   "[%s:%d] => " log_fmt "\n", \
                     __FILE__, __LINE__, ##log_arg); \
    } while (0)

#define ZDEBUG(log_fmt, log_arg...) \
    do{ \
		INFO_W.log(LL_DEBUG,   "[%s:%d] => " log_fmt "\n", \
                     __FILE__, __LINE__, ##log_arg); \
    } while (0)

//============extend===================
#define MACRO_RET(condition, return_val) {\
    if (condition) {\
        return return_val;\
    }\
}

#define MACRO_WARN(condition, log_fmt, log_arg...) {\
    if (condition) {\
        ZWARN( log_fmt, ##log_arg);\
    }\
}

#define MACRO_WARN_RET(condition, return_val, log_fmt, log_arg...) {\
    if ((condition)) {\
        ZWARN( log_fmt, ##log_arg);\
		return return_val;\
    }\
}
#endif
