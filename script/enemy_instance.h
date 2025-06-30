#pragma once
#include "enemy.h"
#include "game_scene.h"
#include "object_manager.h"
#include "resources_manager.h"
#include "settings.h"
#include "supply.h"

extern GameScene* game_scene;

class SmallEnemy : public Enemy
{
public:
	SmallEnemy()
	{
		this->size = Vector2(57.0f, 43.0f);
		this->max_health = SMALL_ENEMY_HEALTH;

		this->health = SMALL_ENEMY_HEALTH;
		this->speed = SMALL_ENEMY_SPEED;
		this->anim_idle.add_frame(ResLoader::instance()->find_texture("enemy1"), 1);

		this->anim_destroy.add_frame(ResLoader::instance()->find_texture("enemy1_down"), 4);
		this->anim_destroy.set_interval(0.1f);
		this->anim_destroy.set_loop(false);
		this->anim_destroy.set_on_finished([&]()
			{
				Mix_PlayChannel(-1, ResLoader::instance()->find_audio("enemy1_down"), 0);
				game_scene->score += 10;	// 击中+10分
				this->is_remove = true;
			});
	}
};

class MiddleEnemy : public Enemy
{
public:
	MiddleEnemy()
	{
		this->size = Vector2(96.0f, 95.0f);
		this->max_health = MIDDLE_ENEMY_HEALTH;

		this->health = MIDDLE_ENEMY_HEALTH;
		this->speed = MIDDLE_ENEMY_SPEED;
		this->anim_idle.add_frame(ResLoader::instance()->find_texture("enemy2"), 1);

		this->anim_hit.add_frame(ResLoader::instance()->find_texture("enemy2_hit"), 1);

		this->anim_destroy.add_frame(ResLoader::instance()->find_texture("enemy2_down"), 4);
		this->anim_destroy.set_interval(0.1f);
		this->anim_destroy.set_loop(false);
		this->anim_destroy.set_on_finished([&]()
			{
				Mix_PlayChannel(-1, ResLoader::instance()->find_audio("enemy2_down"), 0);
				game_scene->score += 50;	// 击中+50分
				this->is_remove = true;
			});
	}
};

class BigEnemy : public Enemy
{
public:
	BigEnemy()
	{
		this->size = Vector2(169.0f, 258.0f);
		this->max_health = BIG_ENEMY_HEALTH;

		this->health = BIG_ENEMY_HEALTH;
		this->speed = BIG_ENEMY_SPEED;
		this->anim_idle.add_frame(ResLoader::instance()->find_texture("enemy3_idle"), 2);
		this->anim_idle.set_interval(0.1f);
		this->anim_idle.set_loop(true);

		this->anim_hit.add_frame(ResLoader::instance()->find_texture("enemy3_hit"), 1);
		this->anim_hit.set_loop(false);

		this->anim_destroy.add_frame(ResLoader::instance()->find_texture("enemy3_down"), 6);
		this->anim_destroy.set_interval(0.1f);
		this->anim_destroy.set_loop(false);
		this->anim_destroy.set_on_finished([&]()
			{
				Mix_PlayChannel(-1, ResLoader::instance()->find_audio("enemy3_down"), 0);
				int num = Random().randint(1, 20);
				if (num == 13)	// 5%的概率掉落炸弹
				{
					Mix_PlayChannel(-1, ResLoader::instance()->find_audio("supply"), 0);
					Supply* newSupply = new Supply("bomb", position);
					ObjectManager::instance()->create_node(newSupply);
				}

				game_scene->score += 100;	// 击中+100分
				this->is_remove = true;
			});
	}
};