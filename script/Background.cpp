module;

#include <SDL3/SDL.h>

module Background;
import FlexCore;
using namespace fce;

Background::Background(float default_y)
{
	Property& prop = this->get_property();
	prop.position = { 0.0f, default_y };
	prop.size = { 600, 900 };
	prop.layer = RenderLayer::Background;
	prop.group_tag = "backgrounds";
	this->default_y = default_y;
}

void Background::on_update(float delta)
{
	Property& prop = this->get_property();
	prop.position += Vector2({ 0.0f, SPEED }) * delta;
	if (prop.position.y >= prop.size.h) prop.position = { 0.0f, -prop.size.h };
}

void Background::on_render(const Camera& camera)
{
	SDL_FRect rect_bg = this->get_property().get_SDLRect();
	Renderer::render_texture(camera, ResourcesManager::instance()->find_texture("background"), nullptr, &rect_bg, 0);
}

void Background::on_ready()
{
	this->get_property().position = { 0.0f, default_y };
}