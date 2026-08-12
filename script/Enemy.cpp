module;

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <string>

module Enemy;

import GameScene;
import Player;

Enemy::Enemy()
{
	auto& p = this->get_property();

	p.anchor = { 0.5f, 0.5f };
	p.position = { Random::randfloat(50.0f, 550.0f), -260.0f };
	p.layer = RenderLayer::GameObject;
	p.group_tag = "enemies";

	this->hit_box = CollisionManager::instance()->create_collision_box();
	this->hit_box->set_layer_src(CollisionLayer::Enemy);
	this->hit_box->set_layer_dst(CollisionLayer::Attack | CollisionLayer::Player);
	this->hit_box->set_on_collide([&](const CollideCtx& info)
		{
			if (info.other_layer & CollisionLayer::Attack)
			{
				health -= info.get<int>("damage");
				animation_player.switch_to("hit");
			}
		});
}

Enemy::~Enemy()
{
	CollisionManager::instance()->destroy_collision_box(hit_box);
	animation_player.clear_animation();
}

void Enemy::on_update(float delta)
{
	auto& p = this->get_property();

	if (p.position.y >= 900 + p.size.h / 2.0f)
		p.can_remove = true;

	if (health <= 0)
	{
		health = 0;	 // 重设为0，防止绘制血条时出现负值
		this->hit_box->set_enabled(false);
		animation_player.switch_to("die");
	}

	// 一个播放音频的技巧，防止音频被重复播放
	if (!play_cnt && health <= 0)
	{
		play_cnt++;
		Mix_PlayChannel(-1, ResourcesManager::instance()->find_audio(audio_name), 0);
	}

	// 对炸弹进行响应
	auto player = SceneManager::instance()->find_scene("game_scene")->find_sprite<Player>("Player");
	if (player->is_used_bomb())
	{
		health = 0;
		this->hit_box->set_enabled(false);
		animation_player.switch_to("die");
	}

	p.position += Vector2(0.0f, speed) * delta;

	hit_box->set_position(p.position);
	animation_player.on_update(delta);
}

void Enemy::on_render(const Camera& camera)
{
	auto& p = this->get_property();
	animation_player.on_render(camera, p);

	SDL_FRect health_bg = { p.position.x, p.position.y + p.size.h / 2.0f + 8.0f, p.size.w + 10.0f, 6.0f};
	SDL_FRect health_fg = { health_bg.x - health_bg.w / 2.0f, health_bg.y - health_bg.h / 2.0f, health_bg.w * (health * 1.0f / max_health), health_bg.h };
	Renderer::render_rect_center(camera, { health_bg.x, health_bg.y }, { health_bg.w, health_bg.h }, colors::Red, true);
	Renderer::render_rect_corner(camera, { health_fg.x, health_fg.y }, { health_fg.w, health_fg.h }, colors::Green, true);
}

// ================================================================================================

SmallEnemy::SmallEnemy()
{
	this->audio_name = "enemy1_down";
	this->max_health = health = 20;
	this->reward = 30;
	this->speed = 400.0f;

	auto& p = this->get_property();
	p.size = { 57, 43 };
	p.group_tag = "enemies";
	this->hit_box->set_size(p.size);

	// 设置动画
	{
		// 待机状态
		auto anim_idle = animation_player.create_animation("idle", ResourcesManager::instance()->find_texture("enemy1"), 1, 1, 1);
		anim_idle->set_loop(true);

		// 受击状态
		auto anim_hit = animation_player.create_animation("hit", ResourcesManager::instance()->find_texture("enemy1"), 1, 1, 1);
		anim_hit->set_loop(false);
		anim_hit->set_on_finished([&]() { animation_player.switch_to("idle"); });

		// 死亡状态
		auto anim_die = animation_player.create_animation("die", new Atlas("assets/gfx/enemy1_down%d.png", 1, 4));
		anim_die->set_loop(false);
		anim_die->set_on_finished([this]()
			{
				auto game_scene = SceneManager::instance()->find_scene("game_scene");
				dynamic_cast<GameScene*>(game_scene)->plus_score(this->reward);
				this->get_property().can_remove = true;
			});

		animation_player.set_animation("idle");
	}
}

MiddleEnemy::MiddleEnemy()
{
	this->audio_name = "enemy2_down";
	this->max_health = health = 50;
	this->reward = 100;
	this->speed = 230.0f;

	auto& p = this->get_property();
	p.size = { 69, 99 };
	p.group_tag = "enemies";
	this->hit_box->set_size(p.size);
	
	// 设置动画
	{
		// 待机动画
		auto anim_idle = animation_player.create_animation("idle", ResourcesManager::instance()->find_texture("enemy2"), 1, 1, 1);
		anim_idle->set_loop(true);

		// 受击状态
		auto anim_hit = animation_player.create_animation("hit", ResourcesManager::instance()->find_texture("enemy2_hit"), 1, 1, 1);
		anim_hit->set_loop(false);
		anim_hit->set_on_finished([&]() { animation_player.switch_to("idle"); });

		// 死亡动画
		auto anim_die = animation_player.create_animation("die", new Atlas("assets/gfx/enemy2_down%d.png", 1, 4));
		anim_die->set_loop(false);
		anim_die->set_on_finished([this]()
			{
				auto game_scene = dynamic_cast<GameScene*>(SceneManager::instance()->find_scene("game_scene"));

				game_scene->plus_score(this->reward);
				game_scene->mid_enemy_cnt--;

				this->get_property().can_remove = true;
			});

		animation_player.set_animation("idle");
	}
}

BigEnemy::BigEnemy()
{
	this->audio_name = "enemy3_down";
	this->max_health = health = 180;
	this->reward = 500;
	this->speed = 100.0f;
	
	auto& p = this->get_property();
	p.size = { 169, 258 };
	p.group_tag = "enemies";
	this->hit_box->set_size(p.size);

	// 设置动画
	{
		// 待机动画
		auto anim_idle = animation_player.create_animation("idle", new Atlas("assets/gfx/enemy3_n%d.png", 1, 2));
		anim_idle->set_loop(true);

		// 受击状态
		auto anim_hit = animation_player.create_animation("hit", ResourcesManager::instance()->find_texture("enemy3_hit"), 1, 1, 1);
		anim_hit->set_loop(false);
		anim_hit->set_on_finished([&]() { animation_player.switch_to("idle"); });

		// 死亡动画
		auto anim_die = animation_player.create_animation("die", new Atlas("assets/gfx/enemy3_down%d.png", 1, 6));
		anim_die->set_loop(false);
		anim_die->set_on_finished([this]()
			{
				auto game_scene = dynamic_cast<GameScene*>(SceneManager::instance()->find_scene("game_scene"));

				game_scene->plus_score(this->reward);
				game_scene->big_enemy_cnt--;

				this->get_property().can_remove = true;
			});

		animation_player.set_animation("idle");
	}
}