#pragma once
#include "baseKit.h"
#include "node.h"
#include <SDL.h>

class Enemy : public Node
{
public:
	Enemy();

	~Enemy() = default;

	virtual void on_update(float delta) override;

	virtual void on_render(const Camera& camera) override;

	virtual void on_input(const SDL_Event& event) override {}

protected:
	float speed = 0;
	int health = 0;
	int max_health = 0;

	Animation anim_idle;
	Animation anim_hit;
	Animation anim_destroy;

private:
	Animation* current_animation;
	Random random;
};