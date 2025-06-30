#pragma once
#include "node.h"
#include "baseKit.h"
#include "util.h"
#include <SDL.h>

class Supply : public Node
{
public:
	Supply() = default;

	Supply(
		const std::string& type,	// "bomb" or "bullet"
		const Vector2& position		// initial position
	);

	~Supply();

	virtual void on_update(float delta) override;

	virtual void on_render(const Camera& camera) override;

	virtual void on_input(const SDL_Event& event) override {}

private:
	Random random;
	SDL_Texture* texture = nullptr;
	std::string type;
};