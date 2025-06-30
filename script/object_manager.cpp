#include "object_manager.h"

#include "bullet.h"
#include "enemy_instance.h"
#include "game_scene.h"
#include <algorithm>

extern GameScene* game_scene;
ObjectManager* ObjectManager::m_instace = nullptr;

ObjectManager* ObjectManager::instance()
{
	if (!m_instace)
		m_instace = new ObjectManager();

	return m_instace;
}

void ObjectManager::init_function()
{
	timer_enemy_spawn.set_one_shot(false);
	timer_enemy_spawn.set_on_timeout([&]()
		{
			enemy_num++;
			int n = m_instace->random.randint(1, 10);
			if (n <= 5)
				ObjectManager::instance()->create_node(new SmallEnemy());
			else if (n > 5 && n <= 9 && game_scene->score >= 1000)
				ObjectManager::instance()->create_node(new MiddleEnemy());
			else if (game_scene->score >= 5000 && n == 10)
				ObjectManager::instance()->create_node(new BigEnemy());
		});

	timer_fire_rate.set_one_shot(false);
	timer_fire_rate.set_wait_time(0.2f);
	timer_fire_rate.set_on_timeout([&]()
		{
			Mix_PlayChannel(-1, ResLoader::instance()->find_audio("bullet"), 0);
			Vector2 pos = game_scene->player->get_position();
			pos.y -= 63;
			Bullet* newBullet = new Bullet(pos);
			newBullet->set_layer(Render::Bullet, Collision::Bullet);

			ObjectManager::instance()->create_node(newBullet);
		});

	timer_spawn_supply.set_one_shot(false);
	timer_spawn_supply.set_wait_time(random.randfloat(17.0f, 20.0f));
	timer_spawn_supply.set_on_timeout([&]()
		{
			Supply* newSupply = new Supply("bullet", Vector2(random.randfloat(200.0f, 500.0f), -150.0f));
			ObjectManager::instance()->create_node(newSupply);
			timer_spawn_supply.set_wait_time(random.randfloat(17.0f, 20.0f));
		});

	timer_power_up.set_one_shot(true);
	timer_power_up.set_wait_time(POWERUP_TIME);
	timer_power_up.set_on_timeout([&](){ game_scene->bullet_level = 1; });
}

Node* ObjectManager::create_node(Node* _New)
{
	node_list.push_back(_New);
	return _New;
}

void ObjectManager::destory_node(Node* node)
{
	node_list.erase(std::remove(node_list.begin(), node_list.end(), node), 
		node_list.end());
	delete node;
}

void ObjectManager::on_update(float delta)
{
	// 更新计时器
	timer_enemy_spawn.set_wait_time(random.randfloat(0.4f, 0.6f));
	timer_fire_rate.set_wait_time(game_scene->bullet_level == 1 ? 0.2f : 0.1f);

	if (!game_scene->player->invincible())	timer_fire_rate.on_update(delta);
	if (game_scene->score >= 2500)			timer_spawn_supply.on_update(delta);
	if (enemy_num <= MAX_ENEMY)				timer_enemy_spawn.on_update(delta);
	if(game_scene->bullet_level == 2)		timer_power_up.on_update(delta);

	// 更新敌机计数器
	this->enemy_num = std::count_if(node_list.begin(), node_list.end(), [&](Node* node)-> bool
		{
			return node->get_collision_layer() == Collision::Enemy;
		});

	// 对节点先按层级，再按照Y坐标进行排序
	node_list.sort([&](Node* a, Node* b) -> bool
		{
			if (a->get_render_layer() == b->get_render_layer())
				return a->get_position().y < b->get_position().y;
			else
				return a->get_render_layer() < b->get_render_layer();
		});

	// 遍历节点列表进行更新
	for (Node* node : node_list)
	{
		node->on_update(delta);

		// 判断该节点是否可以移除
		if (node->can_remove())
		{
			node_list.erase(std::remove(node_list.begin(), node_list.end(), node), 
				node_list.end());
			delete node;
		}
	}

	// 处理碰撞
	this->proccess_collide();
}

void ObjectManager::on_render(const Camera& camera)
{
	// 遍历节点列表进行渲染
	for (Node* node : node_list)
		node->on_render(camera);
}

void ObjectManager::on_input(const SDL_Event& event)
{
	for (Node* node : node_list)
		node->on_input(event);
}

void ObjectManager::release()
{
	for (Node* node : node_list)
	{
		node_list.erase(std::remove(node_list.begin(), node_list.end(), node),
			node_list.end());
		delete node;
	}
}

void ObjectManager::proccess_collide()
{
	for (Node* node_src : node_list)
	{
		if (node_src->get_collision_layer() == Collision::None
			|| node_src->get_collision_layer() == Collision::Supply)
			continue;

		for (Node* node_dst : node_list)
		{
			if (node_src == node_dst || node_dst->get_collision_layer() == Collision::None
				|| node_src->get_collision_layer() == node_dst->get_collision_layer())
				continue;

			// 进行碰撞检测
			bool is_collide = false;

			// 如果是子弹和敌机之间的碰撞
			if (node_dst->get_collision_layer() == Collision::Bullet)
				is_collide = (distance_of(node_src, node_dst) < node_src->get_size().y / 2);
			else
			{
				//只有玩家与补给之间碰撞才有效
				if (node_src->get_collision_layer() == Collision::Player)
					is_collide = (distance_of(node_src, node_dst) < node_dst->get_size().y
						&& node_dst->get_collision_layer() == Collision::Supply);
				else
					is_collide = (distance_of(node_src, node_dst) < node_dst->get_size().y / 2
						&& node_dst->get_collision_layer() != Collision::Supply);
			}

			if (is_collide)
			{
				if (node_dst->get_collision_layer() == Collision::Supply)
					timer_power_up.restart();

				if (node_dst->on_collide)
					node_dst->on_collide();
			}
		}
	}
}