

#include "error.h"
#include "game.h"
#include <stdlib.h>


int
main (int argc, char **argv)
{
    error_init (LOG_MODE_DEBUG);

    if (game_start ()) fatal ();

    error_quit ();
    exit (EXIT_SUCCESS);
}


/* end of file */
