//#include "/Code/Classes/simpleSDL/simpleSDL.h"
//#include "simpleSDL.h"
#include <SDL.h>
#include <SDL_image.h>
#include <memory>
#include <imgui.h>
#include <imgui_internal.h>
#include <iostream>
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"
#include "logs.h"


constexpr int WINDOW_W {1800};
constexpr int WINDOW_H {1200};

using WindowPtr = std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>;
using RenderPtr = std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)>;

[[nodiscard]] static bool InitSdl(WindowPtr& window_,
	RenderPtr& render_, int width, int height);
static float  GetDpiScale(SDL_Window* w, SDL_Renderer* r);


int main(int argc, char* argv[])
{

	WindowPtr window {nullptr, SDL_DestroyWindow};
	RenderPtr render {nullptr, SDL_DestroyRenderer};
	bool running = InitSdl(window, render, WINDOW_W, WINDOW_H);
	if (!running) return 1;

	const float dpiScale = GetDpiScale(window.get(), render.get());
	float uiScale = 1.0f;
	// Если запущено через X11/XWayland - делаем UI больше руками
    if (const char* vd = SDL_GetCurrentVideoDriver(); vd && std::strcmp(vd, "x11") == 0)
    {
        uiScale = 2.0f;
    }


	// Init imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();



    ImGuiStyle& style = ImGui::GetStyle();
    style = ImGuiStyle();          // чтобы не накапливать масштабы
    style.ScaleAllSizes(uiScale);  //  ВНИМАНИЕ - только uiScale, не dpiScale

	// запрещаем запись в ini file
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = nullptr;

    io.Fonts->Clear();
    const float baseFontPx = 16.0f;
    // Важно: пересоздать текстуру шрифтов для SDLRenderer2 бэкенда
    //ImGui_ImplSDLRenderer2_DestroyDeviceObjects();
    io.Fonts->AddFontFromFileTTF("assets/Roboto-Medium.ttf", baseFontPx * dpiScale * uiScale);

    // А чтобы “DPI” не сделал текст в 2 раза больше на Wayland — компенсируем глобальным масштабом:
    io.FontGlobalScale = 1.0f / dpiScale;

	// подключаем Docking
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;



	ImGui_ImplSDL2_InitForSDLRenderer(window.get(), render.get());
	ImGui_ImplSDLRenderer2_Init(render.get());


	SDL_Event e;

	while (running)
	{
		while (SDL_PollEvent(&e))
		{
			ImGui_ImplSDL2_ProcessEvent(&e);

			if (e.type == SDL_QUIT) running = false;
			if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE)
				running = false;
			else if (e.type == SDL_KEYDOWN)
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
		}

		// New Frame
		ImGui_ImplSDLRenderer2_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		// убираем отступы
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		ImGuiViewport* vp = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(vp->WorkPos, ImGuiCond_Always);
		ImGui::SetNextWindowSize(vp->WorkSize, ImGuiCond_Always);
		ImGui::SetNextWindowViewport(vp->ID);

		ImGuiWindowFlags host_flags =
			ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus |
			ImGuiWindowFlags_NoDocking;

		ImGui::Begin("DockHost", nullptr, host_flags);
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");

		// Опционально - флаг: даем центральной зоне быть "прозрачной"
		ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

		// Строим раскладку только один раз
		// Вариант без ini: можно просто сделать static bool
		static bool built = false;
		if (!built)
		{
			built = true;

			ImGui::DockBuilderRemoveNode(dockspace_id); // очистить старое дерево
			ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
			ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

			// Разрешаем - слева колонка, справа колонка, снизу консоль, центр - viewport
			ImGuiID dock_main_id = dockspace_id;

			ImGuiID dock_left_id = ImGui::DockBuilderSplitNode(dock_main_id, 
				ImGuiDir_Left, 0.22f, nullptr, &dock_main_id);
			ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dock_main_id,
				ImGuiDir_Right, 0.28f, nullptr, &dock_main_id);
			ImGuiID dock_bottom_id = ImGui::DockBuilderSplitNode(dock_main_id,
				ImGuiDir_Down, 0.25f, nullptr, &dock_main_id);

			// Доким окна по именам (имена должны совпадать с Begin("...")
			ImGui::DockBuilderDockWindow("Hierarchy", dock_left_id);
			ImGui::DockBuilderDockWindow("Inspector", dock_right_id);
			ImGui::DockBuilderDockWindow("Console", dock_bottom_id);
			ImGui::DockBuilderDockWindow("Viewport", dock_main_id);

			ImGui::DockBuilderFinish(dockspace_id);

		}
		ImGui::DockSpace(dockspace_id, ImVec2(0, 0), dockspace_flags);
		ImGui::End();

		ImGui::PopStyleVar(2);

		ImGui::Begin("Hierarchy");
		ImGui::Text("Heirarchy");
		ImGui::End();

		ImGui::Begin("Inspector");
		ImGui::Text("Inspector");
		ImGui::End();

		ImGui::Begin("Console");
		ImGui::Text("Console");
		ImGui::End();

		ImGui::Begin("Viewport");
		ImGui::Text("Viewport");
		ImGui::End();


		// Render
		ImGui::Render();
		SDL_SetRenderDrawColor(render.get(), 20, 20, 24, 255);
		SDL_RenderClear(render.get());
		ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), render.get());
		SDL_RenderPresent(render.get());

	}
		// Shutdown
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();


	render.reset();
	window.reset();
	IMG_Quit();
	SDL_Quit();

	return 0;
}

static float  GetDpiScale(SDL_Window* w, SDL_Renderer* r)
{
    int win_w, win_h, out_w, out_h, pw, ph;
    SDL_GetWindowSize(w, &win_w, &win_h);
#ifdef LOG
    std::cout << "Window size is: [" << win_w << ", " << win_h << "]\n";
#endif
    SDL_GetRendererOutputSize(r, &out_w, &out_h);
#ifdef LOG
    std::cout << "Render output size is: [" << out_w << ", " << out_h << "]\n";
#endif
    SDL_GetWindowSizeInPixels(w, &pw, &ph);
#ifdef LOG
    std::cout << "Pixels size: [" << pw << ", " << ph << "]\n";
#endif
    return (win_w > 0) ? (float)out_w / (float)win_w : 1.0f;
}

bool InitSdl(WindowPtr& window_, RenderPtr& render_, int width, int height)
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
#ifdef LOG
		std::cerr << "SDL_Init Error: " << SDL_GetError() << '\n';
#endif
		return false;
	}
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        std::cerr << "IMG_Init Error: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    window_.reset(SDL_CreateWindow(
        "Table", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN));

    if (!window_)
    {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        IMG_Quit();
        SDL_Quit();
        return false;
    }

    render_.reset(SDL_CreateRenderer(
        window_.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
    if (!render_)
    {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        window_.reset();
        IMG_Quit();
        SDL_Quit();
        return false;
    }

    // Проверка работы Vsync
    if (SDL_RenderSetVSync(render_.get(), 1) != 0)
    {
#ifdef LOG
        std::cout << "RenderSetVSync failed " << SDL_GetError() << '\n';
#endif
    }

    // Включаем альфа-смешивание для рендерера
    SDL_SetRenderDrawBlendMode(render_.get(), SDL_BLENDMODE_BLEND);

    return true;
}