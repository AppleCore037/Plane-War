#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <cmath>
#include <functional>
#include <string>
#include <vector>

class Atlas;
class Vector2;
class Timer;
class Camera;
class Animation;
class Text;

// ȫ�ֱ���
extern bool is_bombed;

// ͼ��
class Atlas
{
public:
	Atlas() = default;

	~Atlas()
	{
		for (SDL_Texture* texture : tex_list)
			SDL_DestroyTexture(texture);
	}

	// ���ض�·����������
	void load(SDL_Renderer* renderer, const char* path_template, int num)
	{
		for (int i = 0; i < num; i++)
		{
			char path_file[256];
			sprintf_s(path_file, path_template, i + 1);

			SDL_Texture* texture = IMG_LoadTexture(renderer, path_file);
			tex_list.push_back(texture);
		}
	}

	// ���
	void clear()
	{
		tex_list.clear();
	}

	// ��ȡ��С
	int get_size() const
	{
		return (int)tex_list.size();
	}

	// ��ȡ����
	SDL_Texture* get_texture(int idx)
	{
		if (idx < 0 || idx >= tex_list.size())
			return nullptr;

		return tex_list[idx];
	}

	// ��ӵ�������
	void add_texture(SDL_Texture* texture)
	{
		tex_list.push_back(texture);
	}

private:
	std::vector<SDL_Texture*> tex_list;		// ����
};

// ��ά����
class Vector2
{
public:
	float x = 0.0;
	float y = 0.0;

public:
	Vector2() = default;

	~Vector2() = default;

	Vector2(float x, float y) : x(x), y(y) {}

	Vector2 operator+(const Vector2& vct2) const
	{
		return Vector2(x + vct2.x, y + vct2.y);
	}

	Vector2 operator-(const Vector2& vct2)
	{
		return Vector2(x - vct2.x, y - vct2.y);
	}

	Vector2 operator*(float val)
	{
		return Vector2(x * val, y * val);
	}

	Vector2 operator/(float val)
	{
		return Vector2(x / val, y / val);
	}

	float operator*(const Vector2& vct2)
	{
		return x * vct2.x + y * vct2.y;
	}

	void operator+=(const Vector2& vct2)
	{
		this->x += vct2.x;
		this->y += vct2.y;
	}

	void operator-=(const Vector2& vct2)
	{
		this->x -= vct2.x;
		this->y -= vct2.y;
	}

	void operator/=(float val)
	{
		this->x /= val;
		this->y /= val;
	}

	void operator*=(float val)
	{
		this->x *= val;
		this->y *= val;
	}

	// ��ȡ����
	float length() const
	{
		return sqrt(x * x + y * y);
	}

	// һ�㻯
	Vector2 normalize()
	{
		float len = this->length();

		if (len == 0)
			return Vector2(0, 0);
		return Vector2(x / len, y / len);
	}
};

// ��ʱ��
class Timer
{
public:
	Timer() = default;

	~Timer() = default;

	// ������ʱ��
	void restart()
	{
		pass_time = 0;
		shotted = false;
	}

	// ���õȴ�ʱ��
	void set_wait_time(float val)
	{
		wait_time = val;
	}

	// ���õ��δ���
	void set_one_shot(bool flag)
	{
		one_shot = flag;
	}

	// ���ûص�����
	void set_on_timeout(std::function<void()> on_timeout)
	{
		this->on_timeout = on_timeout;
	}

	// ��ͣ
	void pause()
	{
		paused = true;
	}

	// ����
	void resume()
	{
		paused = false;
	}

	// ���¼�ʱ��
	void on_update(float delta)
	{
		if (paused)
			return;

		pass_time += delta;
		if (pass_time >= wait_time)
		{
			bool can_shot = (!one_shot || (one_shot && !shotted));
			shotted = true;

			if (can_shot && on_timeout)
				on_timeout();
			pass_time -= wait_time;
		}
	}

private:
	float pass_time = 0;				// ����ʱ��
	float wait_time = 0;				// �ȴ�ʱ��
	bool paused = false;				// ��ͣ
	bool shotted = false;				// �Ƿ񴥷�
	bool one_shot = false;				// ���δ���
	std::function<void()> on_timeout;	// �ص�����
};

