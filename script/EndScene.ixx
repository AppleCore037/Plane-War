module;

#include <SDL3/SDL.h>

export module EndScene;

import FlexCore;
import Buttons;
using namespace fce;

export class EndScene : public Scene
{
public:
	EndScene();
	~EndScene() override;

	void on_enter() override;

	void on_render(const Camera& cam_game, const Camera& cam_ui) override;

	void on_input(const SDL_Event& event) override;

private:
	int final_score = 0;
	Text* die_info = nullptr;
	Text* score_info = nullptr;

	IButton* restart_button = nullptr;
	IButton* exit_button = nullptr;
};