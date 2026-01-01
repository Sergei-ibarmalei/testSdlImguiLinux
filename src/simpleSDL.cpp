#include "simpleSDL.h"

Ssdl::Ssdl(const char* appName, int w, int h)
{
    if (!appName)
    {
#ifdef LOG
        std::cerr << "App name is absent, abort.\n";
#endif
        init = false; 
        return;
    }
    if (w <= 0 || h <= 0)
    {
#ifdef LOG
        std::cerr << "Window width or height are incorrect, abort.\n";
#endif
        init = false;
        return;
    }
    init = initSimple(appName, w, h);
}

bool Ssdl::initSimple(const char* appName, int w, int h)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
#ifdef LOG
        std::cerr << "SDL init failed, abort. " <<
            SDL_GetError() << '\n';
#endif
        return false;
    }
    if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) == 0)
    {
#ifdef LOG
        std::cerr << "SDL image failed, abort. " <<
            IMG_GetError() << '\n';
#endif
        return false;
    }

    window = SDL_CreateWindow(appName,
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            w,
            h,
            SDL_WINDOW_SHOWN);
    if (!window)
    {
#ifdef LOG
        std::cerr << "Window could not create, abort. " <<
            SDL_GetError() << '\n';
#endif
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer)
    {
#ifdef LOG
        std::cerr << "Renderer could not create, abort. " <<
            SDL_GetError() << '\n';
#endif
        return false;
    }
    return true;
}

Ssdl::~Ssdl()
{
    if (renderer) SDL_DestroyRenderer(renderer);
    renderer = nullptr;
    if (window) SDL_DestroyWindow(window);
    window = nullptr;
    IMG_Quit();
    SDL_Quit();
}
