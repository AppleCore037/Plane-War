#pragma once
#include "node.h"
#include "baseKit.h"

class Bullet : public Node
{
public:
	Bullet(const Vector2& pos);

	~Bullet() = default;

	void on_update(float delta) override;

	void on_render(const Camera& camera) override;

	void on_input(const SDL_Event& event) override {}

private:
	SDL_Texture* texture = nullptr;
};