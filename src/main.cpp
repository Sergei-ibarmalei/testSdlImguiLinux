#ifdef _WIN32
#include <SDL.h>
#include <SDL_image.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#endif

//#include <SDL.h>
//#include <SDL_image.h>

#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"

#include <cstdio>
#include <iostream>


static float  GetDpiScale(SDL_Window* w, SDL_Renderer* r)
{
    int win_w, win_h, out_w, out_h, pw, ph;
    SDL_GetWindowSize(w, &win_w, &win_h);
    std::cout << "Window size is: [" << win_w << ", " << win_h << "]\n";
    SDL_GetRendererOutputSize(r, &out_w, &out_h);
    std::cout << "Render output size is: [" << out_w << ", " << out_h << "]\n";
    SDL_GetWindowSizeInPixels(w, &pw, &ph);
    std::cout << "Pixels size: [" << pw << ", " << ph << "]\n";
    return (win_w > 0) ? (float)out_w / (float)win_w : 1.0f;
}



int main(int, char**)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        std::printf("SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    const int imgFlags = IMG_INIT_PNG; 
    if ((IMG_Init(imgFlags) & imgFlags) != imgFlags)
    {
        std::printf("IMG_Init failed: %s\n", IMG_GetError());
        // not fatal for this ImGui smoke test
    }

    SDL_Window* window = SDL_CreateWindow(
        "testSdlImguiLinux",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 720,
        SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI
    );

    if (!window)
    {
        std::printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!renderer)
    {
        std::printf("SDL_CreateRenderer failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    const float scale = GetDpiScale(window, renderer);
    const float baseFontPx = 16.0f;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); //(void)io;
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(scale);

    io.Fonts->Clear();
    // Важно: пересоздать текстуру шрифтов для SDLRenderer2 бэкенда
    ImGui_ImplSDLRenderer2_DestroyDeviceObjects();
    io.Fonts->AddFontFromFileTTF("assets/Roboto-Medium.ttf", baseFontPx * scale);
    io.FontGlobalScale = 1.0f;
    //io.Fonts->Build();




    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

#ifdef ENABLE_IMGUI_DEMO
    bool showDemo = true;
#endif

    bool running = true;
    while (running)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                {
                    running = false;
                    break;
                }
                default: {}
                }
            }
            ImGui_ImplSDL2_ProcessEvent(&e);
            if (e.type == SDL_QUIT)
                running = false;
            if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE)
                running = false;
        }

        // Typical order for SDL2 + SDLRenderer2 backends
        ImGui_ImplSDL2_NewFrame();
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("ImGui + SDL2 smoke test");
        ImGui::Text("Project: %s", "testSdlImguiLinux");
#ifdef ENABLE_IMGUI_DEMO
        ImGui::Checkbox("Show Demo Window", &showDemo);
        ImGui::Text("Demo is ENABLED (compile-time).");
#else
        ImGui::Text("Demo is DISABLED (compile-time).");
#endif
        ImGui::Text("If you see this window: OK :)");
        ImGui::End();

#ifdef ENABLE_IMGUI_DEMO
        if (showDemo)
            ImGui::ShowDemoWindow(&showDemo);
#endif

        ImGui::Render();

        SDL_SetRenderDrawColor(renderer, 20, 20, 25, 255);
        SDL_RenderClear(renderer);

        // Your backend expects 2 args:
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);

        SDL_RenderPresent(renderer);
    }

    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    IMG_Quit();
    SDL_Quit();
    return 0;
}