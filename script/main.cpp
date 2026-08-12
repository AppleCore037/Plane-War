#include <SDL3/SDL.h>

import FlexCore;
import MenuScene;
import GameScene;
import EndScene;
using namespace fce;

SDL_Surface* surf_icon = nullptr;

static void init()
{
	surf_icon = SDL_LoadSurface("assets/gfx/icon.png");
	SDL_SetWindowIcon(Main_Window, surf_icon);

	utils::Set_ScaleMode(ScaleMode::Best);
	ResourcesManager::instance()->load_resources("assets");
	Text::set_global_font(ResourcesManager::instance()->find_font("msyh"));

	SceneManager::instance()->add_scene("menu_scene", new MenuScene());
	SceneManager::instance()->add_scene("game_scene", new GameScene());
	SceneManager::instance()->add_scene("end_scene", new EndScene());
	SceneManager::instance()->set_current_scene("menu_scene");

	Game::Game_Camera->set_position({ 300, 450 });
}

static void deinit()
{
	SDL_DestroySurface(surf_icon);
}

int main()
{
	Game::Init_Graphic("Plane War", { 600, 900 }, { 600, 900 });
	Game::Init_Custom_Setup(init);

	Game::Main_Loop(60);

	Game::Release_Custom_Setup(deinit);
	Game::Release_Graphic();
}