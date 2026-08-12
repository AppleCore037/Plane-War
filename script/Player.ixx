module;

export module Player;

import FlexCore;
import Bullet;
using namespace fce;

export class Player : public FlatBody
{
public:
	Player();
	~Player() override;

	void on_update(float delta) override;

	void on_render(const Camera& camera) override;

	void on_input(const SDL_Event& event) override;

	void on_ready() override;

	int get_health() const;

	int get_bomb() const;

	bool is_used_bomb() const;

private:
	void load_animation();

	void set_bound();

private:
	AnimationPlayer anim_player;
	CollisionBox* hit_box = nullptr;

	Timer timer_shoot;
	Bullet* bullet_prototype = nullptr;

	Timer timer_invalid;
	Timer timer_power_up;
	const Vector2& DEFAULT_POS = { 300, 650 };

	int health = 3;
	int bomb_cnt = 3;
	int level = 1;

	bool can_shoot = true;
	bool is_invalid = false;
	bool used_bomb = false;
};