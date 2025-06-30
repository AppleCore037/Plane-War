#include "enemy.h"

#include "util.h"
#include "settings.h"


#include "object_manager.h"
#include "game_scene.h"
#include <SDL_mixer.h>

extern SDL_Renderer* renderer;
extern GameScene* game_scene;
extern bool is_bombed;

Enemy::Enemy()
{
	this->position.x = random.randfloat(50.0f, 450.0f);
	this->position.y = -200.0f;
	this->current_animation = nullptr;
	this->set_layer(Render::Node, Collision::Enemy);

	this->on_collide = [&]()
		{
			game_scene->score += 5;		// 击中+5分
			if(game_scene->bullet_level == 1)
				health -= BULLET_DAMAGE_NORMAL;
			else
				health -= BULLET_DAMAGE_POWERUP;
		};
}

void Enemy::on_update(float delta)
{
	if (position.y > 830)
		is_remove = true;
	if (is_bombed)
		health = 0;

	if (health <= 0)
	{
		this->set_layer(Render::Node, Collision::None);	// 取消碰撞
		current_animation = &anim_destroy;
	}
	else
		current_animation = &anim_idle;

	this->velocity = Vector2(0, speed);
	this->position += velocity * delta;

	if (current_animation != nullptr)
	{
		this->current_animation->set_position(position);
		this->current_animation->on_update(delta);
	}
}

void Enemy::on_render(const Camera& camera)
{
	// 绘制血条
	SDL_Rect health_bar_bg = { (int)position.x - (size.x / 2), (int)position.y + (size.y / 2 + 5), (int)size.x, 5 };
	SDL_Rect health_bar_fg = { (int)position.x - (size.x / 2), (int)position.y + (size.y / 2 + 5), 0, 5 };

	if(health >= 0)
		health_bar_fg.w = ((int)size.x * health / max_health);

	camera.render_rect(&health_bar_bg, { 255, 0, 0, 255 });
	camera.render_rect(&health_bar_fg, { 0, 255, 0, 255 });

	if (current_animation != nullptr)
		this->current_animation->on_render(camera);
}