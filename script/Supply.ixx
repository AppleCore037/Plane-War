module;

#include <SDL3/SDL.h>

export module Supply;

import FlexCore;
using namespace fce;

export class Supply : public Sprite
{
public:
	Supply();
	~Supply() override;

	void on_update(float delta) override;

	void on_render(const Camera& camera) override;

private:
	int type;
	SDL_Texture* texture = nullptr;
	CollisionBox* hit_box = nullptr;
};