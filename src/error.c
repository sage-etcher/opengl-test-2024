#include "error.h"

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define FLAG_SET(var, flag) (var & flag)

static char *vsprintf_dup (const char *fmt, va_list args);

static FILE *g_stream_info    = NULL;
static FILE *g_stream_verbose = NULL;
static FILE *g_stream_debug   = NULL;
static FILE *g_stream_warn    = NULL;
static FILE *g_stream_error   = NULL;


static char *
vsprintf_dup (const char *fmt, va_list args)
{
    va_list args_copy;
    char *buffer = NULL;
    size_t length = 0;

    /* calculate length */
    va_copy (args_copy, args);
    length = vsnprintf (NULL, 0, fmt, args_copy);
    const size_t MAX_LENGTH = SIZE_MAX - 1;
    if (MAX_LENGTH < length)
    {
        errno = ERANGE;
        error_errno (errno);
        length = MAX_LENGTH;
        /* return NULL; */
    }

    /* allocate string */
    errno = 0;
    buffer = malloc (length + 1);
    if (!buffer)
    {
        error_errno (errno);
        return NULL;
    }

    /* write string */
    va_copy (args_copy, args);
    (void)vsnprintf (buffer, length + 1, fmt, args_copy);

    return buffer;
}


static void
write_log_msg (FILE *fp, const char *prefix, char *errmsg)
{
    if (fp == NULL) return;
    fprintf (fp, "%s: %s\n", prefix, errmsg);
}


static void
write_log_code (FILE *fp, const char *prefix, int errnum, char *errmsg)
{
    if (fp == NULL) return;
    fprintf (fp, "%s: %d: %s\n", prefix, errnum, errmsg);
}

static void
write_log_msg_fmt (FILE *fp, const char *prefix, const char *fmt, va_list args)
{
    char *errmsg = NULL;

    errmsg = vsprintf_dup (fmt, args);
    write_log_msg (fp, prefix, errmsg);

    free (errmsg);
    errmsg = NULL;
}

static void
write_log_code_fmt (FILE *fp, const char *prefix, int errnum, const char *fmt, va_list args)
{
    char *errmsg = NULL;

    errmsg = vsprintf_dup (fmt, args);
    write_log_code (fp, prefix, errnum, errmsg);

    free (errmsg);
    errmsg = NULL;
}


void
error_init (uint32_t flags)
{
    const FILE *default_stream_info    = stdout;
    const FILE *default_stream_verbose = stdout;
    const FILE *default_stream_debug   = stdout;
    const FILE *default_stream_warn    = stderr;
    const FILE *default_stream_error   = stderr;

    error_quit ();

    if (FLAG_SET (flags, LOG_INFO))    g_stream_info    = (FILE *)default_stream_info;
    if (FLAG_SET (flags, LOG_VERBOSE)) g_stream_verbose = (FILE *)default_stream_verbose;
    if (FLAG_SET (flags, LOG_DEBUG))   g_stream_debug   = (FILE *)default_stream_debug;
    if (FLAG_SET (flags, LOG_WARNING)) g_stream_warn    = (FILE *)default_stream_warn;
    if (FLAG_SET (flags, LOG_ERROR))   g_stream_error   = (FILE *)default_stream_error;

    return;
}

void
error_quit (void)
{
    g_stream_info    = NULL;
    g_stream_verbose = NULL;
    g_stream_debug   = NULL;
    g_stream_warn    = NULL;
    g_stream_error   = NULL;
}


/* generate functions for each type lol */
#define NEW_LOG_TYPE(fn_prefix, stream, msg_prefix)                           \
void                                                                          \
fn_prefix ## _code (int errnum, char *errmsg)                                 \
{                                                                             \
    write_log_code (stream, msg_prefix, errnum, errmsg);                      \
}                                                                             \
                                                                              \
                                                                              \
void                                                                          \
fn_prefix ## _msg (char *errmsg)                                              \
{                                                                             \
    write_log_msg (stream, msg_prefix, errmsg);                               \
}                                                                             \
                                                                              \
                                                                              \
void                                                                          \
fn_prefix ## _code_fmt (int errnum, const char *fmt, ...)                     \
{                                                                             \
    va_list args, args_copy;                                                             \
    va_start (args, fmt);                                                     \
    write_log_code_fmt (stream, msg_prefix, errnum, fmt, args);               \
}                                                                             \
                                                                              \
                                                                              \
void                                                                          \
fn_prefix ## _msg_fmt (const char *fmt, ...)                                  \
{                                                                             \
    va_list args;                                                             \
    va_start (args, fmt);                                                     \
    write_log_msg_fmt (stream, msg_prefix, fmt, args);                        \
}


NEW_LOG_TYPE(error,   g_stream_error,   "error")
NEW_LOG_TYPE(warn,    g_stream_warn,    "warning")
NEW_LOG_TYPE(debug,   g_stream_debug,   "debug")
NEW_LOG_TYPE(verbose, g_stream_verbose, "verbose")
NEW_LOG_TYPE(info,    g_stream_info,    "info")



/* exits */
_Noreturn void
fatal (void)
{
    exit (EXIT_FAILURE);
}


/* extensions */
void
error_errno (int errnum)
{
    char *errmsg = (errnum == 0 ? "Unknown error"
                                : strerror (errnum));

    error_code (errnum, errmsg);
}


/* end of file */
