module;

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

#include <cmath>
#include <algorithm>
#include <string>

module Player;

Player::Player()
{
	this->enabled_wasd(false);
	this->set_speed(350.0f);

	Property& prop = this->get_property();
	prop.size = { 102, 126 };
	prop.anchor = { 0.5f, 0.5f };
	prop.tag = "Player";
	prop.layer = RenderLayer::GameObject;

	// 设置碰撞箱属性
	this->hit_box = CollisionManager::instance()->create_collision_box();
	this->hit_box->set_size(prop.size);
	this->hit_box->set_layer_src(CollisionLayer::Player);
	this->hit_box->set_layer_dst(CollisionLayer::GameObject);
	this->hit_box->set_on_collide([&](const CollideCtx& info)
		{
			if (info.other_layer & CollisionLayer::Enemy)
			{
				Mix_PlayChannel(-1, ResourcesManager::instance()->find_audio("me_down"), 0);
				can_shoot = false;
				hit_box->set_enabled(false);
				anim_player.set_animation("die");
			}
			else if (info.other_layer & CollisionLayer::GameObject)
			{
				switch (info.get<int>("type"))
				{
				case 1: 
					Mix_PlayChannel(-1, ResourcesManager::instance()->find_audio("get_bullet"), 0);
					level = 2;
					timer_power_up.restart();
					break;
				case 2:
					Mix_PlayChannel(-1, ResourcesManager::instance()->find_audio("get_bomb"), 0);
					bomb_cnt++;
					break;
				}
			}
		});

	// 加载动画
	this->load_animation();
	this->anim_player.set_animation("idle");

	// 设置子弹升级计时器
	this->timer_power_up.set_one_shot(true);
	this->timer_power_up.set_wait_time(10.0f);
	this->timer_power_up.set_on_timeout([&]() { level = 1; });

	// 设置无敌时间计时器
	this->timer_invalid.set_one_shot(true);
	this->timer_invalid.set_wait_time(5.0f);
	this->timer_invalid.set_on_timeout([this]()
		{
			this->can_shoot = true, this->is_invalid = false;
			this->timer_invalid.restart();
			this->hit_box->set_enabled(true);
			this->get_property().alpha = 100.0f;
		});

	// 初始化子弹原型
	this->bullet_prototype = new Bullet();
	this->timer_shoot.set_on_timeout([&]() 
		{
			Mix_PlayChannel(-1, ResourcesManager::instance()->find_audio("bullet"), 0);
			bullet_prototype->clone();
		});
}

Player::~Player()
{
	delete bullet_prototype;
	CollisionManager::instance()->destroy_collision_box(hit_box);
}

void Player::on_update(float delta)
{
	auto& prop = this->get_property();

	if (health == 0) 
		SceneManager::instance()->switch_to("end_scene");

	FlatBody::on_update(delta);

	this->set_bound();
	this->hit_box->set_position(prop.position);

	// 计时器更新
	if (level == 2) timer_power_up.on_update(delta);
	if (is_invalid) timer_invalid.on_update(delta);

	// 使用炸弹逻辑
	if (used_bomb)
	{
		Mix_PlayChannel(-1, ResourcesManager::instance()->find_audio("use_bomb"), 0);
		this->bomb_cnt--;
		this->used_bomb = false;
		Game::Game_Camera->shake(15.0f, 1.1f);
	}

	// 射击子弹逻辑
	if (can_shoot) timer_shoot.on_update(delta);
	this->timer_shoot.set_wait_time(level == 1 ? 0.2f : 0.1f);
	this->bullet_prototype->set_level(level, (level == 1 ? 10 : 20));

	auto& bullet_prop = bullet_prototype->get_property();
	bullet_prop.position = { prop.position.x, prop.position.y - prop.size.h / 2.0f };

	// 更新动画逻辑
	this->anim_player.on_update(delta);
}

void Player::on_render(const Camera& camera)
{
	this->anim_player.on_render(camera, this->get_property());
}

void Player::on_input(const SDL_Event& event)
{
	FlatBody::on_input(event);
	if (event.type == SDL_EVENT_KEY_DOWN)
	{
		if (event.key.key == SDLK_SPACE)
		{
			if (bomb_cnt > 0 && !used_bomb)
				this->used_bomb = true;
		}
	}
}

void Player::on_ready()
{
	FlatBody::on_ready();
	auto& prop = this->get_property();

	this->health = 3;
	this->level = 1;
	this->bomb_cnt = 3;
	prop.position = DEFAULT_POS;

	this->is_invalid = false, this->can_shoot = true;
	hit_box->set_enabled(true);
	prop.alpha = 100.0f;
}

int Player::get_health() const
{
	return this->health;
}

int Player::get_bomb() const
{
	return this->bomb_cnt;
}

bool Player::is_used_bomb() const
{
	return used_bomb;
}

void Player::load_animation()
{
	auto anim_idle = anim_player.create_animation("idle", ResourcesManager::instance()->find_texture("me"), 2, 1, 1);
	anim_idle->set_loop(true);

	auto anim_die = anim_player.create_animation("die", new Atlas("assets/gfx/me_destroy_%d.png", 1, 4));
	anim_die->set_loop(false);
	anim_die->set_on_finished([this]() 
		{ 
			this->health -= 1;
			this->get_property().alpha = 60.0f;
			this->anim_player.switch_to("idle");

			this->is_invalid = true;
			this->timer_invalid.restart();

			FlatBody::on_ready();
			this->get_property().position = DEFAULT_POS;
		});
}

void Player::set_bound()
{
	Vector2 pos = { 
		std::clamp(this->get_property().position.x, 0.0f, 600.0f),
		std::clamp(this->get_property().position.y, 0.0f, 900.0f)
	};
	this->get_property().position = pos;
}