module;

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <iostream>

module Buttons;

import FlexCore;
using namespace fce;

ExitButton::ExitButton()
{
	this->position = { 300.0f, 600.0f };
	this->size = { 300.0f, 41.0f };
}

void ExitButton::on_normal()
{
	this->size = { 300.0f, 41.0f };
}

void ExitButton::on_hover()
{
	this->size = { 330.0f, 45.1f };
}

void ExitButton::on_click()
{
	Mix_PlayChannel(-1, ResourcesManager::instance()->find_audio("button"), 0);
	Game::Release_Graphic();
	exit(EXIT_SUCCESS);
}

void ExitButton::on_render(const Camera& camera)
{
	SDL_FRect rect = { position.x, position.y, size.w, size.h };
	Renderer::render_texture(camera, ResourcesManager::instance()->find_texture("gameover"), nullptr, &rect, 0, { 0.5f, 0.5f });
}

// ====================================================================================

RestartButton::RestartButton()
{
	this->position = { 300.0f, 500.0f };
	this->size = { 300.0f, 41.0f };
}

void RestartButton::on_normal()
{
	this->size = { 300.0f, 41.0f };
}

void RestartButton::on_hover()
{
	this->size = { 330.0f, 45.1f };
}

void RestartButton::on_click()
{
	Mix_PlayChannel(-1, ResourcesManager::instance()->find_audio("button"), 0);
	SceneManager::instance()->switch_to("game_scene");
}

void RestartButton::on_render(const Camera& camera)
{
	SDL_FRect rect = { position.x, position.y, size.w, size.h };
	Renderer::render_texture(camera, ResourcesManager::instance()->find_texture("again"), nullptr, &rect, 0, { 0.5f, 0.5f });
}

// ====================================================================================

PauseButton::PauseButton()
{
	this->position = { 560.0f, 30.0f };
	this->size = { 60.0f, 45.0f };
	cur_texture = ResourcesManager::instance()->find_texture("pause_nor");
}

PauseButton::~PauseButton()
{
	SDL_DestroyTexture(cur_texture);
}

void PauseButton::on_normal()
{
	if (is_pause) cur_texture = ResourcesManager::instance()->find_texture("resume_nor");
	else cur_texture = ResourcesManager::instance()->find_texture("pause_nor");
}

void PauseButton::on_hover()
{
	if (is_pause) cur_texture = ResourcesManager::instance()->find_texture("resume_pressed");
	else cur_texture = ResourcesManager::instance()->find_texture("pause_pressed");
}

void PauseButton::on_click()
{
	Mix_PlayChannel(-1, ResourcesManager::instance()->find_audio("upgrade"), 0);
	is_pause = !is_pause;
	if (is_pause)
	{
		Mix_PauseMusic();
		Clock::set_time_scale(0.0f);
	}
	else
	{
		Mix_ResumeMusic();
		Clock::set_time_scale(1.0f);
	}
}

void PauseButton::on_render(const Camera& camera)
{
	SDL_FRect rect = { position.x, position.y, size.w, size.h };
	Renderer::render_texture(camera, cur_texture, nullptr, &rect, 0, { 0.5f, 0.5f });
}