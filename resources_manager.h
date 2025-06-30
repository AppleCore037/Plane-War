#ifndef _RESOURCES_MANAGER_H_
#define _RESOURCES_MANAGER_H_

#include <SDL.h>
#include <SDL_mixer.h>

#include <string>
#include <unordered_map>

// ��Դ������
class ResourcesManager
{
public:
	// ʵ������Դ����������
	static ResourcesManager* instance();

	// ������Դ
	void load(SDL_Renderer* renderer);

	// Ѱ��������Դ
	SDL_Texture* find_texture(const std::string& key);

	// Ѱ����Ƶ��Դ
	Mix_Chunk* find_audio(const std::string& key);
	
private:
	ResourcesManager() = default;
	~ResourcesManager() = default;

private:
	static ResourcesManager* m_instance;							// ��Դ������ʵ��
	std::unordered_map<std::string, Mix_Chunk*> audio_pool;			// ��Ƶ��Դ��
	std::unordered_map<std::string, SDL_Texture*> texture_pool;		// ������Դ��
};

typedef ResourcesManager ResLoader;

#endif // !_RESOURCES_MANAGER_H_