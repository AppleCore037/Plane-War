module;

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <string>
#include <format>

module GameScene;

import Background;
import Player;
import Enemy;
import Bullet;
import Supply;
import Buttons;

GameScene::GameScene()
{
	this->timer_enemy_spawner.set_wait_time(0.5f);
	this->timer_enemy_spawner.set_on_timeout([this]()
		{
			int type = Random::randint(1, 3);
			switch (type)
			{
			case 1:
				this->add_sprite(std::make_shared<SmallEnemy>());
				break;
			case 2: 
				if (score >= 1000 && mid_enemy_cnt < 16) // 限制中型飞机数量在15以内
				{
					this->mid_enemy_cnt++;
					this->add_sprite(std::make_shared<MiddleEnemy>());
				}
				break;
			case 3: 
				if (score >= 8000 && big_enemy_cnt < 5) // 限制大型飞机数量在4以内
				{
					this->big_enemy_cnt++;
					this->add_sprite(std::make_shared<BigEnemy>());
				}
				break;
			}
		});

	this->timer_supply_spawner.set_wait_time(13.0f);
	this->timer_supply_spawner.set_on_timeout([this]() 
		{ 
			Mix_PlayChannel(-1, ResourcesManager::instance()->find_audio("supply"), 0);
			this->add_sprite(std::make_shared<Supply>());
		});

	this->add_sprite(std::make_shared<Background>(0.0f));
	this->add_sprite(std::make_shared<Background>(-900.0f));
	this->add_sprite(std::make_shared<Player>());

	this->text_bomb = new Text();
	this->text_score = new Text();
	this->pause_button = new PauseButton();
}

GameScene::~GameScene()
{
	delete pause_button;
	delete text_score;
	delete text_bomb;
}

void GameScene::on_enter()
{
	Scene::on_enter();
	this->score = 0;
	this->big_enemy_cnt = 0;
	this->mid_enemy_cnt = 0;
	Mix_PlayMusic(ResourcesManager::instance()->find_music("game_music"), -1);
}

void GameScene::on_update(float delta)
{
	if (score >= 5000)
		this->timer_supply_spawner.on_update(delta);

	this->timer_enemy_spawner.on_update(delta);

	// 设置分数文字信息
	std::string score_str = std::to_string(score);
	std::u8string score_info = u8"分数:" + std::u8string(score_str.begin(), score_str.end());
	text_score->set_string(std::string(score_info.begin(), score_info.end()));

	Scene::on_update(delta);
}

void GameScene::on_render(const Camera& cam_game, const Camera& cam_ui)
{
	Renderer::fill({ 195, 200, 201, 255 });
	Scene::on_render(cam_game, cam_ui);
	
	this->pause_button->on_render(cam_ui);
	this->show_health(cam_ui);
	this->show_bomb(cam_ui);
	Renderer::render_text(cam_ui, { 10.0f, 10.0f }, text_score, 30, colors::Black);
}

void GameScene::on_input(const SDL_Event& event)
{
	this->pause_button->on_input(event);
	Scene::on_input(event);
}

void GameScene::on_exit()
{
	Mix_PauseMusic();
	this->find_group("enemies").range([this](std::shared_ptr<Sprite> sprite) { this->remove_spirte(sprite); });
	this->find_group("bullets").range([this](std::shared_ptr<Sprite> sprite) { this->remove_spirte(sprite); });
	this->find_group("supplies").range([this](std::shared_ptr<Sprite> sprite) { this->remove_spirte(sprite); });
}

void GameScene::plus_score(int val)
{
	this->score += val;
}

int GameScene::get_score() const
{
	return this->score;
}

void GameScene::show_health(const Camera& camera)
{
	auto player = this->find_sprite<Player>("Player");

	float w = 55.2f, h = 68.4f;
	for (int i = 0; i < player->get_health(); i++)
	{
		SDL_FRect life_rect = { 10.0f + w * i, 830.0f, w, h };
		Renderer::render_texture(camera, ResourcesManager::instance()->find_texture("life"), nullptr, &life_rect, 0);
	}
}

void GameScene::show_bomb(const Camera& camera)
{
	auto player = this->find_sprite<Player>("Player");

	SDL_FRect rect_icon = { 450.0f, 840.0f, 63.0f, 57.0f };
	Renderer::render_texture(camera, ResourcesManager::instance()->find_texture("bomb"), nullptr, &rect_icon, 0);

	this->text_bomb->set_string(std::format("x {}", player->get_bomb()));
	Renderer::render_text(camera, { 525.0f, 838.0f }, text_bomb, 40, colors::Black);
}