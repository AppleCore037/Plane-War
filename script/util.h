#pragma once
#include <chrono>
#include <ctime>
#include <random>
#include <thread>
using namespace std::chrono;

class Random;
class Clock;

// 随机数生成器
class Random 
{
public:
	Random() : generator((unsigned int)std::time(0)) {}
	~Random() = default;

	// 设置随机数生成器的种子
	void setSeed(unsigned int seed) 
	{
		generator.seed(seed);
	}

	// 生成一个在 [min, max] 范围内的随机整数
	int randint(int min, int max) 
	{
		std::uniform_int_distribution<int> distribution(min, max);
		return distribution(generator);
	}

	// 生成一个在 [min, max] 范围内的随机浮点数
	float randfloat(float min, float max) 
	{
		std::uniform_real_distribution<float> distribution(min, max);
		return distribution(generator);
	}

private:
	std::mt19937 generator;  // Mersenne Twister 19937 生成器
};

// 时钟
class Clock
{
public:
	Clock()
	{
		this->targetFrameTime = 1000 / 60;	// 默认是60帧
		this->lastFrameTime = steady_clock::now();
		this->deltaTime = 0;
		this->isFrameLimited = true;	// 默认启用帧率限制
	}

	Clock(int targetFPS)
	{
		this->targetFrameTime = 1000 / targetFPS;
		this->lastFrameTime = steady_clock::now();
		this->deltaTime = 0;
		this->isFrameLimited = true;	// 默认启用帧率限制
	}

	~Clock() = default;

	// 时钟开始计时
	void start_frame()
	{
		auto currentTime = steady_clock::now();
		deltaTime = (float)duration_cast<milliseconds>(currentTime - lastFrameTime).count();
		lastFrameTime = currentTime;
	}

	// 时钟结束计时
	void end_frame() const
	{
		if (isFrameLimited)
		{
			auto currentTime = steady_clock::now();
			auto elapsedTime = duration_cast<milliseconds>(currentTime - lastFrameTime).count();

			// 如果当前帧耗时小于目标帧时间，则等待剩余时间
			if (elapsedTime < targetFrameTime)
			{
				std::this_thread::sleep_for(milliseconds(targetFrameTime - elapsedTime));
			}
		}
	}

	// 设置FPS
	void setFPS(int fps_limit)
	{
		targetFrameTime = 1000 / fps_limit;
	}

	// 获取FPS
	int getFPS() const
	{
		return 1000 / targetFrameTime;
	}

	// 获取帧耗时
	float getDeltaTime() const
	{
		return deltaTime / 1000.0f;  // 返回秒为单位的时间
	}

	// 是否开启帧数限制
	void set_frame_limit(bool enable)
	{
		isFrameLimited = enable;
	}

	// 是否已开启帧数限制
	bool isFrameLimitEnabled() const
	{
		return isFrameLimited;
	}

private:
	int targetFrameTime;						// 目标帧时间（毫秒）
	steady_clock::time_point lastFrameTime;		// 上一帧的时间点
	float deltaTime;							// 上一帧的耗时（毫秒）
	bool isFrameLimited;						// 是否启用帧率限制
};

namespace Layer
{
	// 渲染层级
	enum class Render
	{
		None,
		Background,
		Node,
		Bullet,
	};

	// 碰撞层级
	enum class Collision
	{
		None,
		Player,
		Enemy,
		Bullet,
		Supply
	};
}