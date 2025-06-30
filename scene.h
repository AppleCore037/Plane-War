#pragma once
#include "baseKit.h"
#include <SDL.h>

class Scene
{
public:
	Scene() = default;
	~Scene() = default;

	virtual void on_enter() {}
	virtual void on_update(float delta) {}
	virtual void on_render(const Camera& camera) {}
	virtual void on_input(const SDL_Event& event) {}
	virtual void on_exit() {}
};