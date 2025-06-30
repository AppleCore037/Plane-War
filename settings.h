#pragma once
#include <string>

const int SCREEN_WIDTH = 480;								// 窗口宽度
const int SCREEN_HEIGHT = 700;								// 窗口高度
const std::string TITLE = u8"Plane War (made with SDL)";	// 窗口标题
const int FPS = 60;											// 帧率

const float BACKGROUND_SPEED = 280.0f;		// 背景移动速度
const float BULLET_SPEED = 800.0f;			// 子弹速度
const int BULLET_DAMAGE_NORMAL = 100;		// 子弹普通伤害
const int BULLET_DAMAGE_POWERUP = 150;		// 子弹强化伤害
const float POWERUP_TIME = 8.0f;			// 强化时间
const float PLAYER_SPEED = 250.0f;			// 玩家移动速度
const int PLAYER_HEALTH = 3;				// 玩家生命值

const float SMALL_ENEMY_SPEED = 380.0f;		// 小型敌机速度 380.0f
const float MIDDLE_ENEMY_SPEED = 210.0f;	// 中型敌机速度 210.0f
const float BIG_ENEMY_SPEED = 100.0f;		// 大型敌机速度 100.0f
const int SMALL_ENEMY_HEALTH = 100;			// 小型敌机生命值
const int MIDDLE_ENEMY_HEALTH = 250;		// 中型敌机生命值
const int BIG_ENEMY_HEALTH = 500;			// 大型敌机生命值