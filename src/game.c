#include "game.h"

#include "SDL_video.h"
#include "error.h"
#define NO_SDL_GLEXT
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <stdbool.h>



struct myrenderer
{
    GLuint vao;
    GLuint vbo;
    GLuint main_program;
};
typedef struct myrenderer myrenderer_t;


struct mygame
{
    SDL_Window *win;
    SDL_GLContext *ctx;
    myrenderer_t rend;
    int window_width;
    int window_height;
    bool window_fullscreen;
    bool runtime;
};
typedef struct mygame mygame_t;



static void error_sdl (void);
static void error_glew (GLenum errnum);
static void log_glew_versions (void);

static int game_init (mygame_t *game);
static void game_quit (mygame_t *game);

static void handle_keydown_event (mygame_t *game, SDL_Keysym *keysym);
static void process_events (mygame_t *game);
static void game_render (mygame_t *game);
static void game_run (mygame_t *game);


static void
error_sdl (void)
{
    error_msg ((char *)SDL_GetError ());
}


static void
error_glew (GLenum errnum)
{
    error_msg ((char *)glewGetErrorString (errnum));
}


static int
game_init (mygame_t *game)
{
    /* start SDL2 */
    const uint32_t SDL_FLAGS = (SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    if (SDL_Init (SDL_FLAGS))
    {
        error_sdl ();
        error_msg_fmt ("Cannot initialize SDL2 with %u.", SDL_FLAGS);
        return 1;
    }

    /* hint OpenGL context */
    SDL_GL_SetAttribute (SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute (SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute (SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    /* create window */
    const uint32_t WIN_FLAGS = (SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
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
    game->window_width  = initial_width;
    game->window_height = initial_height;
    game->window_fullscreen = false;

    /* start glew and opengl */
    game->ctx = SDL_GL_CreateContext (game->win);
    if (!game->ctx)
    {
        error_sdl ();
        error_msg ("cannot create opengl context.");
        return 1;
    }

    /* vsync */
    SDL_GL_SetSwapInterval (-1);

    /* glew init */
    glewExperimental = GL_TRUE;
    GLenum err = glewInit ();
    if (err != GLEW_OK)
    {
        error_glew (err);
        error_msg ("cannot initialize GLEW.");
        return 1;
    }

    log_glew_versions ();


    return 0;
}

static void
log_glew_versions (void)
{
    verbose_msg_fmt ("using GLEW %s", glewGetString (GLEW_VERSION));;

    if (glewIsSupported ("GL_VERSION_4_3"))
    {
        verbose_msg ("OpenGL 4.3 is supported");
    }
    else
    {
        warn_msg ("OpenGL 3.3 is not supported");
    }

    return;
}


static void
game_quit (mygame_t *game)
{
    SDL_GL_DeleteContext (game->ctx); game->ctx = NULL;
    SDL_DestroyWindow (game->win); game->win = NULL;
    SDL_Quit ();

    return;
}


static void
handle_key_down (mygame_t *game, SDL_Keysym *keysym)
{
    switch (keysym->sym)
    {
    case SDLK_ESCAPE:
        game->runtime = false;
    }
}


static void
process_events (mygame_t *game)
{
    SDL_Event e;

    while (SDL_PollEvent (&e))
    {
        switch (e.type)
        {
        case SDL_KEYDOWN:
            handle_key_down (game, &e.key.keysym);
            break;
        case SDL_QUIT:
            game->runtime = false;
            return;
        }
    }
}

static int
opengl_init (mygame_t *game)
{
    /* opengl init */
    glClearColor (0.5f, 0.5f, 0.5f, 1.0f);
    glCullFace (GL_BACK);
    glEnable (GL_CULL_FACE);
    glEnable (GL_DEPTH_TEST);
    glDisable (GL_STENCIL_TEST);

    const GLchar vertex_shader_source[] = {
        "#version 430 core\n"
        "layout(location = 0) in vec2 v2VertexPos2D;\n"
        "void main() \n"
        "{ gl_Position = vec4(v2VertexPos2D, 0.0f, 1.0f); }"
    };

    GLuint vertex_shader;
    if (!GL_LoadShader (vertex_shader, GL_VERTEX_SHADER, vertex_shader_source))
        return 1;

    const GLchar fragment_shader_source[] = {
        "#version 430 core\n"
        "out vec3 v3FragOutput;\n"
        "void main()\n"
        "    v3FragOutput = vec3(1.0f, 1.0f, 1.0f);\n"
        "}"
    };

    GLuint fragment_shader;
    if (!GL_LoadShader (fragment_shader, GL_FRAGMENT_SHADER, fragment_shader_source))
        return 1;

    if (!GL_LoadShaders (game->rend.main_program, vertex_shader, fragment_shader))
        return 1;

    glDeleteShader (vertex_shader);
    glDeleteShader (fragment_shader);
    glGenVertexArrays (1, &game->rend.vao);
    glBindVertexArray (game->rend.vao);
}

static void
game_render (mygame_t *game)
{

}


static void
game_run (mygame_t *game)
{
    game->runtime = true;

    while (1)
    {
        process_events (game);
        if (!game->runtime)
            break;

        game_render (game);

        SDL_GL_SwapWindow (game->win);
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
