#include "bullet.h"

#include "settings.h"
#include "resources_manager.h"
#include "game_scene.h"
#include <SDL.h>

extern GameScene* game_scene;

Bullet::Bullet(const Vector2& pos)
{
	this->position = pos;
	this->size = Vector2(5.0f, 11.0f);
	this->texture = ResLoader::instance()->find_texture(game_scene->bullet_level == 1 ? "bullet1" : "bullet2");
	this->on_collide = [&]()
		{
			game_scene->score += 1;		// »÷ÖÐ+1·Ö
			is_remove = true;
		};
}

void Bullet::on_update(float delta)
{
	if (position.y <= 0)
		this->is_remove = true;

	this->velocity = Vector2(0, -BULLET_SPEED);
	this->position += velocity * delta;
}

void Bullet::on_render(const Camera& camera)
{
	SDL_FRect rect_dst = { position.x, position.y, 5.0f, 11.0f };
	camera.render_texture(texture, nullptr, &rect_dst, 0, nullptr);
}