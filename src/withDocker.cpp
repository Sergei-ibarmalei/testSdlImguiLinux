//#include "/Code/Classes/simpleSDL/simpleSDL.h"
#include "simpleSDL.h"
#include <imgui.h>
#include <iostream>
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"

constexpr int WINDOW_W {1280};
constexpr int WINDOW_H {720};

int main(int argc, char* argv[])
{
	Ssdl sdl {"SDL2 + ImGuiDocker", WINDOW_W, WINDOW_H};
	if (sdl.Status() == false) return 1;

	SDL_Window* window = sdl.Window();
	SDL_Renderer* renderer = sdl.Renderer();

	// Init imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	// запрещаем запись в ini file
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = nullptr;

	// подключаем Docking
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;


	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer2_Init(renderer);

	bool running = true;
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
		ImGui::DockSpace(dockspace_id, ImVec2(0, 0));

		ImGui::End();

		ImGui::PopStyleVar(2);

		ImGui::Begin("Hierarchy");
		ImGui::Text("Hello from Heirarchy");
		ImGui::End();

		ImGui::Begin("Inspector");
		ImGui::Text("Hello from Inspector");
		ImGui::End();

		ImGui::Begin("Viewport");
		ImGui::Text("Hello from Viewport");
		ImGui::End();


		// Render
		ImGui::Render();
		SDL_SetRenderDrawColor(renderer, 20, 20, 24, 255);
		SDL_RenderClear(renderer);

		ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
		SDL_RenderPresent(renderer);

	}
		// Shutdown
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();


	//SDL_DestroyRenderer(renderer);
	//SDL_DestroyWindow(window);
	//IMG_Quit();
	//SDL_Quit();

	return 0;
}