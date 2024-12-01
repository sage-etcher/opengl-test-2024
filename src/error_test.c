
#include "error.h"
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int
main (int argc, char **argv)
{
    error_msg ("error before init");

    error_init (LOG_MODE_EVERYTHING);
    error_init (LOG_MODE_NOTHING);

    error_msg ("error msg");
    error_code (0, "error");
    error_msg_fmt ("error msg fmt %d", 0);
    error_code_fmt (0, "error code fmt %d", 0);
    error_errno (ERANGE);
    error_errno (0);

    warn_msg ("warn msg");
    warn_code (0, "warn");
    warn_msg_fmt ("warn msg fmt %d", 0);
    warn_code_fmt (0, "warn code fmt %d", 0);

    info_msg ("info msg");
    info_code (0, "info");
    info_msg_fmt ("info msg fmt %d", 0);
    info_code_fmt (0, "info code fmt %d", 0);

    verbose_msg ("verbose msg");
    verbose_code (0, "verbose");
    verbose_msg_fmt ("verbose msg fmt %d", 0);
    verbose_code_fmt (0, "verbose code fmt %d", 0);

    debug_msg ("debug msg");
    debug_code (0, "debug");
    debug_msg_fmt ("debug msg fmt %d", 0);
    debug_code_fmt (0, "debug code fmt %d", 0);

    error_init (LOG_MODE_EVERYTHING);

    char *fmt = NULL;
    char *string = NULL;
    size_t fmt_length = 0;
    size_t string_length = 0;

    errno = 0;
    string_length = UINT32_MAX + 2;
    string = malloc (string_length + 1);
    if (!string)
    {
        error_errno (errno);
        error_msg ("failed to allocate string\n");
        fatal ();
    }
    memset (string, '1', string_length);
    string[string_length] = '\0';

    errno = 0;
    fmt_length = (UINT32_MAX) * 2;
    fmt = malloc (fmt_length + 1);
    if (!fmt)
    {
        error_errno (errno);
        error_msg ("failed to allocate fmt\n");
        fatal ();
    }
    for (size_t i = 0; i < fmt_length; i+=2)
    {
        fmt[i+0] = '%';
        fmt[i+1] = 's';
    }
    fmt[fmt_length] = '\0';

    

    free (string); string = NULL; string_length = 0;
    free (fmt); fmt = NULL; fmt_length = 0;

    error_quit ();

    error_msg ("error after quit");

    fatal ();

    return EXIT_SUCCESS;
}
