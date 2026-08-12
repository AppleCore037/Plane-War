module;

#include <SDL3/SDL.h>
#include <string>

module EndScene;

import FlexCore;
import GameScene;
import Buttons;
using namespace fce;

EndScene::EndScene()
{
	this->score_info = new Text();

	std::u8string die_u8 = u8"你失败了";
	this->die_info = new Text(std::string(die_u8.begin(), die_u8.end()));

	this->restart_button = new RestartButton();
	this->exit_button = new ExitButton();
}

EndScene::~EndScene()
{
	delete restart_button;
	delete exit_button;
	delete die_info;
	delete score_info;
}

void EndScene::on_enter()
{
	auto game_scene = SceneManager::instance()->find_scene("game_scene");
	this->final_score = dynamic_cast<GameScene*>(game_scene)->get_score();

	std::u8string score_u8 = u8"你的最终得分为: " + std::u8string((const char8_t*)std::to_string(final_score).c_str());
	this->score_info->set_string(std::string(score_u8.begin(), score_u8.end()));

	int highest_score = Data::read<int>("record", "highest_score");
	if (final_score > highest_score) Data::save("record", "highest_score", final_score, Data::Override);
}

void EndScene::on_render(const Camera& cam_game, const Camera& cam_ui)
{
	SDL_FRect rect_bg = { 0, 0, 600, 900 };
	Renderer::render_texture(cam_ui, ResourcesManager::instance()->find_texture("background"), nullptr, &rect_bg, 0);

	Renderer::render_text(cam_ui, { 165, 150 }, die_info, 70.0f, colors::Red);
	Renderer::render_text(cam_ui, { 165, 330 }, score_info, 30.0f, colors::Black);

	this->restart_button->on_render(cam_ui);
	this->exit_button->on_render(cam_ui);
}

void EndScene::on_input(const SDL_Event& event)
{
	this->restart_button->on_input(event);
	this->exit_button->on_input(event);
}