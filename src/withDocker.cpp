//#include "/Code/Classes/simpleSDL/simpleSDL.h"
#include "simpleSDL.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <iostream>
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"

constexpr int WINDOW_W {1800};
constexpr int WINDOW_H {1200};

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
		SDL_SetRenderDrawColor(renderer, 20, 20, 24, 255);
		SDL_RenderClear(renderer);

		ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
		SDL_RenderPresent(renderer);

	}
		// Shutdown
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();




	return 0;
}