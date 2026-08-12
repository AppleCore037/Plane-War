module;

#include <SDL3/SDL.h>

export module Buttons;

import FlexCore;
using namespace fce;

// UI按钮接口
export class IButton
{
public:
	IButton() = default;
	virtual ~IButton() = default;

	void on_input(const SDL_Event& event)
	{
		// 判断鼠标是否在按钮上悬停
		bool in_range_x = event.motion.x >= position.x - size.w / 2.0f && event.motion.x <= position.x + size.w / 2.0f;
		bool in_range_y = event.motion.y >= position.y - size.h / 2.0f && event.motion.y <= position.y + size.h / 2.0f;

		if (event.type == SDL_EVENT_MOUSE_MOTION)	// 鼠标悬停在按钮上
		{
			if (in_range_x && in_range_y)
			{
				this->on_hover();
				this->is_first_hover = false;
				SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER));
			}
			else
			{
				this->on_normal();
				this->is_first_hover = true;
				SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT));
			}
		}
		if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) // 按钮被点击
		{
			if (in_range_x && in_range_y)
				this->on_click();
		}
		if (event.type == SDL_EVENT_MOUSE_BUTTON_UP && event.button.button == SDL_BUTTON_LEFT) // 按钮被释放
		{
			if (in_range_x && in_range_y)
			{
				this->on_hover();
				SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER));
			}
			else
			{
				this->on_normal();
				SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT));
			}
		}
	}

	virtual void on_render(const Camera& camera) {}

protected:
	virtual void on_normal() = 0;	// 正常
	virtual void on_hover() = 0;	// 鼠标悬空
	virtual void on_click() = 0;	// 被点击

protected:
	Vector2 position = { 0, 0 }; // 位置
	Size size = { 0, 0 };		 // 大小
	bool is_first_hover = true;	 // 是否首次悬停
};


export class ExitButton : public IButton
{
public:
	ExitButton();
	~ExitButton() override = default;

	void on_normal() override;
	void on_hover() override;
	void on_click() override;

	void on_render(const Camera& camera) override;
};

export class RestartButton : public IButton
{
public:
	RestartButton();
	~RestartButton() override = default;

	void on_normal() override;
	void on_hover() override;
	void on_click() override;

	void on_render(const Camera& camera) override;
};

export class PauseButton : public IButton
{
public:
	PauseButton();
	~PauseButton() override;

	void on_normal() override;
	void on_hover() override;
	void on_click() override;

	void on_render(const Camera& camera) override;

private:
	SDL_Texture* cur_texture = nullptr;
	bool is_pause = false;
};