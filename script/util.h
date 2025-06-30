#pragma once
#include <chrono>
#include <ctime>
#include <random>
#include <thread>
using namespace std::chrono;

class Random;
class Clock;

// �����������
class Random 
{
public:
	Random() : generator((unsigned int)std::time(0)) {}
	~Random() = default;

	// ���������������������
	void setSeed(unsigned int seed) 
	{
		generator.seed(seed);
	}

	// ����һ���� [min, max] ��Χ�ڵ��������
	int randint(int min, int max) 
	{
		std::uniform_int_distribution<int> distribution(min, max);
		return distribution(generator);
	}

	// ����һ���� [min, max] ��Χ�ڵ����������
	float randfloat(float min, float max) 
	{
		std::uniform_real_distribution<float> distribution(min, max);
		return distribution(generator);
	}

private:
	std::mt19937 generator;  // Mersenne Twister 19937 ������
};

// ʱ��
class Clock
{
public:
	Clock()
	{
		this->targetFrameTime = 1000 / 60;	// Ĭ����60֡
		this->lastFrameTime = steady_clock::now();
		this->deltaTime = 0;
		this->isFrameLimited = true;	// Ĭ������֡������
	}

	Clock(int targetFPS)
	{
		this->targetFrameTime = 1000 / targetFPS;
		this->lastFrameTime = steady_clock::now();
		this->deltaTime = 0;
		this->isFrameLimited = true;	// Ĭ������֡������
	}

	~Clock() = default;

	// ʱ�ӿ�ʼ��ʱ
	void start_frame()
	{
		auto currentTime = steady_clock::now();
		deltaTime = (float)duration_cast<milliseconds>(currentTime - lastFrameTime).count();
		lastFrameTime = currentTime;
	}

	// ʱ�ӽ�����ʱ
	void end_frame() const
	{
		if (isFrameLimited)
		{
			auto currentTime = steady_clock::now();
			auto elapsedTime = duration_cast<milliseconds>(currentTime - lastFrameTime).count();

			// �����ǰ֡��ʱС��Ŀ��֡ʱ�䣬��ȴ�ʣ��ʱ��
			if (elapsedTime < targetFrameTime)
			{
				std::this_thread::sleep_for(milliseconds(targetFrameTime - elapsedTime));
			}
		}
	}

	// ����FPS
	void setFPS(int fps_limit)
	{
		targetFrameTime = 1000 / fps_limit;
	}

	// ��ȡFPS
	int getFPS() const
	{
		return 1000 / targetFrameTime;
	}

	// ��ȡ֡��ʱ
	float getDeltaTime() const
	{
		return deltaTime / 1000.0f;  // ������Ϊ��λ��ʱ��
	}

	// �Ƿ���֡������
	void set_frame_limit(bool enable)
	{
		isFrameLimited = enable;
	}

	// �Ƿ��ѿ���֡������
	bool isFrameLimitEnabled() const
	{
		return isFrameLimited;
	}

private:
	int targetFrameTime;						// Ŀ��֡ʱ�䣨���룩
	steady_clock::time_point lastFrameTime;		// ��һ֡��ʱ���
	float deltaTime;							// ��һ֡�ĺ�ʱ�����룩
	bool isFrameLimited;						// �Ƿ�����֡������
};

namespace Layer
{
	// ��Ⱦ�㼶
	enum class Render
	{
		None,
		Background,
		Node,
		Bullet,
	};

	// ��ײ�㼶
	enum class Collision
	{
		None,
		Player,
		Enemy,
		Bullet,
		Supply
	};
}