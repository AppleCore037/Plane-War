#include "resources_manager.h"

#include <SDL_image.h>

#include <filesystem>
#include <string>

extern SDL_Window* window;
ResourcesManager* ResourcesManager::m_instance = nullptr;

// 实例化资源管理器对象
ResourcesManager* ResourcesManager::instance()
{
	if (m_instance == nullptr)
		m_instance = new ResourcesManager();

	return m_instance;
}

// 加载资源
void ResourcesManager::load(SDL_Renderer* renderer)
{
	using namespace std::filesystem;

	for (const auto& entry : directory_iterator("resource"))
	{
		if (entry.is_regular_file())
		{
			const path& path = entry.path();
			if (path.extension() == ".png")
			{
				SDL_Texture* texture = IMG_LoadTexture(renderer, path.u8string().c_str());
				texture_pool[path.stem().u8string()] = texture;
			}
			if (path.extension() == ".wav")
			{
				Mix_Chunk* audio = Mix_LoadWAV(path.u8string().c_str());
				audio_pool[path.stem().u8string()] = audio;
			}
		}
	}
}

// 寻找纹理资源
SDL_Texture* ResourcesManager::find_texture(const std::string& key)
{
	if (texture_pool.find(key) == texture_pool.end())
	{
		std::string message = u8"texture “" + key + u8"” is not found!";
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, u8"ResourcesManager Error", message.c_str(), window);
		exit(-1);
	}

	return texture_pool[key];
}

// 寻找音频资源
Mix_Chunk* ResourcesManager::find_audio(const std::string& key)
{
	if (audio_pool.find(key) == audio_pool.end())
	{
		std::string message = u8"audio “" + key + u8"” is not found!";
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, u8"ResourcesManager Error", message.c_str(), window);
		exit(-1);
	}

	return audio_pool[key];
}