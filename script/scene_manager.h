#pragma once
#include "scene.h"
#include <unordered_map>

class SceneManager
{
public:
	enum class SceneType
	{
		Menu,
		Game,
	};

public:
	static SceneManager* instance();

	void set_current_scene(SceneType type);

	void switch_to(SceneType type);

	Scene* create_scene(SceneType type ,Scene* _New);

	Scene* get_current_scene() const;

	Scene* find_scene(SceneType type) const;

private:
	SceneManager() = default;
	~SceneManager() = default;

private:
	static SceneManager* m_instance;
	std::unordered_map<SceneType, Scene*> scene_list;
	Scene* current_scene = nullptr;
};