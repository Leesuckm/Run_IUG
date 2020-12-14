#pragma once
#include "HelloWorldScene.h"
#include "EffectAnimation.h"
#include "MonsterCreator.h"

class MonsterCreator;

class BossMonster : public MonsterCreator
{
private:
	b2World* m_world;

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
	Sprite* m_Lava_Punch_crash;
	
	b2Body* m_LightningBall_body;
	Sprite* m_Create_Lightning_Ball;
	Sprite* m_Lightning_Ball;

	Sprite* m_Fire_column;
	bool m_fire_enable;

	Vec2 m_eye_position;
	Vec2 m_Core_position;

	Charactor_Data* m_Charactor_data;
	int m_nCharactor_beforeHp;
	bool m_bCharactor_pain;
	float m_fPainDelay_time = 1.5f;

	EffectAnimation* m_Collide_Effect;

	Layer* m_layer;

	float m_eye_distance;
	float m_eye_distanceX;
	float m_eye_distanceY;
	bool m_Bosseye_moveEnable;

	float m_fPlayTime;
	float m_Boss_ActionTime;

	int m_BossCore_Hp;
	std::vector<b2Body*> m_Lightningbody_vc;

	Vector<Sprite*> m_Lava_vc;
	Vector<Sprite*> m_Column_vc;
	Vector<Sprite*> m_Fire_vc;

public:
	BossMonster();
	~BossMonster();

	void CreateBossMap(Layer* _layer, b2World* _world, Charactor_Data* _charactor_data);
	void CreateBossEye();
	void CreateBossCore();
	virtual ESorSP* AnimationState(BossState state);

	virtual void MonsterPatrol(b2World* world, b2Body* Charactor, float movetime, EffectAnimation* _effectAnimation, Layer* layer);
	virtual void SpriteChange(BossState _bossState);

	void Pattern_IDLE();
	void Pattern_A(Sprite* _charactorSprite, float _movetime);
	void Pattern_B(Sprite* _charactorSprite, float _movetime);
	void Pattern_C(Sprite* _charactorSprite, b2Vec2 _vector);
	void Pattern_D(Sprite* _charactorSprite);

	void CreateFallingLava();

	void CreateLavaPunch(Vec2 _position);
	void LavaPunchCrash();
	
	void CreateLightningBall();
	void CreateFireColumn();

	Sprite* CreateAnimation(const std::string& _sPath, const char* _cNames, int _nSize, float _fFrameTime,Sprite* _Sprite, Vec2 _position, const std::string& _sNodename, bool _repeatForever);

	int getBossState() {return m_BossState; };
};