module;

#include <algorithm>
#include <any>
#include <array>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <format>
#include <functional>
#include <iostream>
#include <memory>
#include <random>
#include <stdexcept>
#include <string>
#include <thread>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>

export module FlexCore;
export import <fstream>;

constexpr float PI = 3.14159265f;	// 圆周率
constexpr SDL_InitFlags SDL_INIT_EVERYTHING = (SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMEPAD | SDL_INIT_SENSOR);
constexpr MIX_InitFlags MIX_INIT_EVERYTHING = (MIX_INIT_MP3 | MIX_INIT_FLAC | MIX_INIT_MID | MIX_INIT_MOD | MIX_INIT_OGG | MIX_INIT_OPUS | MIX_INIT_WAVPACK);
SDL_ScaleMode scale_mode = SDL_SCALEMODE_NEAREST;	// 缩放模式(默认是Nearest)

export namespace fce
{
	// ============================================= 基础设置 =============================================

	SDL_Window* Main_Window = nullptr;			// 主窗口
	SDL_Renderer* Main_Renderer = nullptr;		// 主渲染器
	TTF_TextEngine* Main_TextEngine = nullptr;	// 主文字引擎
	SDL_Event Main_Event;						// 主循环事件

	// 预定义颜色
	namespace colors
	{
		SDL_Color Red = { 255, 0, 0, 255 };			 // 红
		SDL_Color Blue = { 0, 0, 255, 255 };		// 蓝
		SDL_Color Green = { 0, 255, 0, 255 };		 // 绿
		SDL_Color White = { 255, 255, 255, 255 };	 // 白
		SDL_Color Black = { 0, 0, 0, 255 };			 // 黑
		SDL_Color Yellow = { 255, 255, 0, 255 };	 // 黄
		SDL_Color Pink = { 255, 0, 255, 255 };		 // 粉
		SDL_Color LightBlue = { 0, 255, 255, 255 };	 // 浅蓝
		SDL_Color LightGray = { 128, 128, 128, 255 }; // 浅灰
		SDL_Color DarkGray = { 50, 50, 50, 255 };	 // 暗灰
		SDL_Color Orange = { 255, 165, 0, 255 };	// 橙色
		SDL_Color Purple = { 128, 0, 128, 255 };	// 紫色
	}

	// 窗口标签
	enum class WindowFlags
	{
		Default = SDL_EVENT_WINDOW_SHOWN,	// 显示
		Hidden = SDL_WINDOW_HIDDEN,			// 隐藏
		Resizable = SDL_WINDOW_RESIZABLE,	// 可调整大小
		Maximized = SDL_WINDOW_MAXIMIZED,	// 最大化
		Minimized = SDL_WINDOW_MINIMIZED,	// 最小化
		Fullscreen = SDL_WINDOW_FULLSCREEN	// 全屏
	};

	// 消息框类型
	enum class MsgBoxFlags
	{
		Info = SDL_MESSAGEBOX_INFORMATION,	// 信息
		Warning = SDL_MESSAGEBOX_WARNING,	// 警告
		Error = SDL_MESSAGEBOX_ERROR,		// 错误
	};

	// 渲染层级
	enum class RenderLayer
	{
		None,			// 无渲染层
		Background,		// 背景层
		Frontground,	// 前景层
		Label,			// 游戏内部文本层
		GameObject,		// 游戏元素层
		Effect,			// 特效层
		UI				// UI层
	};

	// 碰撞层级
	enum class CollisionLayer
	{
		None = 0,				// 无碰撞层
		Player = 1 << 0,		// 玩家层
		Enemy = 1 << 1,			// 敌人层
		GameMap = 1 << 2,		// 游戏地图层
		GameObject = 1 << 3,	// 游戏元素层
		Attack = 1 << 4,		// 攻击层
		Vehicle = 1 << 5		// 载具层
	};

	// 缩放模式
	enum class ScaleMode
	{
		Linear,		// 线性插值
		Nearest,	// 最近临插值
		Best		// 最佳材质
	};

