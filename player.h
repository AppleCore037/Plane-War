#pragma once
#include "settings.h"
#include "node.h"

class Player : public Node
{
public:
	Player();

	~Player() = default;

	void on_update(float delta) override;

	void on_input(const SDL_Event& event) override;

	void on_render(const Camera& camera) override;

	const int get_health() const
	{
		return this->health;
	}

	bool invincible() const
	{
		return this->is_invincible;
	}

private:
	void check_edge();

	void reset_property();

private:
	Animation anim_idle;
	Animation anim_explosion;
	Animation* current_animation = nullptr;
	Timer timer_invincible;

	bool is_leftKey_down = false;
	bool is_rightKey_down = false;
	bool is_upKey_down = false;
	bool is_downKey_down = false;
	bool is_invincible = false;

	int health = PLAYER_HEALTH;
};