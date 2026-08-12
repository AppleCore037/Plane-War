module;

#include <SDL3/SDL.h>

export module GameScene;

import FlexCore;
import Buttons;
using namespace fce;

export class GameScene : public Scene
{
public:
	int big_enemy_cnt = 0;
	int mid_enemy_cnt = 0;

public:
	GameScene();
	~GameScene();

	void on_enter() override;

	void on_update(float delta) override;

	void on_render(const Camera& cam_game, const Camera& cam_ui) override;

	void on_input(const SDL_Event& event) override;

	void on_exit() override;

	void plus_score(int val);

	int get_score() const;

private:
	void show_health(const Camera& camera);

	void show_bomb(const Camera& camera);

private:
	Text* text_bomb = nullptr;
	Text* text_score = nullptr;
	IButton* pause_button = nullptr;

	Timer timer_enemy_spawner;
	Timer timer_supply_spawner;
	int score = 0;
};