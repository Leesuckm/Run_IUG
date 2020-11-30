#ifndef __Bat_H__
#define __Bat_H__
#pragma once
#include "MonsterCreator.h"

enum _moveState;

class CharactorCreator;
class EffectSprite;
class LightSprite;

class Bat : public MonsterCreator {
private:
	bool m_bJump;
	float m_Delay_time = 0;
	float before_y;
	b2Vec2 m_Between_Ch_Bat;
	b2Vec2 m_Charactor_before_position;

	ESorSP* m_Bat_Sprite;
	ESorSP* m_oldsprite;
	ESorSP* m_newSprite;

public:
	Bat();
	Bat(Charactor_Data _charactor_data, LightEffect* _effect);
	~Bat();

	virtual b2Body* CreateMonster(b2World* world, Vec2 position, Layer* layer);
	virtual ESorSP* AnimationState(_moveState state, Layer* layer);
	virtual void RecognizeMonster(b2World* world, Layer* layer);
	b2Body* CreateBody(b2Vec2 point, Size size, b2BodyType bodytype, const char* spriteName, int type, Layer* layer);


	virtual void MonsterPatrol(b2World* world, std::list<Monster> *Monsters, b2Body* Charactor, float movetime, Layer* layer, int foot);
	virtual void SpriteChange(std::list<Monster>::iterator mob, Layer* layer);

	b2Body* getMonsterBody() { return m_Monster->body; }
	b2Body* getRecognizeBody() { return m_Recognize->body; }
};
#endif __Bat_H__