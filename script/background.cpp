#include "background.h"

#include "settings.h"
#include "resources_manager.h"

Background::Background(const Vector2& pos)
{
	this->texture = ResLoader::instance()->find_texture("background");
	this->position = pos;
	this->set_layer(Render::Background, Collision::None);
}

void Background::on_update(float delta)
{
	this->velocity.y = BACKGROUND_SPEED;
	this->position += velocity * delta;

	if (is_touching_edge())
		this->position.y = -700;
}

void Background::on_render(const Camera& camera)
{
	SDL_FRect rect_dst = { position.x, position.y, 490.0f, 710.0f };
	camera.render_texture(texture, nullptr, &rect_dst, 0, nullptr);
}

bool Background::is_touching_edge()
{
	return position.y >= 700;
}