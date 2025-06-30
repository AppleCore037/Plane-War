#pragma once
#include "node.h"
#include "baseKit.h"
#include "util.h"

#include <SDL.h>
#include <list>

#define MAX_ENEMY 15

class ObjectManager
{
public:
	static ObjectManager* instance();

	void init_function();

	Node* create_node(Node* _New);

	void destory_node(Node* node);

	void on_update(float delta);

	void on_render(const Camera& camera);

	void on_input(const SDL_Event& event);

	void release();

	int& get_enemy_number()
	{
		return this->enemy_num;
	}

private:
	ObjectManager() = default;
	~ObjectManager() = default;

	void proccess_collide();

	float distance_of(Node* node_1, Node* node_2)
	{
		float dx = node_1->get_position().x - node_2->get_position().x;
		float dy = node_1->get_position().y - node_2->get_position().y;
		return sqrt(dx * dx + dy * dy);
	}

private:
	static ObjectManager* m_instace;
	std::list<Node*> node_list;		// �ڵ��б�

	Timer timer_enemy_spawn;		// �������ɼ�ʱ��
	Timer timer_fire_rate;			// �ӵ������ʱ��	
	Timer timer_spawn_supply;		// �������ɼ�ʱ��
	Timer timer_power_up;			// ǿ����ʱ��

	Random random;					// �����
	int enemy_num = 0;
};