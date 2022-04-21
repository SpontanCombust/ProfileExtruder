#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

void handleInput(SDL_Event &event, bool &running) {
    if (event.type == SDL_QUIT) 
    {
        running = false;
    } 
    else if(event.type == SDL_KEYDOWN)
    {
        switch (event.key.keysym.sym)
        {
            case SDLK_ESCAPE:
                running = false;
                break;
            default:
                break;
        }
    }
}

int main(int argc, char const *argv[])
{
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window *window = SDL_CreateWindow(
        "Curves!", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
        WIN_WIDTH, WIN_HEIGHT,
        SDL_WINDOW_OPENGL
    );

    SDL_GLContext context = SDL_GL_CreateContext(window);



    GLenum err = glewInit();
    if(err != GLEW_OK)
    {
        printf("glewInit Error: %s\n", glewGetErrorString(err));
        return 1;
    }

    glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    SDL_GL_SetSwapInterval(1);


    SDL_Event e;
    bool running = true;
    while(running)
    {
        while(SDL_PollEvent(&e))
        {
            handleInput(e, running);
        }
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        SDL_GL_SwapWindow(window);
    }

    SDL_Quit();

    return 0;
}