// �����
class Camera
{
public:
	Camera(SDL_Renderer* renderer)
	{
		this->renderer = renderer;

		timer_shake.set_one_shot(true);
		timer_shake.set_on_timeout([&]()
			{
				is_bombed = false;
				is_shaking = false;
				reset();
			});
	}

	~Camera() = default;

	// ��ȡλ��
	const Vector2& get_position() const
	{
		return position;
	}

	// ����λ��
	void set_position(float x, float y)
	{
		this->position.x = x;
		this->position.y = y;
	}

	// ����λ��
	void set_position(const Vector2& pos)
	{
		this->position = pos;
	}

	// ���������
	void reset()
	{
		position.x = 0;
		position.y = 0;
	}

	// ���������
	void shake(float strength, float duration)
	{
		is_shaking = true;
		shaking_strength = strength;

		timer_shake.set_wait_time(duration);
		timer_shake.restart();
	}

	// ��Ⱦ����
	void render_texture(SDL_Texture* texture, const SDL_Rect* rect_src,
		const SDL_FRect* rect_dst, double angle, const SDL_FPoint* center) const
	{
		SDL_FRect rect_dst_win = *rect_dst;
		rect_dst_win.x -= position.x;
		rect_dst_win.y -= position.y;

		SDL_RenderCopyExF(renderer, texture, rect_src,
			&rect_dst_win, angle, center, SDL_RendererFlip::SDL_FLIP_NONE);
	}

	// ���ƾ���
	void render_rect(const SDL_Rect* rect, SDL_Color color) const
	{
		SDL_Rect rect_dst_win = *rect;
		rect_dst_win.x -= (int)position.x;
		rect_dst_win.y -= (int)position.y;

		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
		SDL_RenderFillRect(renderer, &rect_dst_win);
	}

	// ���������
	void on_update(float delta)
	{
		timer_shake.on_update(delta);

		if (is_shaking)
		{
			position.x = (-50 + rand() % 100) / 50.0f * shaking_strength;
			position.y = (-50 + rand() % 100) / 50.0f * shaking_strength;
		}
	}

private:
	Vector2 position;					// λ��
	Timer timer_shake;					// ���������
	bool is_shaking = false;			// �Ƿ񶶶�
	float shaking_strength = 0;			// ��������
	SDL_Renderer* renderer = nullptr;	// ������Ⱦ��
};

// ����
class Animation
{
public:
	Animation()
	{
		timer.set_one_shot(false);
		timer.set_on_timeout([&]()
			{
				idx_frame++;
				if (idx_frame >= frame_list.size())
				{
					idx_frame = is_loop ? 0 : frame_list.size() - 1;
					if (!is_loop && on_finished)
						on_finished();
				}
			});
	}

	~Animation() = default;

	// ���ö���
	void reset()
	{
		timer.restart();
		idx_frame = 0;
	}

	// ����λ��
	void set_position(const Vector2& position)
	{
		this->position = position;
	}

	// ������ת�Ƕ�
	void set_rotation(double angle)
	{
		this->angle = angle;
	}

	// �������ĵ�
	void set_center(const SDL_FPoint& cneter)
	{
		this->center = center;
	}

	// �����Ƿ�ѭ��
	void set_loop(bool is_loop)
	{
		this->is_loop = is_loop;
	}

	// ����֡���
	void set_interval(float interval)
	{
		timer.set_wait_time(interval);
	}

	// ���ûص�����
	void set_on_finished(std::function<void()> on_finished)
	{
		this->on_finished = on_finished;
	}

	// �������֡
	void add_frame(SDL_Texture* texture, int num_h)
	{
		int width, height;
		SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);

		int width_frame = width / num_h;

