#ifndef _RESOURCES_MANAGER_H_
#define _RESOURCES_MANAGER_H_

#include <SDL.h>
#include <SDL_mixer.h>

#include <string>
#include <unordered_map>

// 资源管理器
class ResourcesManager
{
public:
	// 实例化资源管理器对象
	static ResourcesManager* instance();

	// 加载资源
	void load(SDL_Renderer* renderer);

	// 寻找纹理资源
	SDL_Texture* find_texture(const std::string& key);

	// 寻找音频资源
	Mix_Chunk* find_audio(const std::string& key);
	
private:
	ResourcesManager() = default;
	~ResourcesManager() = default;

private:
	static ResourcesManager* m_instance;							// 资源管理器实例
	std::unordered_map<std::string, Mix_Chunk*> audio_pool;			// 音频资源池
	std::unordered_map<std::string, SDL_Texture*> texture_pool;		// 纹理资源池
};

typedef ResourcesManager ResLoader;

#endif // !_RESOURCES_MANAGER_H_