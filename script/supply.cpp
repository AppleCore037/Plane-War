#include "supply.h"

#include "settings.h"
#include "resources_manager.h"
#include "game_scene.h"

extern GameScene* game_scene;

Supply::Supply(const std::string& type, const Vector2& position)
{
	this->type = type;
	if (type == "bomb")
	{
		this->texture = ResLoader::instance()->find_texture("bomb_supply");
		this->size = Vector2(60, 107);
	}
	else if(type == "bullet")
	{
		this->texture = ResLoader::instance()->find_texture("bullet_supply");
		this->size = Vector2(58, 88);
	}

	this->set_layer(Render::Node, Collision::Supply);
	this->position = Vector2(position);

	this->on_collide = [&]()
		{
			if (this->type == "bomb")
			{
				Mix_PlayChannel(-1, ResLoader::instance()->find_audio("get_bomb"), 0);
				game_scene->bomb_count++;
				game_scene->score += 100;
			}
			else if (this->type == "bullet")
			{
				Mix_PlayChannel(-1, ResLoader::instance()->find_audio("get_bullet"), 0);
				game_scene->bullet_level = game_scene->bullet_level % 2 + 1;
				game_scene->score += 100;
			}

			is_remove = true;
		};
}

Supply::~Supply()
{
	SDL_DestroyTexture(texture);
}

void Supply::on_update(float delta)
{
	this->velocity = Vector2(0, 50);
	this->position += this->velocity * delta;

	if (this->position.y > SCREEN_HEIGHT + size.y)
		this->is_remove = true;
}

void Supply::on_render(const Camera& camera)
{
	if (texture)
	{
		SDL_FRect rect_dst = { position.x, position.y, size.x, size.y };
		camera.render_texture(texture, nullptr, &rect_dst, 0, nullptr);
	}
}