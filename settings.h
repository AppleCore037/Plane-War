#pragma once
#include <string>

const int SCREEN_WIDTH = 480;								// ���ڿ��
const int SCREEN_HEIGHT = 700;								// ���ڸ߶�
const std::string TITLE = u8"Plane War (made with SDL)";	// ���ڱ���
const int FPS = 60;											// ֡��

const float BACKGROUND_SPEED = 280.0f;		// �����ƶ��ٶ�
const float BULLET_SPEED = 800.0f;			// �ӵ��ٶ�
const int BULLET_DAMAGE_NORMAL = 100;		// �ӵ���ͨ�˺�
const int BULLET_DAMAGE_POWERUP = 150;		// �ӵ�ǿ���˺�
const float POWERUP_TIME = 8.0f;			// ǿ��ʱ��
const float PLAYER_SPEED = 250.0f;			// ����ƶ��ٶ�
const int PLAYER_HEALTH = 3;				// �������ֵ

const float SMALL_ENEMY_SPEED = 380.0f;		// С�͵л��ٶ� 380.0f
const float MIDDLE_ENEMY_SPEED = 210.0f;	// ���͵л��ٶ� 210.0f
const float BIG_ENEMY_SPEED = 100.0f;		// ���͵л��ٶ� 100.0f
const int SMALL_ENEMY_HEALTH = 100;			// С�͵л�����ֵ
const int MIDDLE_ENEMY_HEALTH = 250;		// ���͵л�����ֵ
const int BIG_ENEMY_HEALTH = 500;			// ���͵л�����ֵ