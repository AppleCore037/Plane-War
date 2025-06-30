#define SDL_MAIN_HANDLED
#include "baseKit.h"
#include "settings.h"
#include "resources_manager.h"

#include "menu_scene.h"
#include "game_scene.h"
#include "scene_manager.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Event event;
SDL_Surface* surf_icon = nullptr;

bool is_running = true;
bool is_bombed = false;
Camera* camera = nullptr;
Clock frame_clock;
GameScene* game_scene = nullptr;

// SDL初始化
void init_SDL()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
	Mix_Init(MIX_INIT_WAVPACK | MIX_INIT_MP3);
	TTF_Init();

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

	window = SDL_CreateWindow(
		TITLE.c_str(), SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN
	);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	surf_icon = IMG_Load("resource/icon.png");

	SDL_SetWindowIcon(window, surf_icon);
}

// 游戏设置初始化
void init_gameSetup()
{
	ResLoader::instance()->load(renderer);
	camera = new Camera(renderer);
	camera->set_position(0, 0);

	game_scene = new GameScene();
	SceneManager::instance()->create_scene(SceneManager::SceneType::Menu, new MenuScene());
	SceneManager::instance()->create_scene(SceneManager::SceneType::Game, game_scene);
	SceneManager::instance()->set_current_scene(SceneManager::SceneType::Menu);
}

// 释放资源
void release()
{
	delete camera;

	SDL_FreeSurface(surf_icon);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	TTF_Quit();
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}

int main()
{
	SDL_SetMainReady();

	init_SDL();
	init_gameSetup();
	frame_clock.setFPS(FPS);

	Mix_Volume(-1, 80);

	// 游戏主循环
	while (is_running)
	{
		frame_clock.start_frame();

		// 处理消息
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				is_running = false;
				break;
			}
			if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_ESCAPE)
				{
					is_running = false;
					break;
				}
			}

			SceneManager::instance()->get_current_scene()->on_input(event);
		}

		// 处理数据
		float delta = frame_clock.getDeltaTime();
		SceneManager::instance()->get_current_scene()->on_update(delta);

		// 更新画面
		SDL_RenderClear(renderer);
		SceneManager::instance()->get_current_scene()->on_render(*camera);
		SDL_RenderPresent(renderer);

		frame_clock.end_frame();
	}

	release();
	return 0;
}