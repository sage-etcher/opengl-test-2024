#include "game.h"

#include "error.h"
#include <SDL2/SDL.h>


struct mygame
{
    SDL_Window *win;
    SDL_Renderer *rend;
    int runtime;
};
typedef struct mygame mygame_t;


static void error_sdl (void);

static int game_init (mygame_t *game);
static void game_quit (mygame_t *game);
static int game_init_sdl (void);
static int game_make_window (mygame_t *game);

static void game_handle_events (mygame_t *game);
static void game_run (mygame_t *game);


static void
error_sdl (void)
{
    error_msg ((char *)SDL_GetError ());
}


static int
game_init (mygame_t *game)
{
    if (game_init_sdl ()) return 1;
    if (game_make_window (game)) return 1;

    return 0;
}


static void
game_quit (mygame_t *game)
{
    SDL_DestroyRenderer (game->rend); game->rend = NULL;
    SDL_DestroyWindow (game->win); game->win = NULL;
    SDL_Quit ();

    return;
}


static int
game_make_window (mygame_t *game)
{
    const uint32_t WIN_FLAGS = (SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    const uint32_t REND_FLAGS = (SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    const int initial_width  = 640;
    const int initial_height = 480;

    game->win = SDL_CreateWindow ("opengl window",
                                 SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                 initial_width, initial_height,
                                 WIN_FLAGS);
    if (!game->win)
    {
        error_sdl ();
        error_msg_fmt ("Cannot create window with %u.", WIN_FLAGS);
        return 1;
    }

    game->rend = SDL_CreateRenderer (game->win, -1, REND_FLAGS);
    if (!game->rend)
    {
        error_sdl ();
        error_msg_fmt ("Cannot create renderer with %u.", REND_FLAGS);
        return 1;
    }

    return 0;
}


static int
game_init_sdl (void)
{
    const uint32_t SDL_FLAGS = (SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    if (SDL_Init (SDL_FLAGS))
    {
        error_sdl ();
        error_msg_fmt ("Cannot initialize SDL2 with %u.", SDL_FLAGS);
        return 1;
    }

    return 0;
}


static void
game_handle_events (mygame_t *game)
{
    SDL_Event e;

    while (SDL_PollEvent (&e))
    {
        switch (e.type)
        {
        case SDL_QUIT:
            game->runtime = 0;
        case SDL_KEYDOWN:
            switch (e.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                game->runtime = 0;
            }
        }
    }
}


static void
game_run (mygame_t *game)
{
    game->runtime = 1;

    while (1)
    {
        game_handle_events (game);
        if (!game->runtime)
            break;

        (void)SDL_SetRenderDrawColor (game->rend, 100, 100, 100, 255);
        (void)SDL_RenderClear (game->rend);

        SDL_RenderPresent (game->rend);
    }

    return;
}


int
game_start (void)
{
    mygame_t game = { 0 };

    if (game_init (&game))
    {
        error_msg ("cannot initialize the game.");
        return 1;
    }

    game_run(&game);

    game_quit (&game);
    return 0;
}



/* end of file */
