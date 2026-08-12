module;

#include <SDL3/SDL.h>
#include <string>

module MenuScene;

MenuScene::MenuScene()
{
	std::u8string tips_info_u8 = u8"点击屏幕开始游戏";
	this->tips = new Text(std::string(tips_info_u8.begin(), tips_info_u8.end()));
	this->history_highest = new Text();
}

MenuScene::~MenuScene()
{
	delete this->tips;
	delete this->history_highest;
	Scene::~Scene();
}

void MenuScene::on_enter()
{
	SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER));

	int highest_socre = Data::read<int>("record", "highest_score");
	std::string score = std::to_string(highest_socre);
	std::u8string info = u8"历史最高: " + std::u8string(score.begin(), score.end());
	history_highest->set_string(std::string(info.begin(), info.end()));
}

void MenuScene::on_render(const Camera& cam_game, const Camera& cam_ui)
{
	SDL_FRect rect_bg = { 0, 0, 600, 900 };
	Renderer::render_texture(cam_game, ResourcesManager::instance()->find_texture("background"), nullptr, &rect_bg, 0);

	SDL_FRect rect_title = { 300, 200, 556, 130 };
	Renderer::render_texture(cam_game, ResourcesManager::instance()->find_texture("title"), nullptr, &rect_title, 0, { 0.5f, 0.5f });

	Renderer::render_text(cam_game, { 150, 490 }, tips, 40, colors::Black);
	Renderer::render_text(cam_game, { 10, 10 }, history_highest, 25, colors::Black);
}

void MenuScene::on_input(const SDL_Event& event)
{
	if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
	{
		if (event.button.button == SDL_BUTTON_LEFT)
			SceneManager::instance()->switch_to("game_scene");
	}
}

void MenuScene::on_exit()
{
	SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT));
}