	// 重载按位或运算符 |
	inline constexpr CollisionLayer operator|(CollisionLayer lhs, CollisionLayer rhs)
	{
		using underlying = std::underlying_type_t<CollisionLayer>;
		return static_cast<CollisionLayer>(static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
	}

	// 重载按位与运算符 &
	inline constexpr bool operator&(CollisionLayer lhs, CollisionLayer rhs)
	{
		using underlying = std::underlying_type_t<CollisionLayer>;
		return (static_cast<underlying>(lhs) & static_cast<underlying>(rhs)) != 0;
	}

	// ============================================= 基础类型 =============================================

	// 大小
	struct Size
	{
		Size() = default;
		Size(float w, float h) : w(w), h(h) {}

		float w = 0.0f;
		float h = 0.0f;
	};

	// 二维向量
	class Vector2
	{
	public:
		float x = 0.0f;
		float y = 0.0f;

	public:
		Vector2() = default;

		~Vector2() = default;

		Vector2(float x, float y) : x(x), y(y) {}

		Vector2 operator+(const Vector2& vct2) const { return Vector2(x + vct2.x, y + vct2.y); }

		Vector2 operator-(const Vector2& vct2) const { return Vector2(x - vct2.x, y - vct2.y); }

		Vector2 operator-() const { return Vector2(-x, -y); }

		Vector2 operator*(float val) { return Vector2(x * val, y * val); }

		Vector2 operator/(float val) { return Vector2(x / val, y / val); }

		float operator*(const Vector2& vct2) { return x * vct2.x + y * vct2.y; }

		void operator+=(const Vector2& vct2) { x += vct2.x; y += vct2.y; }

		void operator-=(const Vector2& vct2) { x -= vct2.x; y -= vct2.y; }

		void operator/=(float val) { x /= val; y /= val; }

		void operator*=(float val) { x *= val; y *= val; }

		// 获取向量模长
		float length() const { return sqrtf(x * x + y * y); }

		// 检测是否趋近与目标向量
		bool approx(const Vector2& target) const { return (*this - target).length() <= 0.001f; }

		// 将向量旋转指定弧度
		Vector2 rotate(float angle)
		{
			float _cos_a = std::cosf(angle), _sin_a = std::sinf(angle);
			return Vector2(x * _cos_a - y * _sin_a, x * _sin_a + y * _cos_a);
		}

		// 获取法向量(逆时针旋转90°)
		Vector2 get_normal() { return Vector2(-y, x).normalize(); }

		// 将向量标准化为单位向量
		Vector2 normalize() const
		{
			float _len = this->length();
			if (_len == 0) return Vector2(0, 0);
			return Vector2(x / _len, y / _len);
		}
	};

	// 图集
	class Atlas
	{
	public:
		Atlas() = default;
		Atlas(const char* path_template, int beg, int end) { load_from_file(path_template, beg, end); }

		~Atlas()
		{
			for (SDL_Texture* _texture : tex_list)
				SDL_DestroyTexture(_texture);
		}

		// 从指定路径模板加载目标数量纹理
		void load_from_file(const char* path_template, int beg_idx, int end_idx)
		{
			for (int i = beg_idx; i <= end_idx; i++)
			{
				char _path_file[256];
				sprintf_s(_path_file, path_template, i);	// 补全路径

				SDL_Texture* _texture = IMG_LoadTexture(Main_Renderer, _path_file);	// 加载纹理
				SDL_SetTextureScaleMode(_texture, scale_mode);

				if (_texture == nullptr)	// 检查纹理加载状态
				{
					std::string _info = "[load_from_file()]: Cannot load texture from \"" + std::string(_path_file) + "\" !";
					throw std::runtime_error(_info.c_str());
				}

				tex_list.push_back(_texture);	// 加入纹理列表
			}
		}

		// 清空图集
		void clear() { tex_list.clear(); }

		// 获取图集纹理数量
		int get_size() const { return (int)tex_list.size(); }

		// 添加纹理
		void add_texture(SDL_Texture* texture)
		{
			SDL_SetTextureScaleMode(texture, scale_mode);
			tex_list.push_back(texture);
		}

		// 获取纹理
		SDL_Texture* get_texture(int idx)
		{
			if (idx < 0 || idx >= tex_list.size()) return nullptr;
			return tex_list[idx];
		}

	private:
		std::vector<SDL_Texture*> tex_list;		// 纹理集
	};

	// 文本
	class Text
	{
	public:
		Text()
		{
			if (!global_font) throw std::runtime_error("[Text Constructor]: Text default font not set!");
			this->text = TTF_CreateText(Main_TextEngine, global_font, nullptr, NULL);
		}

		Text(const std::string& info)
		{
			if (!global_font) throw std::runtime_error("[Text Constructor]: Text default font not set!");
			this->text = TTF_CreateText(Main_TextEngine, global_font, info.c_str(), NULL);
		}

		~Text() { TTF_DestroyText(text); }

		// 设置全局字体
		static void set_global_font(TTF_Font* font) { global_font = font; }

		// 设置私有字体
		void set_self_font(TTF_Font* font)
		{
			if (!font) TTF_SetTextFont(text, global_font);
			else TTF_SetTextFont(text, font);
		}

		// 设置文本内容
		void set_string(const std::string& info) { TTF_SetTextString(text, info.c_str(), NULL); }

		// 获取SDL格式的文本
		TTF_Text* get_SDLText() { return this->text; }

	private:
		TTF_Text* text = nullptr;
		static TTF_Font* global_font;
	};
	TTF_Font* Text::global_font = nullptr;

	// 精灵属性结构体
	struct Property
	{
	public:
		Vector2 position;					// 位置
		Size size;							// 尺寸
		Vector2 anchor = { 0.0f, 0.0f };	// 锚点
		float direction = 0.0f;				// 方向

		bool can_remove = false;				// 是否标记删除
		bool is_flip = false;					// 是否翻转
		RenderLayer layer = RenderLayer::None;	// 渲染层
		float alpha = 100.0f;					// 透明度（百分比）

		std::string group_tag = "";	 // 所属组标签
		std::string tag = "";		 // 独立标签

		// 获取SDL形式的矩形
		const SDL_FRect& get_SDLRect() const
		{
			SDLRect = { position.x, position.y, size.w, size.h };
			return SDLRect;
		}

	private:
		mutable SDL_FRect SDLRect;	// SDL形式矩形
	};

	// 碰撞信息上下文
	class CollideCtx
	{
		using map_t = std::unordered_map<std::string, std::any>;
	public:
		Vector2 normal;				// 碰撞法线(指向自身)
		float depth;				// 碰撞深度
		CollisionLayer other_layer;	// 对方碰撞层

	public:
		CollideCtx() = default;
		~CollideCtx() = default;

		template <typename T>
		void pack(const std::string& name, const T& value)
		{
			map_t& map = bags[std::type_index(typeid(T))];
			map[name] = value;
		}

		template <typename T>
		T get(const std::string& name) const
		{
			auto it = bags.find(std::type_index(typeid(T)));
			if (it != bags.end())
			{
				auto val_it = it->second.find(name);
				if (val_it != it->second.end()) return std::any_cast<T>(val_it->second);
			}

			std::string _info = std::format("[CollisionInfo::get()]: Cannot get value!\ntype: {}\nname: {}", typeid(T).name(), name);
			throw std::runtime_error(_info.c_str());
		}

		template <typename T>
		bool has(const std::string& name) const
		{
			auto it = bags.find(std::type_index(typeid(T)));
			if (it != bags.end())
				return (it->second.find(name) != it->second.end());
			else
				return false;
		}

	private:
		std::unordered_map<std::type_index, map_t> bags;
	};

	// ============================================= 实用工具 =============================================

	// 进程错误处理
	class custom_error
	{
	public:
		custom_error(const std::string& title, const std::string& info)
			: error_title(title), error_info(info) {}

		custom_error(const char* title, const char* info)
			: error_title(title), error_info(info) {}

		~custom_error() = default;

		// 获取错误标题
		const char* title() const noexcept { return error_title.c_str(); }

		// 获取错误信息
		const char* what() const noexcept {	return error_info.c_str(); }

	private:
		std::string error_title; // 错误标题
		std::string error_info;	 // 错误信息
	};

	// 随机数生成器
	class Random
	{
	public:
		// 生成 [min, max] 范围内的随机整数
		static int randint(int min, int max)
		{
			if (min > max) std::swap(min, max);
			std::uniform_int_distribution<int> dist(min, max);
			return dist(instance().engine);
		}

		// 生成 [min, max] 范围内的随机浮点数
		static float randfloat(float min, float max)
		{
			if (min > max) std::swap(min, max);
			std::uniform_real_distribution<float> dist(min, max);
			return dist(instance().engine);
		}

	private:
		Random() { engine.seed(std::random_device()()); }
		~Random() = default;

		// 获取全局单例
		static Random& instance()
		{
			static Random inst;
			return inst;
		}

	private:
		std::mt19937 engine; // 随机数引擎
	};

	// 全局游戏时钟
	class Clock
	{
		using clock_t = std::chrono::steady_clock;
		using ms_t = std::chrono::milliseconds;

	public:
		// 起始当前帧计时
		static void start_frame()
		{
			Clock& inst = instance();
			auto _past_time = clock_t::now() - inst.last_time;
			inst.delta_time = static_cast<double>(std::chrono::duration_cast<ms_t>(_past_time).count());
			inst.last_time = clock_t::now();	// 更新上次刷新时间
		}

		// 结束当前帧计时
		static void end_frame()
		{
			Clock& inst = instance();

			auto _cur_time = clock_t::now();		// 获取当前时间
			auto _elp_time = std::chrono::duration_cast<ms_t>(_cur_time - inst.last_time).count(); // 经过时间

			// 休眠指定时间
			if (_elp_time < inst.target_dur && inst.is_frame_limited)
				std::this_thread::sleep_for(ms_t(inst.target_dur - _elp_time));
			else
				std::this_thread::sleep_for(ms_t(inst.MIN_DUR));
		}

		// 设置是否垂直同步（默认false）
		static void set_VSync(bool is_abled)
		{
			Clock& inst = instance();
			if (is_abled) inst.target_dur = 1000 / inst.get_screen_refreshRate();
			else inst.target_dur = 1000 / inst.target_fps;	// 设置为目标FPS
		}

		// 设置FPS
		static void set_fps(int fps_limit)
		{
			instance().target_fps = fps_limit;		// 设置目标FPS
			instance().target_dur = 1000 / fps_limit;
		}

		// 获取全局经过时间（秒）
		static float get_global_time()
		{
			auto _cur_time = clock_t::now();
			auto _global_time = std::chrono::duration_cast<ms_t>(_cur_time - instance().global_start_time).count();
			return static_cast<float>(_global_time) / 1000.0f;	// 转换为秒
		}

		// 重置全局经过时间
		static void restart_global_time() { instance().global_start_time = clock_t::now(); }

		// 设置是否启用帧率限制（默认是true）
		static void set_frame_limit(bool flag) { instance().is_frame_limited = flag; }

		// 获取FPS
		static size_t get_fps() { return static_cast<size_t>(1000 / instance().delta_time); }

		// 获取帧间隔
		static float get_DeltaTime()
		{
			double _normal_delta = instance().delta_time / 1000.0;	// 转换为秒
			return static_cast<float>(_normal_delta * instance().time_scale);
		}

		// 设置时间缩放
		static void set_time_scale(double scale)
		{
			if (scale < 0.0f) instance().time_scale = 0.0f;
			else instance().time_scale = scale;
		}

	private:
		Clock()
		{
			this->last_time = clock_t::now();
			this->global_start_time = clock_t::now();
		}

		~Clock() = default;

		// 获取单例
		static Clock& instance()
		{
			static Clock inst;
			return inst;
		}

		// 获取屏幕刷新率（获取失败默认返回60）
		int get_screen_refreshRate() const
		{
			// 获取主显示器（默认显示器）
			SDL_DisplayID _display = SDL_GetPrimaryDisplay();
			if (!_display) return 0;

			// 获取当前显示模式
			const SDL_DisplayMode* _mode = SDL_GetCurrentDisplayMode(_display);
			if (!_mode) return 0;

			// 返回刷新率（如果有效，否则返回默认值60）
			return (_mode->refresh_rate > 0) ? (int)_mode->refresh_rate : 60;
		}

	private:
		int target_fps = 60;			// 目标FPS
		int target_dur = 1000 / 60;		// 目标帧间隔
		double delta_time = 0;			// 单帧间隔
		double time_scale = 1.0;		// 时间缩放
		bool is_frame_limited = true;	// 是否启用帧率限制

		clock_t::time_point last_time;			// 上次刷新时间
		clock_t::time_point global_start_time;	// 全局起始时间

		const int MIN_DUR = 1000 / 999;	// 最小帧间隔
	};

	// 数据读写器
	class Data
	{
	public:
		enum Mod : std::ios::openmode
		{
			Override = std::ios::out, // 覆盖
			Append = std::ios::app	  // 追加
		};

	public:
		// 清空文件
		static void clear(const std::string& fname)
		{
			std::string _file_name = (fname.find(".kvp") == fname.npos ? fname + ".kvp" : fname);

			std::ifstream _reader = std::ifstream(_file_name, std::ios::in);
			if (!_reader.is_open()) return;

			// 用std::ios::trunc清空
			std::ofstream _writer = std::ofstream(_file_name, std::ios::trunc);
			_writer.close();
		}

		// 保存数据
		template <typename T>
		static void save(const std::string& fname, const std::string& key, T val, Mod mod = Append)
		{
			Data& _inst = instance();
			bool _is_std = (_inst.type_map.find(type<T>()) != _inst.type_map.end());	// 是否是C++标准类型
			bool _is_custom = (std::is_trivially_copyable_v<T> && std::is_standard_layout_v<T>); // 是否是合法自定义类型
			bool _is_pointer = (std::is_pointer_v<T> && !std::is_same_v<T, const char*>);	// 是否是指针类型(除const char*)

			// 都不是，或是指针类型，报错
			if (!_is_std && !_is_custom || _is_pointer)
			{
				std::string _info = std::format("Type <{}> is a invalid type!", typeid(T).name());
				throw custom_error("Data Error", _info.c_str());
			}

			std::string _file_name = (fname.find(".kvp") == fname.npos ? fname + ".kvp" : fname);
			std::ofstream _writer = std::ofstream(_file_name, mod | std::ios::binary);

			std::string _key_str = serialization(key);							// 键
			std::string _type_str = serialization(_inst.type_map[type<T>()]);	// 类型
			std::string _val_str = serialization(val);							// 值
			std::string _jump_bit = serialization(_val_str.size());				// 可忽略字节

			// 按模板布局依次写入
			_writer.write(_key_str.c_str(), _key_str.length());
			_writer.write(_type_str.c_str(), _type_str.length());
			_writer.write(_jump_bit.c_str(), _jump_bit.length());
			_writer.write(_val_str.c_str(), _val_str.length());
		}

		// 读取数据
		template <typename T>
		static T read(const std::string& fname, const std::string& key)
		{
			Data& _inst = instance();
			bool _is_std = (_inst.type_map.find(type<T>()) != _inst.type_map.end());	// 是否是C++标准类型
			bool _is_custom = (std::is_trivially_copyable_v<T> || std::is_standard_layout_v<T>); // 是否是合法自定义类型

			if (!_is_std && !_is_custom)	// 都不是，报错
			{
				std::string _info = std::format("Type <{}> is a invalid type!", typeid(T).name());
				throw custom_error("Data Error", _info.c_str());
			}
			else if constexpr (std::is_pointer_v<T>) // 禁用指针传参，防悬空指针
			{
				std::string _info = "Don't use pointers to receive read data.\nUse <std::string> for strings.";
				throw custom_error("Data Error", _info.c_str());
			}

			std::string _file_name = (fname.find(".kvp") == fname.npos ? fname + ".kvp" : fname);
			std::ifstream _reader = std::ifstream(_file_name, std::ios::in | std::ios::binary);

			// 打开失败，返回默认值
			if (!_reader.is_open()) return T();

			// 读取
			while (_reader.peek() != EOF)
			{
				std::string _read_key = deserialization<std::string>(_reader);  // 键
				uint8_t _read_type = deserialization<uint8_t>(_reader);			// 类型
				size_t _jump_bit = deserialization<size_t>(_reader);			// 可忽略字节

				// 找到符合目标
				if (_read_key == key && _read_type == _inst.type_map[type<T>()])
					return deserialization<T>(_reader);
				else
					_reader.ignore(_jump_bit);
			}

			return T(); // 返回默认构造
		}

	private:
		Data()
		{
			// 初始化类型映射表
			type_map = {
				{ std::type_index(typeid(bool)), 1U },
				{ std::type_index(typeid(char)), 2U },
				{ std::type_index(typeid(unsigned char)), 3U },
				{ std::type_index(typeid(int)), 4U },
				{ std::type_index(typeid(unsigned int)), 5U },
				{ std::type_index(typeid(long long)), 6U },
				{ std::type_index(typeid(unsigned long long)), 7U },
				{ std::type_index(typeid(float)), 8U },
				{ std::type_index(typeid(double)), 9U },
				{ std::type_index(typeid(long double)), 10U },
				{ std::type_index(typeid(short)), 11U },
				{ std::type_index(typeid(unsigned short)), 12U },
				{ std::type_index(typeid(std::string)), 13U },
				{ std::type_index(typeid(const char*)), 13U },

			};
		}

		~Data() = default;

		// 获取单例
		static Data& instance()
		{
			static Data inst;
			return inst;
		}

		// 获取类型索引
		template <typename T>
		static const std::type_index type() { return std::type_index(typeid(T)); }

		// 序列化
		template <typename T>
		static const std::string serialization(T val)
		{
			/* 序列化模板布局
			* [[len][content]][		 ][			 ][			  ]
			*	   key          type    jump_bit      value
			*/

			// 字节接收缓冲区
			std::string _buffer;

			if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, const char*>) // 是字符串类型
			{
				std::string _tmp_str = std::string(val); // 临时字符串
				size_t _str_len = _tmp_str.length();	 // 字符串大小（用于取地址）

				_buffer.append(reinterpret_cast<const char*>(&_str_len), sizeof(size_t));
				_buffer.append(_tmp_str.c_str(), _tmp_str.length());
			}
			else
			{
				T _tmp_val = val;
				_buffer.append(reinterpret_cast<const char*>(&_tmp_val), sizeof(T));
			}

			return _buffer;
		}

		// 反序列化
		template <typename T>
		static T deserialization(std::ifstream& reader)
		{
			if constexpr (std::is_same_v<T, std::string>) // 是字符串类型
			{
				size_t _len = 0;	 // 字符串长度
				std::string _result; // 字符串内容

				reader.read((char*)&_len, sizeof(size_t));	// 读取长度
				_result.resize(_len);						// 预留空间
				reader.read(_result.data(), _len);			// 读取内容

				return _result;
			}
			else    // 不是字符串类型
			{
				T _result = T();
				reader.read((char*)&_result, sizeof(T));
				return _result;
			}
		}

	private:
		std::unordered_map<std::type_index, uint8_t> type_map; // 类型映射表
	};

