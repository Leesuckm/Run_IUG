#ifndef __Obstacle_H__
#define __Obstacle_H__
#pragma once
#include "MonsterCreator.h"
#include "EffectAnimation.h"

enum _moveState;

struct BladeObstacle {
	b2Body* BaseBody;
	b2Body* BladeBody;
	Sprite* RopeSprite;
};

struct GunObstacle {
	b2Body* BaseBody;
	b2Body* RecognizeBody;
	Sprite* RecognizeSprite;
};

class CharactorCreator;
class EffectSprite;
class LightSprite;

class Obstacle {
private:
	b2World* m_world;
	Layer* m_layer;

	EffectAnimation m_EffectAnimation;

	LightEffect* m_lighteffect;
	Charactor_Data* m_Charactor_data;

	Sprite* m_Lava_Sprite;
	Sprite* m_Lava_Spriteb;
	float m_fLava_Eruptiontime;
	Vec2 m_oldLava_pos;
	Vec2 m_newLava_pos;

	float m_fObstacleDelayTime = 2;
	float m_fLopelength;

	float m_fStartDelay = 2;  
	float m_fBulletFireDelay = 2;

	bool m_bJump;
	float jump_time = 0;
	float before_y;
	b2WheelJoint* m_spring1, *m_spring2;

	std::list<BladeObstacle*> lb_BladeOb;
	std::list<GunObstacle*> lb_GunOb;
	std::list<b2Body*> lb_TrapBullet;

public:
	Obstacle();
	Obstacle(b2World* world, Layer* layer);
	Obstacle(b2World* world, Layer* layer, LightEffect* _lighteffect);
	~Obstacle();

	void CreateObstacle(Charactor_Data* _charactor_data);
	Sprite* CreateLava();
	void LavaEruption(float dt);

	void CreateTraps();
	void CreatefallingBlade(b2Body* b);
	void CreatefallingBlade(b2Body* b, bool _blind);
	void CreateGunTrap();

	//void FallingBladeControl(int contect, float dt);
	Charactor_Data* FallingBladeControl(int contect, float dt, float* _delaytime, b2Body* _charactor_body);
	void GunTrapControl(int contect, float dt, float* _delaytime, Charactor_Data* _charactor_data, b2Body* _charactor_body, int* _contactBullet);
	bool GunBulletCollision(Sprite* charactor_sprite, int* _contactBullet);
	void GunTrapFireAnimation(GunObstacle* _gunObstacle, const std::string& _sPlist_Path, const char* _cFilename_Path);
	void GunTrapFireAnimation(GunObstacle* _gunObstacle, const std::string& _sPlist_Path, const std::string& _sSheet_Path, const char* _cFilename_Path, LightEffect* _lighteffect);
	

	Sprite* getLavaSprite() {return m_Lava_Sprite; };
	Sprite* getLavaSpriteb() { return m_Lava_Spriteb; };

	Sprite* AnimationState(_moveState state, Layer* layer);
	
	void SpriteChange(std::list<Monster>::iterator mob, Layer* layer);

	
};
#endif __Obstacle_H__