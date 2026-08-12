module;

#include <SDL3/SDL.h>
#include <string>

module Supply;

Supply::Supply()
{
	auto& p = this->get_property();

	this->type = Random::randint(1, 2);
	p.position = { Random::randfloat(50.0f, 550.0f), -107.0f };
	p.anchor = { 0.5f, 0.5f };
	p.layer = RenderLayer::GameObject;
	p.group_tag = "supplies";
	
	this->hit_box = CollisionManager::instance()->create_collision_box();
	this->hit_box->set_layer_src(CollisionLayer::GameObject);
	this->hit_box->set_layer_dst(CollisionLayer::Player);
	this->hit_box->set_on_collide([this](const CollideCtx&) { this->get_property().can_remove = true; });
	this->hit_box->set_send_props([&](CollideCtx& info) { info.pack("type", type); });

	if (type == 1)
	{
		p.size = { 58.0f, 88.0f };
		this->texture = ResourcesManager::instance()->find_texture("bullet_supply");
	}
	else
	{
		p.size = { 60.0f, 107.0f };
		this->texture = ResourcesManager::instance()->find_texture("bomb_supply");
	}
}

Supply::~Supply()
{
	CollisionManager::instance()->destroy_collision_box(hit_box);
	SDL_DestroyTexture(texture);
}

void Supply::on_update(float delta)
{
	auto& p = this->get_property();

	if (p.position.y >= 900.0f + p.size.h)
		p.can_remove = true;

	p.position += Vector2(0.0f, 150.0f) * delta;
	this->hit_box->set_position(p.position);
}

void Supply::on_render(const Camera& camera)
{
	SDL_FRect rect = this->get_property().get_SDLRect();
	Renderer::render_texture(camera, texture, nullptr, &rect, 0, this->get_property().anchor);
}