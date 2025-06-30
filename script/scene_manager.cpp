#include "scene_manager.h"

extern SDL_Window* window;
SceneManager* SceneManager::m_instance = nullptr;

SceneManager* SceneManager::instance()
{
	if (m_instance == nullptr)
		m_instance = new SceneManager();

	return m_instance;
}

void SceneManager::set_current_scene(SceneType type)
{
	if (scene_list.find(type) == scene_list.end())
		return;

	current_scene = scene_list[type];
	current_scene->on_enter();
}

void SceneManager::switch_to(SceneType type)
{
	current_scene->on_exit();
	if (scene_list.find(type) == scene_list.end())
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Scene not found!", window);
		return;
	}

	current_scene = scene_list[type];
	current_scene->on_enter();
}

Scene* SceneManager::create_scene(SceneType type, Scene* _New)
{
	scene_list.insert(std::make_pair(type, _New));
	return _New;
}

Scene* SceneManager::get_current_scene() const
{
	return current_scene;
}

Scene* SceneManager::find_scene(SceneType type) const
{
	if (scene_list.find(type) == scene_list.end())
		return nullptr;

	return scene_list.at(type);
}