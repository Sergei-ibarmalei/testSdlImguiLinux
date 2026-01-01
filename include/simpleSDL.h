#ifdef _WIN32
    #include <SDL.h>
    #include <SDL_image.h>
#elif defined(__linux__)
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_image.h>
#else
    #error "Unsupported platform"
#endif

#define LOG
#ifdef LOG
    #include <iostream>
#endif

class Ssdl
{
    private:
        bool init {true};
        [[nodiscard]] bool initSimple(const char* appName, int w, int h);
        SDL_Window* window {nullptr};
        SDL_Renderer* renderer {nullptr};       

    public:
        Ssdl(const char* appName, int w, int h);
        ~Ssdl();
        Ssdl(const Ssdl&) = delete;
        Ssdl& operator=(const Ssdl&) = delete;
        Ssdl(Ssdl&&) = delete;
        Ssdl& operator=(Ssdl&&) = delete;

        SDL_Window* Window() const noexcept {return window;}
        SDL_Renderer* Renderer() const noexcept {return renderer;}

        bool Status() const {return init;}



};
