#pragma once
#include "node.h"

class Background : public Node
{
public:
	Background(const Vector2& pos);

	~Background() = default;

	void on_update(float delta) override;

	void on_render(const Camera& camera) override;

	void on_input(const SDL_Event& event) override {};

private:
	bool is_touching_edge();

private:
	SDL_Texture* texture = nullptr;
};