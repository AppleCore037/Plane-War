module;

#include <SDL3/SDL.h>

export module Background;
import FlexCore;
using namespace fce;

export class Background : public Sprite
{
public:
	Background() = default;
	~Background() override = default;

	Background(float default_y);

	void on_update(float delta) override;

	void on_render(const Camera& camera) override;

	void on_ready() override;

private:
	float default_y = 0.0f;
	const float SPEED = 250.0f;
};