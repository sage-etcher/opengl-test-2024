

#ifndef ERROR_HEADER
#define ERROR_HEADER

#include <stdarg.h>
#include <stdint.h>

enum error_flags {
    LOG_NONE    = 0,
    LOG_ERROR   = 1,
    LOG_WARNING = 2,
    LOG_INFO    = 4,
    LOG_VERBOSE = 8,
    LOG_DEBUG   = 16,
};

#define LOG_MODE_EVERYTHING (LOG_ERROR | LOG_INFO | LOG_WARNING | LOG_VERBOSE | LOG_DEBUG)
#define LOG_MODE_DEBUG      LOG_MODE_EVERYTHING
#define LOG_MODE_VERBOSE    (LOG_ERROR | LOG_INFO | LOG_WARNING | LOG_VERBOSE)
#define LOG_MODE_TERSE      (LOG_ERROR | LOG_INFO)
#define LOG_MODE_NOTHING    (LOG_NONE)


void error_init (uint32_t flags);
void error_quit (void);

void error_msg (char *errmsg);
void error_code (int errnum, char *errmsg);
void error_msg_fmt (const char *format, ...);
void error_code_fmt (int errnum, const char *format, ...);
void error_errno (int errnum);

void warn_msg (char *errmsg);
void warn_code (int errnum, char *errmsg);
void warn_msg_fmt (const char *format, ...);
void warn_code_fmt (int errnum, const char *format, ...);

void info_msg (char *errmsg);
void info_code (int errnum, char *errmsg);
void info_msg_fmt (const char *format, ...);
void info_code_fmt (int errnum, const char *format, ...);

void verbose_msg (char *errmsg);
void verbose_code (int errnum, char *errmsg);
void verbose_msg_fmt (const char *format, ...);
void verbose_code_fmt (int errnum, const char *format, ...);

void debug_msg (char *errmsg);
void debug_code (int errnum, char *errmsg);
void debug_msg_fmt (const char *format, ...);
void debug_code_fmt (int errnum, const char *format, ...);

_Noreturn void fatal (void);



#endif /* ERROR_HEADER */
/* end of file */
