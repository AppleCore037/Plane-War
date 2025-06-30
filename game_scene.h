#pragma once
#include "scene.h"
#include "scene_manager.h"
#include <SDL.h>
#include <SDL_mixer.h>

#include "background.h"
#include "object_manager.h"
#include "resources_manager.h"
#include "player.h"

extern SDL_Renderer* renderer;
extern Camera* camera;
extern SDL_Window* window;
extern bool is_bombed;
extern bool is_running;

class GameScene : public Scene
{
public:
	int bullet_level = 0;
	int score = 0;
	int bomb_count = 0;
	Player* player = nullptr;

public:
	GameScene() = default;
	~GameScene() = default;

	void on_enter() override
	{
		bullet_level = 1;
		bomb_count = 3;

		player = new Player();
		player->set_position(Vector2(SCREEN_WIDTH / 2.0, 600));
		player->set_layer(Render::Node, Collision::Player);

		ObjectManager::instance()->init_function();
		ObjectManager::instance()->create_node(new Background(Vector2(-5, 0)));
		ObjectManager::instance()->create_node(new Background(Vector2(-5, -SCREEN_HEIGHT)));
		ObjectManager::instance()->create_node(player);

		text_score = new Text(renderer);
		Mix_PlayChannel(-1, ResLoader::instance()->find_audio("game_music"), -1);
	}

	void on_update(float delta) override
	{
		if (player->get_health() == 0)
		{
			std::string message = u8"You are dead!\nyour final score is: " + std::to_string(score);
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, u8"Game Over", message.c_str(), window);

			this->on_exit();
			is_running = false;
		}

		camera->on_update(delta);
		ObjectManager::instance()->on_update(delta);
	}

	void on_render(const Camera& camera) override
	{
		// 绘制背景
		SDL_Texture* texture_bg = ResLoader::instance()->find_texture("background");
		SDL_Rect rect_bg = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
		SDL_RenderCopy(renderer, texture_bg, nullptr, &rect_bg);

		// 绘制游戏对象
		ObjectManager::instance()->on_render(camera);

		// 绘制生命值
		SDL_Texture* texture_health = ResLoader::instance()->find_texture("life");
		for (int i = 0; i < player->get_health(); i++)
		{
			SDL_FRect rect_health = { 55 * i, SCREEN_HEIGHT - 65, 55, 65 };
			SDL_RenderCopyExF(renderer, texture_health, nullptr, &rect_health, 0, nullptr, SDL_FLIP_NONE);
		}

		// 绘制分数
		text_score->set_color({ 0, 0, 0, 255 });
		text_score->set_text("Score: " + std::to_string(score));
		text_score->on_render(Vector2(0, 0));

		// 绘制炸弹数量
		SDL_Texture* texture_bomb = ResLoader::instance()->find_texture("bomb");
		SDL_Rect rect_bomb = { SCREEN_WIDTH - 115, SCREEN_HEIGHT - 57, 63, 57 };
		Text* text_bomb = new Text(renderer);

		text_bomb->set_size(36);
		text_bomb->set_color({ 0, 0, 0, 255 });
		text_bomb->set_text("x" + std::to_string(bomb_count));
		text_bomb->on_render(Vector2(SCREEN_WIDTH - 45, SCREEN_HEIGHT - 47));
		SDL_RenderCopy(renderer, texture_bomb, nullptr, &rect_bomb);
	}

	void on_input(const SDL_Event& event) override
	{
		if (event.type == SDL_KEYDOWN)
		{
			if (event.key.keysym.sym == SDLK_SPACE && bomb_count > 0)
			{
				Mix_PlayChannel(-1, ResLoader::instance()->find_audio("use_bomb"), 0);
				bomb_count--;
				is_bombed = true;
				camera->shake(10.0f, 1.0f);
			}
		}

		ObjectManager::instance()->on_input(event);
	}

	void on_exit() override
	{
		this->bomb_count = 3;
		this->score = 0;
		this->bullet_level = 1;

		Mix_FreeChunk(ResLoader::instance()->find_audio("game_music"));
		ObjectManager::instance()->release();
		delete text_score;
	}

private:
	Text* text_score = nullptr;
};