	// 数学运算工具
	namespace maths
	{
		// 角度转弧度
		inline float deg_to_rad(float degree) { return degree * PI / 180.0f; }

		// 弧度转角度
		inline float rad_to_deg(float radian) { return radian * 180.0f / PI; }

		// 插值函数
		inline float lerp(float current, float target, float t) { return current + (target - current) * t; }

		// 计算两点距离
		inline float distance_to(const Vector2& pos_1, const Vector2& pos_2)
		{
			float _dx = pos_1.x - pos_2.x, _dy = pos_1.y - pos_2.y;
			return sqrtf((_dx * _dx) + (_dy * _dy));
		}

		// 摆动函数
		inline float swing(float min, float max, float strength)
		{
			if (max < min) std::swap(min, max);
			if (strength < 0) strength = -strength;

			// 摆动区间 = (max-min)÷2 * sin(dt*k) + (max+min)÷2
			return (max - min) / 2.0f * std::sin(Clock::get_global_time() * strength) + (max + min) / 2.0f;
		}

		// 绘制空心圆形
		inline void render_circle(const Vector2& center_pos, float radius, SDL_Color color)
		{
			if (radius <= 0) return;

			float _x = 0, _y = radius, _d = 3 - 2 * radius;
			SDL_SetRenderDrawColor(Main_Renderer, color.r, color.g, color.b, color.a);

			while (_x <= _y)
			{
				SDL_RenderPoint(Main_Renderer, center_pos.x + _x, center_pos.y + _y);
				SDL_RenderPoint(Main_Renderer, center_pos.x - _x, center_pos.y + _y);
				SDL_RenderPoint(Main_Renderer, center_pos.x + _x, center_pos.y - _y);
				SDL_RenderPoint(Main_Renderer, center_pos.x - _x, center_pos.y - _y);
				SDL_RenderPoint(Main_Renderer, center_pos.x + _y, center_pos.y + _x);
				SDL_RenderPoint(Main_Renderer, center_pos.x - _y, center_pos.y + _x);
				SDL_RenderPoint(Main_Renderer, center_pos.x + _y, center_pos.y - _x);
				SDL_RenderPoint(Main_Renderer, center_pos.x - _y, center_pos.y - _x);

				if (_d < 0)
				{
					_d = _d + 4 * _x + 6;
				}
				else
				{
					_d = _d + 4 * (_x - _y) + 10;
					_y -= 1.0f;
				}
				_x += 1.0f;
			}
		}

		// 绘制实心圆形
		inline void render_filled_circle(const Vector2& center_pos, float radius, SDL_Color color)
		{
			if (radius <= 0) return;

			SDL_SetRenderDrawColor(Main_Renderer, color.r, color.g, color.b, color.a);
			for (float _y = -radius; _y <= radius; _y += 1.0f)
			{
				int _x_length = static_cast<int>(sqrtf(radius * radius - _y * _y));
				SDL_RenderLine(Main_Renderer,
					center_pos.x - _x_length, center_pos.y + _y,
					center_pos.x + _x_length, center_pos.y + _y);
			}
		}
	};

	// 小工具
	namespace utils
	{
		// 获取键盘/鼠标事件的UTF-8格式字符串
		inline const char* Get_EventName(const SDL_Event& event)
		{
			static std::string _result = "None";

			if (event.type == SDL_EVENT_KEY_DOWN)
				_result = SDL_GetKeyName(event.key.key);
			if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
			{
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:	_result = "Left Mouse Button";   break;
				case SDL_BUTTON_MIDDLE: _result = "Middle Mouse Button"; break;
				case SDL_BUTTON_RIGHT:	_result = "Right Mouse Button";	 break;
				case SDL_BUTTON_X1:		_result = "Side Button 1";		 break;
				case SDL_BUTTON_X2:		_result = "Side Button 2";		 break;
				}
			}
			if (event.type == SDL_EVENT_MOUSE_WHEEL)
			{
				if (event.wheel.y > 0) _result = "Mouse Wheel Up";
				if (event.wheel.y < 0) _result = "Mouse Wheel Down";
			}
			if (event.type == SDL_EVENT_MOUSE_MOTION)
				_result = "Mouse Motion";

			return _result.c_str();
		}

		// 显示消息框
		inline void Show_MessageBox(MsgBoxFlags type, const char* title, const char* message)
		{
			// 检测主窗口是否存在
			if (Main_Window != nullptr)
				SDL_ShowSimpleMessageBox((SDL_MessageBoxFlags)type, title, message, Main_Window);
			else
				SDL_ShowSimpleMessageBox((SDL_MessageBoxFlags)type, title, message, nullptr);
		}

