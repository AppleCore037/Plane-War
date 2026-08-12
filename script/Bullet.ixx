module;

#include <SDL3/SDL.h>

export module Bullet;

import FlexCore;
using namespace fce;

export class Bullet : public Sprite
{
public:
	Bullet();
	~Bullet() override;

	Bullet(const Bullet& other);

	void on_update(float delta) override;

	void on_render(const Camera& camera) override;

	void on_ready() override;

	void set_level(int lv, int dmg);

	void clone();

private:
	int damage = 10;
	int level = 1;
	const float SPEED = 700.0f;
	CollisionBox* hit_box = nullptr;
};