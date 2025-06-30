#include "player.h"

#include "settings.h"
#include "resources_manager.h"

Player::Player()
{
	this->size = Vector2(102.0f, 126.0f);

	this->timer_invincible.set_one_shot(true);
	this->timer_invincible.set_wait_time(3.5f);
	this->timer_invincible.set_on_timeout([&]()
		{
			is_invincible = false;
			this->set_layer(Render::Node, Collision::Player);
			SDL_SetTextureAlphaMod(ResLoader::instance()->find_texture("player_idle"), 255);
		});

	anim_idle.add_frame(ResLoader::instance()->find_texture("player_idle"), 2);
	anim_idle.set_interval(0.1f);
	anim_idle.set_loop(true);

	anim_explosion.add_frame(ResLoader::instance()->find_texture("player_down"), 4);
	anim_explosion.set_interval(0.08f);
	anim_explosion.set_loop(false);
	anim_explosion.set_on_finished([&]()
		{
			health -= 1;
			anim_explosion.reset();
			current_animation = &anim_idle;
			reset_property();
		});

	this->on_collide = [&]() 
		{ 
			Mix_PlayChannel(-1, ResLoader::instance()->find_audio("me_down"), 0);
			current_animation = &anim_explosion; 
		};
	this->current_animation = &anim_idle;
}

void Player::on_update(float delta)
{
	this->check_edge();
	this->velocity.x = (is_rightKey_down - is_leftKey_down) * PLAYER_SPEED;
	this->velocity.y = (is_downKey_down - is_upKey_down) * PLAYER_SPEED;
	this->position += velocity * delta;

	if (is_invincible)
		this->timer_invincible.on_update(delta);

	if (current_animation != nullptr)
	{
		this->current_animation->set_position(position);
		this->current_animation->on_update(delta);
	}
}

void Player::on_input(const SDL_Event& event)
{
	if (event.type == SDL_KEYDOWN)
	{
		if (event.key.keysym.sym == SDLK_UP)
			this->is_upKey_down = true;
		else if (event.key.keysym.sym == SDLK_DOWN)
			this->is_downKey_down = true;
		else if (event.key.keysym.sym == SDLK_LEFT)
			this->is_leftKey_down = true;
		else if (event.key.keysym.sym == SDLK_RIGHT)
			this->is_rightKey_down = true;
	}
	else if (event.type == SDL_KEYUP)
	{
		if (event.key.keysym.sym == SDLK_UP)
			this->is_upKey_down = false;
		else if (event.key.keysym.sym == SDLK_DOWN)
			this->is_downKey_down = false;
		else if (event.key.keysym.sym == SDLK_LEFT)
			this->is_leftKey_down = false;
		else if (event.key.keysym.sym == SDLK_RIGHT)
			this->is_rightKey_down = false;
	}
}

void Player::on_render(const Camera& camera)
{
	if(current_animation != nullptr)
		this->current_animation->on_render(camera);
}

void Player::check_edge()
{
	if (position.x < 0)
		position.x = 0;

	else if (position.x > 480)
		position.x = 480;

	else if (position.y < 0)
		position.y = 0;

	else if (position.y > 700)
		position.y = 700;
}

void Player::reset_property()
{
	this->is_invincible = true;
	this->position = Vector2(SCREEN_WIDTH / 2, 600);
	this->set_layer(Render::Node, Collision::None);

	SDL_SetTextureAlphaMod(ResLoader::instance()->find_texture("player_idle"), 128);
	this->timer_invincible.restart();
}