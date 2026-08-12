module;

#include <SDL3/SDL.h>
#include <format>

module Bullet;

import FlexCore;
using namespace fce;

Bullet::Bullet()
{
	Property& prop = this->get_property();

	prop.size = { 5.0f, 11.0f };
	prop.anchor = { 0.5f, 0.5f };
	prop.layer = RenderLayer::GameObject;
	prop.group_tag = "bullets";

	this->hit_box = CollisionManager::instance()->create_collision_box();
	this->hit_box->set_size(prop.size);
	this->hit_box->set_enabled(false);
}

Bullet::~Bullet()
{
	CollisionManager::instance()->destroy_collision_box(hit_box);
}

Bullet::Bullet(const Bullet& other)
{
	Property& my_prop = this->get_property();
	const Property& other_prop = other.get_property();

	my_prop.position = other_prop.position;
	my_prop.size = other_prop.size;
	my_prop.anchor = other_prop.anchor;
	my_prop.layer = RenderLayer::GameObject;
	my_prop.group_tag = other_prop.group_tag;

	this->damage = other.damage;
	this->level = other.level;

	this->hit_box = CollisionManager::instance()->create_collision_box();
	this->hit_box->set_size(other.hit_box->get_size());
	this->hit_box->set_enabled(true);
	this->hit_box->set_layer_src(CollisionLayer::Attack);
	this->hit_box->set_layer_dst(CollisionLayer::Enemy);

	this->hit_box->set_send_props([&](CollideCtx& info) { info.pack("damage", damage); });
	this->hit_box->set_on_collide([this](const CollideCtx&) { this->get_property().can_remove = true; });
}

void Bullet::on_update(float delta)
{
	Property& prop = this->get_property();

	if (prop.position.y <= -prop.size.h)
		prop.can_remove = true;

	prop.position += Vector2(0.0f, -SPEED) * delta;
	this->hit_box->set_position(prop.position);
}

void Bullet::on_render(const Camera& camera)
{
	SDL_Texture* texture;
	std::string tex_name = std::format("bullet{}", level);

	texture = ResourcesManager::instance()->find_texture(tex_name);
	SDL_FRect rect_bullet = this->get_property().get_SDLRect();

	Renderer::render_texture(camera, texture, nullptr, &rect_bullet, this->get_property().direction, this->get_property().anchor);
}

void Bullet::on_ready()
{
	this->damage = 10;
	this->level = 1;
}

void Bullet::set_level(int lv, int dmg)
{
	this->level = lv;
	this->damage = dmg;
}

void Bullet::clone()
{
	auto game_scene = SceneManager::instance()->find_scene("game_scene");
	game_scene->add_sprite(std::make_shared<Bullet>(*this));
}