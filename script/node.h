#pragma once
#include "util.h"
#include "baseKit.h"
#include <SDL.h>
using namespace Layer;

class Node
{
public:
	std::function<void()> on_collide;

public:
	Node() = default;
	~Node() = default;

	virtual void on_update(float delta) = 0;

	virtual void on_input(const SDL_Event& event) = 0;

	virtual void on_render(const Camera& camera) = 0;
	
	const Vector2& get_position() const
	{
		return this->position;
	}

	void set_position(const Vector2& pos)
	{
		this->position = pos;
	}

	bool can_remove() const
	{
		return this->is_remove;
	}

	void set_layer(Render render,  Collision collision)
	{
		this->render_layer = render;
		this->collision_layer = collision;
	}

	Render get_render_layer() const
	{
		return this->render_layer;
	}

	Collision get_collision_layer() const
	{
		return this->collision_layer;
	}

	const Vector2& get_size() const
	{
		return this->size;
	}

protected:
	Render render_layer = Render::None;
	Collision collision_layer = Collision::None;

	bool is_remove = false;

	Vector2 position;
	Vector2 velocity;
	Vector2 size;
};