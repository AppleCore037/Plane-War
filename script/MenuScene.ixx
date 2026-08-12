module;

#include <SDL3/SDL.h>

export module MenuScene;

import FlexCore;
using namespace fce;

export class MenuScene : public Scene
{
public:
	MenuScene();
	~MenuScene() override;

	void on_enter() override;

	void on_render(const Camera& cam_game, const Camera& cam_ui) override;

	void on_input(const SDL_Event& event) override;

	void on_exit() override;

private:
	Text* tips = nullptr;
	Text* history_highest = nullptr;
};