		for (int i = 0; i < num_h; i++)
		{
			SDL_Rect rect_src;
			rect_src.x = i * width_frame, rect_src.y = 0;
			rect_src.w = width_frame, rect_src.h = height;

			frame_list.emplace_back(texture, rect_src);
		}
	}

	// �������֡
	void add_frame(Atlas* atlas)
	{
		for (int i = 0; i < atlas->get_size(); i++)
		{
			SDL_Texture* texture = atlas->get_texture(i);

			int width, height;
			SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);

			SDL_Rect rect_src;
			rect_src.x = 0, rect_src.y = 0;
			rect_src.w = width, rect_src.h = height;

			frame_list.emplace_back(texture, rect_src);
		}
	}

	// ���¶���
	void on_update(float delta)
	{
		timer.on_update(delta);
	}

	// ��Ⱦ����
	void on_render(const Camera& camera) const
	{
		const Frame& frame = frame_list[idx_frame];
		const Vector2& pos_camera = camera.get_position();

		SDL_FRect rect_dst{};
		rect_dst.x = position.x - frame.rect_src.w / 2.0f;
		rect_dst.y = position.y - frame.rect_src.h / 2.0f;
		rect_dst.w = (float)frame.rect_src.w;
		rect_dst.h = (float)frame.rect_src.h;

		camera.render_texture(frame.texture, &frame.rect_src, &rect_dst, angle, &center);
	}

private:
	// ����֡�ṹ��
	struct Frame
	{
		SDL_Rect rect_src;
		SDL_Texture* texture = nullptr;

		Frame() = default;
		Frame(SDL_Texture* texture, const SDL_Rect& rect_src)
			: texture(texture), rect_src(rect_src) {}

		~Frame() = default;
	};

private:
	Vector2 position;					// λ��
	double angle = 0;					// �Ƕ�
	SDL_FPoint center = { 0 };			// ���ĵ�

	Timer timer;						// ���ü�ʱ��
	bool is_loop = true;				// �Ƿ�ѭ��
	size_t idx_frame = 0;				// ��ǰ֡����
	std::vector<Frame> frame_list;		// ����֡�б�
	std::function<void()> on_finished;	// �������Ž����ص�����
};

// �ı�(����SDL_ttf�ķ�װ��)
class Text
{
public:
	Text() = default;

	Text(SDL_Renderer* renderer, const std::string& font_path = "C:/Windows/Fonts/arial.ttf",
		int size = 24, SDL_Color color = { 255,255,255,255 })
	{
		this->renderer = renderer;
		this->font = TTF_OpenFont(font_path.c_str(), size);
		this->size = size;
		this->color = color;
		this->texture = nullptr;
		this->font_path = font_path;

		this->update_texture();
	}

	~Text()
	{
		// TTF_CloseFont(font);
		SDL_DestroyTexture(texture);
		SDL_DestroyRenderer(renderer);
	}

	// ��������
	void set_font(std::string& font_path)
	{
		this->font = TTF_OpenFont(font_path.c_str(), size);
		this->font_path = font_path;
		this->update_texture();
	}

	// ������ɫ
	void set_color(SDL_Color color)
	{
		this->color = color;
		this->update_texture();
	}

	// ���ô�С
	void set_size(int size)
	{
		this->size = size;
		this->font = TTF_OpenFont(font_path.c_str(), size);
		this->update_texture();
	}

	// �����ı�����
	template <typename _Ty>
	void set_text(const _Ty& text)
	{
		this->text = std::string(text);
		this->update_texture();
	}

	// ��Ⱦ����
	void on_render(float x, float y)
	{
		SDL_Rect rect_dst = { (int)x, (int)y, width, height };
		SDL_RenderCopy(renderer, texture, nullptr, &rect_dst);
	}

	// ��Ⱦ����
	void on_render(const Vector2& pos)
	{
		SDL_Rect rect_dst = { (int)pos.x, (int)pos.y, width, height };
		SDL_RenderCopy(renderer, texture, nullptr, &rect_dst);
	}

private:
	// ������������
	void update_texture()
	{
		if (texture)
		{
			SDL_DestroyTexture(texture);
			texture = nullptr;
		}

		SDL_Surface* surf_text = TTF_RenderText_Solid(font, text.c_str(), color);
		if (surf_text)
		{
			texture = SDL_CreateTextureFromSurface(renderer, surf_text);
			width = surf_text->w;
			height = surf_text->h;
			SDL_FreeSurface(surf_text);
		}
	}

private:
	SDL_Renderer* renderer;					// ������Ⱦ��
	SDL_Texture* texture = nullptr;			// ����

	SDL_Color color;						// ��ɫ
	int size = 0;							// ��С
	TTF_Font* font = nullptr;				// ����
	std::string font_path;					// ����·��
	std::string text;						// �ı�����
	int width, height;						// ����
};