#pragma once
#include "baseKit.h"
#include "resources_manager.h"
#include "scene.h"
#include "scene_manager.h"
#include "settings.h"
#include <SDL.h>

extern SDL_Renderer* renderer;

class MenuScene : public Scene
{
public:
	MenuScene() = default;
	~MenuScene() = default;

	void on_enter() override 
	{
		SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND)); // 设置鼠标为手型
	}

	void on_exit() override {}
	void on_update(float delta) override {}

	void on_render(const Camera& camera) override
	{
		SDL_Texture* texture_bg = ResLoader::instance()->find_texture("background");
		SDL_FRect rect_bg = { 0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
		camera.render_texture(texture_bg, nullptr, &rect_bg, 0, nullptr);
		
		Text* text = new Text(renderer);
		text->set_color({ 0, 0, 0, 255 });
		text->set_size(30);
		text->set_text(u8"Click screen to start");
		text->on_render((SCREEN_WIDTH / 2.0f) - 135.0f, (SCREEN_HEIGHT / 2.0f) - 25.0f);
	}

	void on_input(const SDL_Event& event) override
	{
		if (event.type == SDL_MOUSEBUTTONDOWN)
		{
			//判断是否按下鼠标左键
			if (event.button.button == SDL_BUTTON_LEFT)
				SceneManager::instance()->switch_to(SceneManager::SceneType::Game);
		}
	}
};