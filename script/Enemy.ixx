module;

#include <SDL3/SDL.h>
#include <string>

export module Enemy;

import FlexCore;
using namespace fce;

export class Enemy : public Sprite
{
public:
	Enemy();

	~Enemy() override;

	void on_update(float delta) override;

	void on_render(const Camera& camera) override;

protected:
	int health, max_health;
	int reward;
	float speed;

	int play_cnt = 0;

	std::string audio_name;
	CollisionBox* hit_box = nullptr;
	AnimationPlayer animation_player;
};

export class SmallEnemy : public Enemy
{
public:
	SmallEnemy();
};

export class MiddleEnemy : public Enemy
{
public:
	MiddleEnemy();
};

export class BigEnemy : public Enemy
{
public:
	BigEnemy();
};