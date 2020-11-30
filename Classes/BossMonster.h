#pragma once
#include "HelloWorldScene.h"
#include "MonsterCreator.h"

class MonsterCreator;

class BossMonster : public MonsterCreator
{
private:
	BossState m_BossState;
	BossState m_BeforeBossState;

	Sprite* m_BossMap_Background;

	Sprite* FallingLava_Sprite;

	Sprite* m_Boss_eye;
	Sprite* m_Boss_eye_Sprite;
	Sprite* m_Boss_Core_Sprite;

	Sprite* m_Boss_eye_in[4];
	Sprite* m_Boss_eye_out[4];

	Sprite* m_Lava_Punch;

	Vec2 m_eye_position;
	Vec2 m_Core_position;

	Charactor_Data* m_Charactor_data;
	Layer* m_layer;

	float m_eye_distance;
	float m_eye_distanceX;
	float m_eye_distanceY;
	bool m_Bosseye_moveEnable;

	float m_fPlayTime;
	float m_Boss_ActionTime;

	int m_BossCore_Hp;

	Vector<Sprite*> m_Lava_vc;

public:
	BossMonster();
	~BossMonster();

	void CreateBossMap(Layer* _layer, Charactor_Data* _charactor_data);
	void CreateBossEye();
	void CreateBossCore();
	virtual ESorSP* AnimationState(BossState state);

	virtual void MonsterPatrol(b2World* world, b2Body* Charactor, float movetime, Layer* layer);
	virtual void SpriteChange(BossState _bossState);

	void Pattern_A();
	void Pattern_B(Vec2 position);
	//void Pattern_C();
	//void Pattern_D();

	bool getBossState() {return m_BossState; };
};