		// 设置缩放模式
		inline void Set_ScaleMode(ScaleMode mode)
		{
			switch (mode)
			{
			case ScaleMode::Linear: scale_mode = SDL_SCALEMODE_LINEAR; break;
			case ScaleMode::Nearest: scale_mode = SDL_SCALEMODE_NEAREST; break;
			case ScaleMode::Best: scale_mode = SDL_SCALEMODE_PIXELART; break;
			}
		}
	}

	// ============================================= 图形渲染 =============================================

	/*
	* 笔记：
	* 游戏渲染有三个次元：
	*	1.屏幕空间	  物理像素(0,0) = 窗口左上角(也就是物理坐标)  	鼠标事件、窗口大小都在这个次元
	*	2.渲染空间	  逻辑像素(0,0) = 画布左上角(也就是逻辑坐标)		精灵位置、UI布局、摄像机视口都在这个次元
	*	3.世界空间	  游戏单位自定义（米/格，也就是世界坐标）			玩家位置、碰撞体、AI、物理逻辑都在这个次元
	*
	* 屏幕空间就是你的游戏窗口，当需要访问鼠标时介入这一空间，通过物理坐标->逻辑坐标->世界坐标这样链式转换
	* 渲染空间就是Renderer的逻辑渲染大小（也就是开发时的默认大小，渲染器以这个为基准绘制整个游戏世界，并在屏幕空间自动缩放居中）
	* 世界空间就是真正的游戏世界，角色的坐标等属性在这里更新，必要时通过Camera与渲染空间坐标相互转化
	*/

	// 摄像机
	class Camera
	{
	public:
		// 跟随方式
		enum FollowStyle
		{
			None = 0,			// 无跟随
			Only_X = 1 << 0,	// 仅跟随X轴
			Only_Y = 1 << 1,	// 仅跟随Y轴
			Static = 1 << 2,	// 静态跟随
			Smooth = 1 << 3,	// 平滑跟随
		};

	public:
		Camera(const Vector2& pos) : base_position(pos) {}
		~Camera() = default;

		// 设置位置
		void set_position(const Vector2& pos) { base_position = pos; }

		// 获取位置
		const Vector2& get_position() const { return position; }

		// 设置缩放
		void set_zoom(float scale)
		{
			if (scale <= 0.0f) zoom = 0.01f; //	防止缩放过小导致的渲染问题
			else zoom = scale;
		}

		// 获取缩放
		float get_zoom() const { return zoom; }

		// 重置摄像机
		void reset()
		{
			position = base_position = shake_position = Vector2(0, 0);
			this->zoom = 1.0f;
			this->is_shaking = false;
		}

		// 抖动摄像机
		void shake(float strength, float decay)
		{
			this->is_shaking = true;
			this->shake_strength = strength;
			this->shake_decay = (decay <= 0.0f ? 0.1f : decay);	// 防止幅度不衰减或负衰减
		}

		// 跟随目标，支持多种跟随方式（静态/平滑，单轴/双轴）
		void look_at(const Vector2& target, int style, float smooth_factor = 0.05f)
		{
			if (style & FollowStyle::None) return;

			if (style & FollowStyle::Static)	// 静态跟随
			{
				if (style & FollowStyle::Only_X)
					base_position.x = target.x;
				else if (style & FollowStyle::Only_Y)
					base_position.y = target.y;
				else
					base_position = target;
			}
			else if (style & FollowStyle::Smooth)	// 平滑跟随
			{
				float _delta_smooth = 1 - std::powf(1 - smooth_factor, Clock::get_DeltaTime());	// 保持系数稳定
				if (style & FollowStyle::Only_X)
					base_position.x = maths::lerp(base_position.x, target.x, _delta_smooth);
				else if (style & FollowStyle::Only_Y)
					base_position.y = maths::lerp(base_position.y, target.y, _delta_smooth);
				else
				{
					base_position.x = maths::lerp(base_position.x, target.x, _delta_smooth);
					base_position.y = maths::lerp(base_position.y, target.y, _delta_smooth);
				}
			}
		}

		// 自定义跟随逻辑，支持回调函数
		void look_at(const Vector2& target, std::function<void(const Vector2&)> follow) { follow(target); }

		// 世界坐标->窗口坐标
		Vector2 world_to_screen(const Vector2& world_pos) const
		{
			int log_width, log_height;	// 逻辑窗口尺寸
			SDL_GetRenderLogicalPresentation(Main_Renderer, &log_width, &log_height, nullptr);

			// 窗口坐标 = 逻辑视野中心点 + (世界坐标 - 摄像机坐标) * 缩放因子
			float _screen_x = (log_width / 2.0f) + (world_pos.x - position.x) * zoom;
			float _screen_y = (log_height / 2.0f) + (world_pos.y - position.y) * zoom;
			return Vector2(_screen_x, _screen_y);

			/* PS：摄像机视野就是整个逻辑窗口的尺寸，而且SDL3的Renderer会自动缩放，自动居中，因此无需适配缩放比 */
		}

		// 窗口坐标->世界坐标
		Vector2 screen_to_world(const Vector2& screen_pos) const
		{
			int log_width, log_height;	// 逻辑窗口尺寸
			SDL_GetRenderLogicalPresentation(Main_Renderer, &log_width, &log_height, nullptr);

			// 世界坐标 = (窗口坐标 - 视野中心点) / 缩放因子 + 摄像机坐标
			float _world_x = (screen_pos.x - log_width / 2.0f) / zoom + position.x;
			float _world_y = (screen_pos.y - log_height / 2.0f) / zoom + position.y;
			return Vector2(_world_x, _world_y);
		}

		// 判断对象是否在视野内(传原始是世界属性就行，内部会自动变换)
		bool target_in_view(const Vector2& pos, const Size& size) const
		{
			int log_width, log_height;	// 逻辑窗口尺寸
			SDL_GetRenderLogicalPresentation(Main_Renderer, &log_width, &log_height, nullptr);

			// 世界属性变换
			Vector2 _screen_pos = this->world_to_screen(pos);
			Size _screen_size = { size.w * zoom, size.h * zoom };

			// 检查是否在视野内
			bool _in_range_x = (_screen_pos.x >= -_screen_size.w && _screen_pos.x <= log_width + _screen_size.w);
			bool _in_range_y = (_screen_pos.y >= -_screen_size.h && _screen_pos.y <= log_height + _screen_size.h);
			return (_in_range_x && _in_range_y);
		}

		// 更新摄像机状态
		void on_update(float delta)
		{
			if (is_shaking && shake_strength > 0.1f)	// 如果正在抖动且抖动幅度足够大
			{
				shake_position.x = (-50 + std::rand() % 100) / 50.0f * shake_strength;
				shake_position.y = (-50 + std::rand() % 100) / 50.0f * shake_strength;
				shake_strength *= 1.0f - shake_decay * delta;	// 衰减抖动幅度
			}
			else // 否则就重置抖动偏移
			{
				this->is_shaking = false;
				this->shake_position = Vector2(0, 0);
			}

			// 最终位置 = 基础位置 + 抖动位置
			position = base_position + shake_position;
		}

	private:
		Vector2 position;		// 最终位置
		Vector2 base_position;	// 基础位置
		Vector2 shake_position;	// 抖动位置

		bool is_shaking = false;		// 是否正在抖动
		float shake_strength = 0.0f;	// 抖动幅度
		float shake_decay = 0.0f;		// 抖动衰减程度

		float zoom = 1.0f;	// 缩放大小
	};

	// 渲染器
	class Renderer
	{
	public:
		// 填充背景色
		static void fill(SDL_Color color)
		{
			SDL_SetRenderDrawColor(Main_Renderer, color.r, color.g, color.b, color.a);
			SDL_RenderClear(Main_Renderer);
		}

		// 渲染纹理
		static void render_texture(const Camera& camera, SDL_Texture* tex, const SDL_FRect* rect_src,
			const SDL_FRect* rect_dst, double angle, const Vector2& anchor = Vector2(0, 0), bool is_flip = false)
		{
			// 视野剔除
			if (!camera.target_in_view({ rect_dst->x, rect_dst->y }, { rect_dst->w, rect_dst->h }))
				return;

			SDL_FRect _rect_dst_win = *rect_dst;

			// 纹理属性变换
			_rect_dst_win.x = camera.world_to_screen(Vector2(rect_dst->x, rect_dst->y)).x;
			_rect_dst_win.y = camera.world_to_screen(Vector2(rect_dst->x, rect_dst->y)).y;
			_rect_dst_win.w *= camera.get_zoom();
			_rect_dst_win.h *= camera.get_zoom();

			// 设置旋转中心
			SDL_FPoint _rotate_center = { _rect_dst_win.w * anchor.x, _rect_dst_win.h * anchor.y };

			// 设置渲染中心
			_rect_dst_win.x -= anchor.x * _rect_dst_win.w;
			_rect_dst_win.y -= anchor.y * _rect_dst_win.h;

			SDL_RenderTextureRotated(Main_Renderer, tex, rect_src, &_rect_dst_win, angle,
				&_rotate_center, (is_flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE));
		}

		// 渲染线段
		static void render_line(const Camera& camera, const Vector2& begin, const Vector2& end, SDL_Color color)
		{
			// 线段属性变换
			Vector2 _screen_begin = camera.world_to_screen(begin);
			Vector2 _screen_end = camera.world_to_screen(end);

			SDL_SetRenderDrawColor(Main_Renderer, color.r, color.g, color.b, color.a);
			SDL_RenderLine(Main_Renderer, _screen_begin.x, _screen_begin.y, _screen_end.x, _screen_end.y);
		}

		// 渲染矩形（以中心点为基准）
		static void render_rect_center(const Camera& camera, const Vector2& pos, const Size& size,
			SDL_Color color, bool is_filled = false)
		{
			// 视野剔除
			if (!camera.target_in_view(pos, size))
				return;

			SDL_SetRenderDrawColor(Main_Renderer, color.r, color.g, color.b, color.a);

			// 矩形属性变换
			Vector2 _corner_pos = { pos.x - size.w / 2.0f, pos.y - size.h / 2.0f };	// 以中心点为基准，计算左上角位置
			SDL_FRect _rect = {
				camera.world_to_screen(_corner_pos).x,
				camera.world_to_screen(_corner_pos).y,
				size.w * camera.get_zoom(),
				size.h * camera.get_zoom()
			};

			if (is_filled) SDL_RenderFillRect(Main_Renderer, &_rect);
			else SDL_RenderRect(Main_Renderer, &_rect);
		}

		// 渲染矩形（以左上角为基准）
		static void render_rect_corner(const Camera& camera, const Vector2& pos, const Size& size,
			SDL_Color color, bool is_filled = false)
		{
			// 视野剔除
			if (!camera.target_in_view(pos, size))
				return;

			SDL_SetRenderDrawColor(Main_Renderer, color.r, color.g, color.b, color.a);

			// 矩形属性变换
			SDL_FRect _rect = {
				camera.world_to_screen(pos).x,
				camera.world_to_screen(pos).y,
				size.w * camera.get_zoom(),
				size.h * camera.get_zoom()
			};

			if (is_filled) SDL_RenderFillRect(Main_Renderer, &_rect);
			else SDL_RenderRect(Main_Renderer, &_rect);
		}

		// 渲染圆形
		static void render_circle(const Camera& camera, const Vector2& center_pos, float radius,
			SDL_Color color, bool is_filled = false)
		{
			// 视野剔除
			if (!camera.target_in_view(center_pos, { radius, radius }))
				return;

			// 圆形属性变换
			Vector2 _screen_center = camera.world_to_screen(center_pos);
			float _screen_radius = radius * camera.get_zoom();

			if (is_filled) maths::render_filled_circle(_screen_center, _screen_radius, color);
			else maths::render_circle(_screen_center, _screen_radius, color);
		}

		// 渲染文字
		static void render_text(const Camera& camera, const Vector2& pos, Text* text, float ptsize, SDL_Color color)
		{
			TTF_SetFontSize(TTF_GetTextFont(text->get_SDLText()), ptsize * camera.get_zoom());
			TTF_SetTextColor(text->get_SDLText(), color.r, color.g, color.b, color.a);
			TTF_DrawRendererText(text->get_SDLText(), camera.world_to_screen(pos).x, camera.world_to_screen(pos).y);
		}
	};

	// ============================================= 基础组件 =============================================

	// 计时器
	class Timer
	{
	public:
		Timer() = default;
		~Timer() = default;

		// 重启计时器
		void restart() { pass_time = 0; shotted = false; }

		// 设置等待时间
		void set_wait_time(float val) { this->wait_time = val; }

		// 设置是否单次触发（默认false）
		void set_one_shot(bool flag) { this->one_shot = flag; }

		// 设置回调函数
		void set_on_timeout(std::function<void()> callback) { on_timeout = callback; }

		// 暂停
		void pause() { paused = true; }

		// 继续
		void resume() { paused = false; }

		// 更新计时器
		void on_update(float delta)
		{
			if (paused) return;

			pass_time += delta;
			if (pass_time >= wait_time)
			{
				bool _can_shot = (!one_shot || (one_shot && !shotted));
				shotted = true;

				if (_can_shot && on_timeout) this->on_timeout();
				pass_time -= wait_time;
			}
		}

	private:
		float pass_time = 0;				// 经过时间
		float wait_time = 0;				// 等待时间
		bool paused = false;				// 是否暂停
		bool shotted = false;				// 是否已经触发
		bool one_shot = false;				// 是否单次触发
		std::function<void()> on_timeout;	// 回调函数
	};

	// 碰撞箱
	class CollisionManager; // 前向声明
	class CollisionBox
	{
		friend class CollisionManager;
	public:
		// 设置是否启用碰撞（默认true）
		void set_enabled(bool flag) { this->enabled = flag; }

		// 设置自身碰撞层
		void set_layer_src(CollisionLayer layer) { this->layer_src = layer; }

		// 设置目标碰撞层
		void set_layer_dst(CollisionLayer layer) { this->layer_dst = layer; }

		// 设置碰撞回调函数
		void set_on_collide(std::function<void(const CollideCtx&)> func) { this->on_collide = func; }

		// 设置发送属性回调函数
		void set_send_props(std::function<void(CollideCtx&)> func) { this->send_props = func; }

		// 设置碰撞箱坐标
		void set_position(const Vector2& pos) { this->position = pos; }

		// 设置碰撞箱大小
		void set_size(const Size& size) { this->size = size; }

		// 设置碰撞箱方向
		void set_direction(float dir) { this->direction = dir; }

		// 获取碰撞箱大小
		const Size& get_size() const { return this->size; }

		// 获取碰撞箱坐标
		const Vector2& get_position() const { return this->position; }

		// 获取碰撞箱方向
		const float get_direction() const { return this->direction; }

	private:
		Size size = { 0, 0 };	// 碰撞箱大小
		Vector2 position;		// 碰撞箱坐标
		float direction = 0.0f;	// 碰撞箱方向

		bool enabled = true;	// 是否启用碰撞检测
		CollisionLayer layer_src = CollisionLayer::None;	// 自身碰撞层
		CollisionLayer layer_dst = CollisionLayer::None;	// 目标碰撞层

		std::function<void(const CollideCtx&)> on_collide;	// 碰撞回调函数
		std::function<void(CollideCtx&)> send_props;		// 发送属性信息
		std::array<Vector2, 4> corners;		// 矩形碰撞箱四个顶点

	private:
		CollisionBox() = default;
		~CollisionBox() = default;

		// 计算碰撞箱的四个顶点坐标
		void set_corners()
		{
			float half_w = size.w / 2.0f, half_h = size.h / 2.0f;
			float radian = maths::deg_to_rad(direction);

			this->corners[0] = position + Vector2(-half_w, -half_h).rotate(radian);
			this->corners[1] = position + Vector2(half_w, -half_h).rotate(radian);
			this->corners[2] = position + Vector2(half_w, half_h).rotate(radian);
			this->corners[3] = position + Vector2(-half_w, half_h).rotate(radian);
		}
	};

	// 动画
	class Animation
	{
	public:
		Animation()
		{
			timer.set_wait_time(0.1f);
			timer.set_on_timeout([&]() -> void
				{
					idx_frame++;
					if (idx_frame >= frame_list.size())
					{
						idx_frame = (is_loop ? 0 : frame_list.size() - 1);
						if (!is_loop && on_finished)
							on_finished();
					}
				});
		}

		Animation(SDL_Texture* texture, int num_w, int num_h, int h_idx) 
			: Animation() { this->add_frame(texture, num_w, num_h, h_idx); }

		Animation(Atlas* atlas) : Animation() { this->add_frame(atlas); }

		// 重置动画
		void reset()
		{
			this->timer.restart();
			this->idx_frame = 0;
		}

		// 设置循环(默认为true)
		void set_loop(bool flag) { is_loop = flag; }

		// 设置动画间隔(默认0.1f)
		void set_interval(float dur) { timer.set_wait_time(dur); }

		// 设置回调函数
		void set_on_finished(std::function<void()> callback) { on_finished = callback; }

		// 从长图中添加序列帧
		void add_frame(SDL_Texture* texture, int num_w, int num_h, int h_idx)
		{
			float _tex_width, _tex_height;
			SDL_GetTextureSize(texture, &_tex_width, &_tex_height);	// 获取序列帧宽高

			float _width_frame = _tex_width / num_w;	// 获取单张序列帧宽
			float _height_frame = _tex_height / num_h;	// 获取单张序列帧高
			for (int i = 0; i < num_w; i++)
			{
				SDL_FRect _rect_src{};
				_rect_src.x = i * _width_frame, _rect_src.y = (h_idx - 1) * _height_frame;
				_rect_src.w = _width_frame, _rect_src.h = _height_frame;

				frame_list.emplace_back(texture, _rect_src); // 将序列帧加入列表
			}
		}

		// 从图集中添加序列帧
		void add_frame(Atlas* atlas)
		{
			for (int i = 0; i < atlas->get_size(); i++)
			{
				SDL_Texture* _texture = atlas->get_texture(i);

				float _width, _height;
				SDL_GetTextureSize(_texture, &_width, &_height);
				SDL_FRect _rect_src = { 0, 0, _width, _height };

				frame_list.emplace_back(_texture, _rect_src);
			}
		}

		// 更新
		void on_update(float delta) { timer.on_update(delta); }

		// 渲染
		void on_render(const Camera& camera, const Property& prop)
		{
			const Frame& _frame = frame_list[idx_frame];
			SDL_SetTextureAlphaMod(_frame.texture, (Uint8)(255.0f * (prop.alpha / 100.0f)));
			Renderer::render_texture(camera, _frame.texture, &_frame.rect_src, &prop.get_SDLRect(), prop.direction, prop.anchor, prop.is_flip);
		}

	private:
		// 序列帧结构体
		struct Frame
		{
			SDL_FRect rect_src;
			SDL_Texture* texture = nullptr;

			Frame() = default;
			Frame(SDL_Texture* texture, const SDL_FRect& rect_src)
				: texture(texture), rect_src(rect_src) {}

			~Frame() = default;
		};

	private:
		Timer timer;						// 帧计时器
		bool is_loop = true;				// 是否循环
		size_t idx_frame = 0;				// 当前帧索引
		std::vector<Frame> frame_list;		// 帧列表
		std::function<void()> on_finished;	// 播放结束回调
	};

	// 动画播放器
	class AnimationPlayer
	{
		using animation_t = std::unique_ptr<Animation>;
	public:
		AnimationPlayer() = default;
		~AnimationPlayer() { this->clear_animation(); }

		// 添加动画
		template <typename... Args>
		Animation* create_animation(const std::string& name, Args&&... args)
		{
			if (animation_pool.find(name) != animation_pool.end())	// 检查动画已存在
			{
				std::string _info = "[add_animation()]: Animation \"" + name + "\"is already exist!";
				throw custom_error("Animation Player Error", _info.c_str());
			}
			animation_pool[name] = std::make_unique<Animation>(std::forward<Args>(args)...);
			return animation_pool[name].get();
		}

		// 切换动画
		void switch_to(const std::string& name)
		{
			if (animation_pool.find(name) == animation_pool.end())
			{
				std::string _info = "[switch_to()]: Animation \"" + name + "\" is not exist!";
				throw custom_error("Animation Player Error", _info.c_str());
			}
			this->current_animation = animation_pool[name].get();
		}

		// 设置当前动画，并从头播放
		void set_animation(const std::string& name)
		{
			if (animation_pool.find(name) == animation_pool.end())
			{
				std::string _info = "[set_animation()]: Animation \"" + name + "\" is not exist!";
				throw custom_error("Animation Player Error", _info.c_str());
			}

			this->current_animation = animation_pool[name].get();
			this->current_animation->reset();
		}

		// 获取当前动画
		Animation* get_current()
		{
			if (!current_animation)
				throw custom_error("Animation Player Error", "[get_current()]: \"current_animation\" is a nullptr!");
			return current_animation;
		}

		// 更新动画
		void on_update(float delta)
		{
			if (!current_animation) return;
			current_animation->on_update(delta);
		}

		// 渲染动画
		void on_render(const Camera& camera, const Property& prop)
		{
			if (!current_animation) return;
			current_animation->on_render(camera, prop);
		}

		// 清空动画池
		void clear_animation()
		{
			if (animation_pool.empty())
				return;

			// 释放动画资源
			for (auto& [name, animation] : animation_pool)
				animation.reset();

			animation_pool.clear();
			current_animation = nullptr;
		}

	private:
		std::unordered_map<std::string, animation_t> animation_pool;	// 动画池
		Animation* current_animation = nullptr;		// 当前动画
	};

	// 状态节点
	class StateNode
	{
	public:
		StateNode() = default;
		virtual ~StateNode() = default;

		virtual void on_enter() {}				// 进入状态
		virtual void on_update(float delta) {}	// 更新状态
		virtual void on_exit() {}				// 退出状态
	};

	// 状态机
	class StateMachine
	{
		using state_t = std::unique_ptr<StateNode>;
	public:
		StateMachine() = default;
		~StateMachine() { this->clear_state_node(); }

		// 更新状态机
		void on_update(float delta)
		{
			if (!current_state)
			{
				std::string _info = "[on_update()]: \"current_state\" is nullptr! Please check the initialization";
				throw custom_error("State Machine Error", _info.c_str());
			}

			if (need_init)	// 如果需要初始化状态机
			{
				current_state->on_enter();
				need_init = false;		// 初始化完成
			}

			current_state->on_update(delta);
		}

		// 设置初状态节点（用于初始化）
		void set_entry(const std::string& name)
		{
			// 如果未找到目标状态
			if (state_pool.find(name) == state_pool.end())
			{
				std::string _info = "[set_entry()]: State node \"" + name + "\" is not found!";
				throw custom_error("State Machine Error", _info.c_str());
			}
			current_state = state_pool[name].get();
		}

		// 切换状态节点
		void switch_to(const std::string& name)
		{
			// 如果未找到目标状态
			if (state_pool.find(name) == state_pool.end())
			{
				std::string _info = "[switch_to()]: State node \"" + name + "\" is not found!";
				throw custom_error("State Machine Error", _info.c_str());
			}

			if (current_state) current_state->on_exit();	// 退出
			current_state = state_pool[name].get();			// 切换
			current_state->on_enter();						// 进入
		}

		// 添加状态节点
		StateNode* create_state_node(const std::string& name)
		{
			if (state_pool.find(name) != state_pool.end())	// 如果该状态已存在
			{
				std::string _info = "[add_state_node()]: State node \"" + name + "\" is already exist!";
				throw custom_error("State Machine Error", _info.c_str());
			}
			state_pool[name] = std::make_unique<StateNode>();
			return state_pool[name].get();
		}

		// 清空状态池
		void clear_state_node()
		{
			if (state_pool.empty())
				return;

			// 释放状态池内所有状态节点
			for (auto& [name, state] : state_pool)
				state.reset();

			state_pool.clear();
			current_state = nullptr;
		}

	private:
		bool need_init = true;	// 是否需要初始化状态机
		StateNode* current_state = nullptr;			// 当前状态节点
		std::unordered_map<std::string, state_t> state_pool; // 状态池
	};

	// ============================================= 游戏元素 =============================================

	// 精灵
	class Sprite
	{
	public:
		Sprite() = default;
		virtual ~Sprite() = default;

		virtual void on_update(float delta) {};				// 更新
		virtual void on_render(const Camera& camera) {};	// 渲染
		virtual void on_input(const SDL_Event& event) {};	// 输入
		virtual void on_ready() {};							// 重置

		Property& get_property() { return this->property; }
		const Property& get_property() const { return this->property; }

		// 面向鼠标指针
		void point_to_mouse(const Camera& camera)
		{
			Vector2 _window_mouse_pos, _logical_mouse_pos;
			SDL_GetMouseState(&_window_mouse_pos.x, &_window_mouse_pos.y);	// 获取鼠标物理窗口坐标
			SDL_RenderCoordinatesFromWindow(Main_Renderer, _window_mouse_pos.x, _window_mouse_pos.y, &_logical_mouse_pos.x, &_logical_mouse_pos.y); // 获取鼠标逻辑窗口坐标

			Vector2 _world_mouse_pos = camera.screen_to_world(_logical_mouse_pos); // 逻辑窗口坐标转世界坐标
			Vector2 dist = _world_mouse_pos - property.position;

			property.direction = maths::rad_to_deg(std::atan2f(dist.y, dist.x));
		}

		// 面向目标点
		void point_to(const Vector2& target)
		{
			Vector2 dist = target - property.position;
			property.direction = maths::rad_to_deg(std::atan2f(dist.y, dist.x));
		}

	protected:
		Property property;
	};

	// 平面角色模板
	class FlatBody : public Sprite
	{
	public:
		FlatBody() = default;
		~FlatBody() override = default;

		void set_velocity(const Vector2& vel) { this->velocity = vel; }
		void set_speed(float val) { this->speed = val; }
		void set_move_dir(const Vector2& dir) { this->move_dir = dir; }
		void enabled_arrow(bool flag) { this->is_arrow_ctrl = flag; }
		void enabled_wasd(bool flag) { this->is_wasd_ctrl = flag; }

		const Vector2& get_velocity() const { return velocity; }
		const float get_speed() const { return speed; }
		const Vector2& get_move_dir() const { return move_dir; }
		const bool get_arrow_ctrl() const { return is_arrow_ctrl; }
		const bool get_wasd_ctrl() const { return is_wasd_ctrl; }

		void on_ready() override
		{
			velocity = Vector2(0.0f, 0.0f);
			flag_y.first = flag_y.second = flag_x.first = flag_x.second = false;
		}

		void on_update(float delta) override
		{
			this->move_dir = { (flag_x.second - flag_x.first) * 1.0f, (flag_y.second - flag_y.first) * 1.0f };
			this->velocity = move_dir.normalize() * speed;
			property.position += velocity * delta;
		}

		void on_input(const SDL_Event& event) override
		{
			if (event.type == SDL_EVENT_KEY_DOWN)
			{
				if (is_arrow_ctrl) // 启用方向键控制
				{
					if (event.key.key == SDLK_UP) flag_y.first = true;
					if (event.key.key == SDLK_DOWN) flag_y.second = true;
					if (event.key.key == SDLK_LEFT) flag_x.first = true;
					if (event.key.key == SDLK_RIGHT) flag_x.second = true;
				}
				if (is_wasd_ctrl)	// 启用WASD控制
				{
					if (event.key.key == SDLK_W) flag_y.first = true;
					if (event.key.key == SDLK_S) flag_y.second = true;
					if (event.key.key == SDLK_A) flag_x.first = true;
					if (event.key.key == SDLK_D) flag_x.second = true;
				}
			}
			if (event.type == SDL_EVENT_KEY_UP)
			{
				if (is_arrow_ctrl)
				{
					if (event.key.key == SDLK_UP) flag_y.first = false;
					if (event.key.key == SDLK_DOWN) flag_y.second = false;
					if (event.key.key == SDLK_LEFT) flag_x.first = false;
					if (event.key.key == SDLK_RIGHT) flag_x.second = false;
				}
				if (is_wasd_ctrl)
				{
					if (event.key.key == SDLK_W) flag_y.first = false;
					if (event.key.key == SDLK_S) flag_y.second = false;
					if (event.key.key == SDLK_A) flag_x.first = false;
					if (event.key.key == SDLK_D) flag_x.second = false;
				}
			}
		}

	private:
		std::pair<bool, bool> flag_y = { false, false }; // 上 下
		std::pair<bool, bool> flag_x = { false, false }; // 左 右

		Vector2 velocity;				// 速度
		Vector2 move_dir = { 0, 0 };	// 方向向量
		float speed = 200.0f;			// 移动速度大小

		bool is_arrow_ctrl = true;  // 开启方向键控制移动
		bool is_wasd_ctrl = true;	// 开启WASD控制移动
	};

	// 精灵组
	class Scene; // 前向声明
	class SpriteGroup
	{
		friend class Scene;
	public:
		SpriteGroup() = default;
		~SpriteGroup() = default;

		// 对精灵进行批量处理
		void range(std::function<void(std::shared_ptr<Sprite>)> process_callback)
		{
			for (std::shared_ptr<Sprite> sprite : list)
			{
				if (!sprite.use_count()) continue;  // 无引用计数，跳过
				if (sprite->get_property().can_remove) continue; // 已被标记删除，跳过
				process_callback(sprite);
			}
		}

	private:
		// 添加精灵
		void add_sprite(std::shared_ptr<Sprite> sprite) { list.push_back(sprite); }

		// 移除精灵
		void remove_sprite(std::shared_ptr<Sprite> sprite) { list.erase(std::remove(list.begin(), list.end(), sprite), list.end()); }

	private:
		std::vector<std::shared_ptr<Sprite>> list;
	};

	// 场景
	class Scene
	{
	public:
		Scene() = default;
		virtual ~Scene() { this->clear_sprite(); }

		// 进入场景回调（用于属性的初始化）
		virtual void on_enter() 
		{
			for (std::shared_ptr<Sprite> sprite : sprite_list)
			{
				if (!sprite.use_count()) continue;	// 无引用计数，跳过
				if (sprite->get_property().can_remove) continue; // 已被标记删除，跳过
				sprite->on_ready();
			}
		}

		// 处理输入
		virtual void on_input(const SDL_Event& event) 
		{
			for (std::shared_ptr<Sprite> sprite : sprite_list)
			{
				if (!sprite.use_count()) continue;	// 无引用计数，跳过
				if (sprite->get_property().can_remove) continue; // 已被标记删除，跳过
				sprite->on_input(event);
			}
		}

		// 处理数据
		virtual void on_update(float delta) 
		{
			// 对精灵排序
			std::sort(sprite_list.begin(), sprite_list.end(), [](auto a, auto b) -> bool
				{
					std::pair<RenderLayer, RenderLayer> _layer = { a->get_property().layer, b->get_property().layer };
					std::pair<Vector2, Vector2> _pos = { a->get_property().position, b->get_property().position };

					// 让精灵先按照渲染层，再按照Y轴排序
					if (_layer.first == _layer.second) return _pos.first.y < _pos.second.y;
					else return _layer.first < _layer.second;
				});
			
			// 更新精灵
			for (std::shared_ptr<Sprite> sprite : sprite_list)
			{
				if (!sprite.use_count()) continue;	// 无引用计数，跳过
				if (sprite->get_property().can_remove) continue; // 已被标记删除，跳过
				sprite->on_update(delta);
			}

			// 延迟删除已标记的精灵
			for (std::shared_ptr<Sprite> sprite : sprite_list)
			{
				if (!sprite.use_count()) continue;	// 无引用计数，跳过

				const Property& prop = sprite->get_property();
				if (!prop.can_remove) continue;	// 未被标记删除，跳过

				if (prop.tag != "") sprite_registry.erase(prop.tag); // 从注册表中删除
				if (prop.group_tag != "") group_list[prop.group_tag].remove_sprite(sprite); // 从所属组中删除

				sprite_list.erase(std::remove(sprite_list.begin(), sprite_list.end(), sprite), sprite_list.end());
				sprite.reset();
			}
		}

		// 更新画面
		virtual void on_render(const Camera& cam_game, const Camera& cam_ui) 
		{
			// 渲染精灵
			for (std::shared_ptr<Sprite> sprite : sprite_list)
			{
				if (!sprite.use_count()) continue;	// 无引用计数，跳过

				const Property& prop = sprite->get_property();
				if (prop.can_remove) continue;	// 已被标记删除，跳过

				if (prop.layer == RenderLayer::UI)
					sprite->on_render(cam_ui);
				else if (cam_game.target_in_view(prop.position, prop.size))
					sprite->on_render(cam_game);
			}
		}

		// 退出场景回调（用于某些属性的释放）
		virtual void on_exit() {}

		// 添加精灵
		void add_sprite(std::shared_ptr<Sprite> sprite)
		{
			const Property& prop = sprite->get_property();

			// 既没有组名也没有标签的精灵是非法的
			if (prop.tag == "" && prop.group_tag == "")
			{
				std::string _info = "[add_sprite()]: Sprites that provide neither a unique tag nor a group name are illegal";
				throw custom_error("Scene Error", _info.c_str());
			}

			if (prop.tag != "") // 提供了唯一标签
			{
				// 验证唯一性
				if (sprite_registry.find(prop.tag) != sprite_registry.end())
				{
					std::string _info = std::format("[add_sprite()]: Sprite \"{}\" is already exist!", prop.tag);
					throw custom_error("Scene Tree Error", _info.c_str());
				}
				sprite_registry[prop.tag] = sprite;
			}
			if (prop.group_tag != "") // 提供了group_name
			{
				SpriteGroup& _new_group = group_list[prop.group_tag];
				_new_group.add_sprite(sprite);
			}

			sprite_list.push_back(sprite);
		}

		// 移除精灵
		void remove_spirte(std::shared_ptr<Sprite> sprite)
		{
			if (!sprite || !sprite.use_count()) return;	// 无效的内存，返回
			sprite->get_property().can_remove = true;
		}

		// 查找拥有唯一标签的精灵（支持子类返回）
		template <typename T = Sprite>
		T* find_sprite(const std::string& name)
		{
			auto it = sprite_registry.find(name);

			if (it == sprite_registry.end())
			{
				std::string _info = std::format("[find_sprite()]: Sprite \"{}\" is not found!", name);
				throw custom_error("Scene Tree Error", _info.c_str());
			}
			else
			{
				const Property& prop = it->second->get_property();
				if (prop.can_remove) return nullptr;  // 标记为已删除的精灵返回空指针
			}

			T* _result = dynamic_cast<T*>(it->second.get());
			if (_result == nullptr)
			{
				std::string _info = std::format("<{}> --> <{}>", typeid(Sprite*).name(), typeid(T*).name());
				throw custom_error("Scene Tree Error", "[find_sprite()]: Illegal polymorphic type conversion!\n" + _info);
			}

			return _result;
		}

		// 查找精灵组
		SpriteGroup& find_group(const std::string& name) { return group_list[name]; }

		// 清空精灵列表
		void clear_sprite()
		{
			for (std::shared_ptr<Sprite> sprite : sprite_list) sprite.reset();
			this->sprite_list.clear();
			this->sprite_registry.clear();
			this->group_list.clear();
		}

	private:
		std::vector<std::shared_ptr<Sprite>> sprite_list;
		std::unordered_map<std::string, std::shared_ptr<Sprite>> sprite_registry;
		std::unordered_map<std::string, SpriteGroup> group_list;
	};

	// ============================================= 管理器类 =============================================

	// 资源管理器
	class ResourcesManager
	{
	public:
		// 获取资源管理器单例
		static ResourcesManager* instance()
		{
			if (!m_instance) m_instance = new ResourcesManager();
			return m_instance;
		}

		// 加载资源
		void load_resources(const char* directory)
		{
			using namespace std::filesystem;

			if (!exists(directory))	// 判断文件是否存在
			{
				std::string _info = "[load_resources()]: Dictionary \"" + std::string(directory) + "\" is an error dictionary!";
				throw custom_error("ResourcesManager Error", _info.c_str());
			}

			// 遍历目标文件内部所有文件
			for (const auto& _entry : recursive_directory_iterator(directory))
			{
				if (_entry.is_regular_file())	// 如果是有效文件
				{
					const auto& _path = _entry.path();
					if (_path.extension() == ".bmp" || _path.extension() == ".png" || _path.extension() == ".jpg")
					{
						SDL_Texture* _texture = IMG_LoadTexture(Main_Renderer, _path.string().c_str());
						SDL_SetTextureScaleMode(_texture, scale_mode);	// 设置缩放模式
						texture_pool[_path.stem().string()] = _texture;
					}
					if (_path.extension() == ".wav")
					{
						Mix_Chunk* _audio = Mix_LoadWAV(_path.string().c_str());
						audio_pool[_path.stem().string()] = _audio;
					}
					if (_path.extension() == ".mp3" || _path.extension() == ".ogg")
					{
						Mix_Music* _music = Mix_LoadMUS(_path.string().c_str());
						music_pool[_path.stem().string()] = _music;
					}
					if (_path.extension() == ".ttf" || _path.extension() == ".TTF" || _path.extension() == ".ttc")
					{
						TTF_Font* _font = TTF_OpenFont(_path.string().c_str(), 24);	// 默认24号
						font_pool[_path.stem().string()] = _font;
					}
				}
			}
		}

		// 查找纹理资源
		SDL_Texture* find_texture(const std::string& name)
		{
			if (texture_pool.find(name) == texture_pool.end())
			{
				std::string _info = "[find_texture()]: Texture \"" + name + "\" is not found!";
				throw custom_error("ResourcesManager Error", _info.c_str());
			}
			return texture_pool[name];
		}

		// 查找音频资源
		Mix_Chunk* find_audio(const std::string& name)
		{
			if (audio_pool.find(name) == audio_pool.end())
			{
				std::string _info = "[find_audio()]: Audio \"" + name + "\" is not found!";
				throw custom_error("ResourcesManager Error", _info.c_str());
			}
			return audio_pool[name];
		}

		// 查找音乐资源
		Mix_Music* find_music(const std::string& name)
		{
			if (music_pool.find(name) == music_pool.end())
			{
				std::string _info = "[find_music()]: Music \"" + name + "\" is not found!";
				throw custom_error("ResourcesManager Error", _info.c_str());
			}
			return music_pool[name];
		}

		// 查找字体资源
		TTF_Font* find_font(const std::string& name)
		{
			if (font_pool.find(name) == font_pool.end())
			{
				std::string _info = "[find_font()]: Font \"" + name + "\" is not found!";
				throw custom_error("ResourcesManager Error", _info.c_str());
			}
			return font_pool[name];
		}

		// 显示已加载的资源
		void debug_resources_list()
		{
			printf("================================\n");
			printf("已加载的纹理资源如下 (共%d个):\n", (int)texture_pool.size());
			for (auto& _texture : texture_pool)
				printf("%s\n", _texture.first.c_str());

			printf("\n已加载的音频资源如下 (共%d个):\n", (int)audio_pool.size() + (int)music_pool.size());
			for (auto& _audio : audio_pool)
				printf("%s\n", _audio.first.c_str());
			for (auto& _music : music_pool)
				printf("%s\n", _music.first.c_str());

			printf("\n已加载的字体资源如下 (共%d个):\n", (int)font_pool.size());
			for (auto& _font : font_pool)
				printf("%s\n", _font.first.c_str());
			printf("================================\n");
		}

	private:
		ResourcesManager() = default;
		~ResourcesManager() = default;

	private:
		static ResourcesManager* m_instance;	// 资源管理器单例
		std::unordered_map<std::string, SDL_Texture*> texture_pool;	// 纹理资源池
		std::unordered_map<std::string, Mix_Chunk*> audio_pool;		// 音频资源池
		std::unordered_map<std::string, Mix_Music*> music_pool;		// 音乐资源池
		std::unordered_map<std::string, TTF_Font*> font_pool;		// 字体资源池
	};
	ResourcesManager* ResourcesManager::m_instance = nullptr;

	// 碰撞管理器
	class CollisionManager
	{
	public:
		// 获取碰撞管理器单例
		static CollisionManager* instance()
		{
			if (!m_instance) m_instance = new CollisionManager();
			return m_instance;
		}

		// 创建碰撞箱
		CollisionBox* create_collision_box()
		{
			CollisionBox* _new_box = new CollisionBox();
			collision_box_list.push_back(_new_box);
			return _new_box;
		}

		// 销毁碰撞箱
		void destroy_collision_box(CollisionBox* collision_box)
		{
			auto target_it = std::remove(collision_box_list.begin(), collision_box_list.end(), collision_box);
			collision_box_list.erase(target_it, collision_box_list.end());
			delete collision_box;
		}

		// 处理碰撞检测
		void process_collision()
		{
			for (CollisionBox* box_src : collision_box_list)
			{
				// 源碰撞箱未启用 or 没有目标碰撞层
				if (!box_src->enabled || box_src->layer_dst == CollisionLayer::None)
					continue;

				for (CollisionBox* box_dst : collision_box_list)
				{
					// 目标碰撞箱未启用 or 是源碰撞箱 or 该碰撞层不是源碰撞箱要检测的
					if (!box_dst->enabled || box_src == box_dst || !(box_src->layer_dst & box_dst->layer_src))
						continue;

					Vector2 normal;	// 分离轴
					float depth;	// 碰撞深度

					box_src->set_corners();	// 设置源碰撞箱的四个角
					box_dst->set_corners(); // 设置目标碰撞箱的四个角
					bool is_collide = SAT_collision(box_src, box_dst, normal, depth);

					CollideCtx ctx;
					ctx.normal = normal, ctx.depth = depth, ctx.other_layer = box_src->layer_src;

					if (is_collide && box_dst->on_collide)
					{
						if (box_src->send_props) box_src->send_props(ctx);
						box_dst->on_collide(ctx);
					}
				}
			}
		}

		// 调试碰撞信息
		void debug_collision_box(const Camera& camera)
		{
			for (CollisionBox* box : collision_box_list)
			{
				SDL_Color _color;

				// 设置颜色信息
				if (box->enabled) _color = { 255,195,195,255 };
				else _color = { 115,155,175,255 };

				box->set_corners();
				Renderer::render_line(camera, box->corners[0], box->corners[1], _color);
				Renderer::render_line(camera, box->corners[1], box->corners[2], _color);
				Renderer::render_line(camera, box->corners[2], box->corners[3], _color);
				Renderer::render_line(camera, box->corners[3], box->corners[0], _color);
			}
		}

	private:
		CollisionManager() = default;
		~CollisionManager() = default;

		// SAT分离轴碰撞检测
		bool SAT_collision(CollisionBox* src, CollisionBox* dst, Vector2& out_normal, float& out_depth)
		{
			/* 以下是2-SAT的碰撞检测算法，原理为如果两个矩形在任意轴上的投影都有重合部分，说明这两个矩形相交了
			   具体逻辑为分别计算两个碰撞箱(矩形)的长和宽，并获取其法向量作为候选分离轴，判断两矩形的投影是否在四条轴上都相交 */

			   // 建立四条候选轴
			axes[0] = (src->corners[1] - src->corners[0]).get_normal();
			axes[1] = (src->corners[3] - src->corners[0]).get_normal();
			axes[2] = (dst->corners[1] - dst->corners[0]).get_normal();
			axes[3] = (dst->corners[3] - dst->corners[0]).get_normal();

			float min_overlap = INFINITY;
			Vector2 best_normal;

			// 对于每个轴进行投影计算
			for (Vector2 axis : axes)
			{
				// 计算源碰撞箱的投影
				float min_src = INFINITY, max_src = -INFINITY;
				for (Vector2 corner : src->corners)
				{
					float p = corner * axis; // 矩形的顶点在当前预选轴上的投影长度
					min_src = std::min(min_src, p);
					max_src = std::max(max_src, p);
				}

				// 计算目标碰撞箱的投影
				float min_dst = INFINITY, max_dst = -INFINITY;
				for (Vector2 corner : dst->corners)
				{
					float p = corner * axis; // 矩形的顶点在当前预选轴上的投影长度
					min_dst = std::min(min_dst, p);
					max_dst = std::max(max_dst, p);
				}

				// 在这条轴上没有重叠就是没有发生碰撞
				if (max_src < min_dst || max_dst < min_src) return false;

				// 计算重叠量，并更新最小重叠量与最佳分离轴
				float overlap = std::min(max_src, max_dst) - std::max(min_src, min_dst);
				if (overlap < min_overlap)
				{
					min_overlap = overlap;
					best_normal = axis;
				}
			}

			Vector2 diff = src->position - dst->position; // 获得一个指向源碰撞箱的向量
			if (diff * best_normal < 0) best_normal = -best_normal;	// 翻转，确保碰撞法线指向源碰撞箱

			out_normal = best_normal;
			out_depth = min_overlap;
			return true;	// 每个方向上的轴都有重合，说明碰上了
		}

	private:
		static CollisionManager* m_instance;	// 碰撞管理器单例
		std::vector<CollisionBox*> collision_box_list;	// 碰撞箱列表（以后可能支持四叉树）
		std::array<Vector2, 4> axes;	// 碰撞的四条候选轴
	};
	CollisionManager* CollisionManager::m_instance = nullptr;

	// 场景管理器
	class SceneManager
	{
	public:
		// 获取场景管理器单例
		static SceneManager* instance()
		{
			if (!m_instance) m_instance = new SceneManager();
			return m_instance;
		}

		// 设置当前场景
		void set_current_scene(const std::string& name)
		{
			if (scene_pool.find(name) == scene_pool.end())
			{
				std::string _info = "[set_current_scene()]: Scene \"" + name + "\" is not exist!";
				throw custom_error("Scene Manager Error", _info.c_str());
			}

			current_scene = scene_pool[name];
			current_scene->on_enter();
		}

		// 切换场景(预处理)
		void switch_to(const std::string& name)
		{
			if (scene_pool.find(name) == scene_pool.end())
			{
				std::string _info = "[switch_to()]: Scene \"" + name + "\" is not exist!";
				throw custom_error("Scene Manager Error", _info.c_str());
			}

			current_scene->on_exit();
			current_scene = scene_pool[name];
			current_scene->on_enter();
		}

		// 添加场景
		void add_scene(const std::string& name, Scene* scene)
		{
			if (scene_pool.find(name) != scene_pool.end())
			{
				std::string _info = "[add_scene()]: Scene \"" + name + "\" is already exist!";
				throw custom_error("SceneManager Error", _info.c_str());
			}
			scene_pool[name] = scene;
		}

		// 查找场景
		template <typename T = Scene>
		T* find_scene(const std::string& name)
		{
			if (scene_pool.find(name) == scene_pool.end())
			{
				std::string _info = "[find_scene()]: Scene \"" + name + "\" is not exist!";
				throw custom_error("Scene Manager Error", _info.c_str());
			}

			T* _result = dynamic_cast<T*>(scene_pool[name]);
			if (_result == nullptr)
			{
				std::string _info = std::format("<{}> --> <{}>", typeid(Scene*).name(), typeid(T*).name());
				throw custom_error("Scene Manager Error", "[find_scene()]: Illegal polymorphic type conversion!\n" + _info);
			}

			return _result;
		}

		// 处理输入
		void on_input(const SDL_Event& event)
		{
			if (current_scene)
				current_scene->on_input(event);
		}

		// 处理数据
		void on_update(float delta)
		{
			if (current_scene)
				current_scene->on_update(delta);
		}

		// 更新画面
		void on_render(const Camera& cam_game, const Camera& cam_ui)
		{
			if (current_scene)
				current_scene->on_render(cam_game, cam_ui);
		}

	private:
		SceneManager() = default;
		~SceneManager() = default;

	private:
		static SceneManager* m_instance;	// 场景管理器单例
		Scene* current_scene = nullptr;		// 当前场景
		bool is_switch_pending = false;		// 是否切换场景，用于延迟逻辑
		std::unordered_map<std::string, Scene*> scene_pool;	// 场景池
	};
	SceneManager* SceneManager::m_instance = nullptr;

	// ============================================= 主控 =============================================

	// 游戏循环主控
	class Game
	{
	public:
		static Camera* Game_Camera;
		static Camera* UI_Camera;

	public:
		Game() = default;
		~Game() = default;

		// 初始化
		static void Init_Graphic(const char* title, const Size& size, const Size& normal_size = { 1280, 720 }, WindowFlags flag = WindowFlags::Default)
		{
			try
			{
				// 初始化SDL相关模块
				SDL_Init(SDL_INIT_EVERYTHING);
				TTF_Init();

				SDL_AudioSpec spec = { SDL_AUDIO_S32, 2, 44100 };
				Mix_Init(MIX_INIT_EVERYTHING);
				Mix_OpenAudio(NULL, &spec);

				// 创建主窗口
				Main_Window = SDL_CreateWindow(title, (int)size.w, (int)size.h, (SDL_WindowFlags)flag);
				if (!Main_Window) throw std::runtime_error("Failed to create Main_Window!");

				// 创建主渲染器
				Main_Renderer = SDL_CreateRenderer(Main_Window, nullptr);
				if (!Main_Renderer) throw std::runtime_error("Failed to create Main_Renderer!");

				// 创建主文字引擎
				Main_TextEngine = TTF_CreateRendererTextEngine(Main_Renderer);
				if (!Main_TextEngine) throw std::runtime_error("Failed to create Main_TextEngine!");

				// 初始化摄像机
				Game_Camera = new Camera({ 0, 0 });
				UI_Camera = new Camera({ normal_size.w / 2.0f, normal_size.h / 2.0f });

				// 设置渲染器的逻辑渲染尺寸
				int w = static_cast<int>(normal_size.w), h = static_cast<int>(normal_size.h);
				SDL_SetRenderLogicalPresentation(Main_Renderer, w, h, SDL_LOGICAL_PRESENTATION_LETTERBOX);
			}
			catch (const std::exception& e)
			{
				utils::Show_MessageBox(MsgBoxFlags::Error, "FlexCore Error", e.what());
				Release_Graphic();
				exit(EXIT_FAILURE);
			}
			catch (const custom_error& e)
			{
				utils::Show_MessageBox(MsgBoxFlags::Error, e.title(), e.what());
				Release_Graphic();
				exit(EXIT_FAILURE);
			}
			catch (...)
			{
				utils::Show_MessageBox(MsgBoxFlags::Error, "Unknow Error", "An unknown error has occurred!");
				Release_Graphic();
				exit(EXIT_FAILURE);
			}
		}

		// 初始化自定义设置项
		static void Init_Custom_Setup(std::function<void()> construction)
		{
			try
			{
				construction();
			}
			catch (const std::exception& e)
			{
				utils::Show_MessageBox(MsgBoxFlags::Error, "FlexCore Error", e.what());
				Release_Graphic();
				exit(EXIT_FAILURE);
			}
			catch (const custom_error& e)
			{
				utils::Show_MessageBox(MsgBoxFlags::Error, e.title(), e.what());
				Release_Graphic();
				exit(EXIT_FAILURE);
			}
			catch (...)
			{
				utils::Show_MessageBox(MsgBoxFlags::Error, "Unknow Error", "An unknown error has occurred!");
				Release_Graphic();
				exit(EXIT_FAILURE);
			}
		}

		// 主循环
		static void Main_Loop(int fps)
		{
			try
			{
				Clock::set_fps(fps);

				bool is_running = true;
				while (is_running)
				{
					// 开始新帧
					Clock::start_frame();

					// 1.处理输入
					while (SDL_PollEvent(&Main_Event))
					{
						if (Main_Event.type == SDL_EVENT_QUIT) is_running = false;
						SceneManager::instance()->on_input(Main_Event);
					}

					// 2.处理数据
					SceneManager::instance()->on_update(Clock::get_DeltaTime()); // 场景更新
					CollisionManager::instance()->process_collision();			 // 处理碰撞

					Game_Camera->on_update(Clock::get_DeltaTime());	// 更新游戏摄像机
					UI_Camera->on_update(Clock::get_DeltaTime());	// 更新UI摄像机

					// 3.更新画面
					SDL_RenderClear(Main_Renderer);
					SceneManager::instance()->on_render(*Game_Camera, *UI_Camera);
					SDL_RenderPresent(Main_Renderer);

					// 结束该帧
					Clock::end_frame();
				}
			}
			catch (const custom_error& e)
			{
				utils::Show_MessageBox(MsgBoxFlags::Error, e.title(), e.what());
				Release_Graphic();
				exit(EXIT_FAILURE);
			}
			catch (const std::exception& e)
			{
				utils::Show_MessageBox(MsgBoxFlags::Error, "FlexCore Error", e.what());
				Release_Graphic();
				exit(EXIT_FAILURE);
			}
			catch (...)
			{
				utils::Show_MessageBox(MsgBoxFlags::Error, "Unknow Error", "An unknown error has occurred!");
				Release_Graphic();
				exit(EXIT_FAILURE);
			}
		}

		// 销毁自定义设置
		static void Release_Custom_Setup(std::function<void()> destruction)
		{
			try
			{
				destruction();
			}
			catch (const custom_error& e)
			{
				utils::Show_MessageBox(MsgBoxFlags::Error, e.title(), e.what());
				Release_Graphic();
				exit(EXIT_FAILURE);
			}
			catch (const std::exception& e)
			{
				utils::Show_MessageBox(MsgBoxFlags::Error, "FlexCore Error", e.what());
				Release_Graphic();
				exit(EXIT_FAILURE);
			}
			catch (...)
			{
				utils::Show_MessageBox(MsgBoxFlags::Error, "Unknow Error", "An unknown error has occurred!");
				Release_Graphic();
				exit(EXIT_FAILURE);
			}
		}

		// 销毁资源
		static void Release_Graphic()
		{
			delete UI_Camera;
			delete Game_Camera;

			TTF_DestroyRendererTextEngine(Main_TextEngine);
			SDL_DestroyRenderer(Main_Renderer);
			SDL_DestroyWindow(Main_Window);

			TTF_Quit();
			Mix_Quit();
			SDL_Quit();
		}
	};
	Camera* Game::Game_Camera = nullptr;
	Camera* Game::UI_Camera = nullptr;
}