#ifndef __EFFECT_ANIMATION_H__
#define __EFFECT_ANIMATION_H__
#pragma once
#include "HelloWorldScene.h"
#include "singletonBase.h"
#include <mutex>

class EffectAnimation : public CSingletonBase<EffectAnimation>{
private:
	std::list<Effecter>* Gun_Crash_Effects;
	std::list<Effecter>* Sword_Slice_Effects;
	std::list<Effecter>* Bomb_poong_Effects;
	std::list<Effecter>* GunTrap_Crash_Effects;
	std::list<Effecter>* Collide_Effects;
	cocos2d::Texture2D* m_texture;
	Layer* m_layer;

public:
	EffectAnimation();
	EffectAnimation(Layer* _layer);
	~EffectAnimation();

	Sprite* CreateEffect(std::string& sPath, int max, Size size,Vec2 position, float animation_time, Layer* layer);
	void CollideEffect(const std::string& _sPath, const char* _cName, int max, Vec2 position, float animation_time);
	
	std::list<Effecter>* getGunEffects()		{ return Gun_Crash_Effects; };
	std::list<Effecter>* getSwordEffects()		{ return Sword_Slice_Effects; };
	std::list<Effecter>* getBombEffects()		{ return Bomb_poong_Effects; };
	std::list<Effecter>* getGunTrapEffects()	{ return GunTrap_Crash_Effects; };
	std::list<Effecter>* getCollideEffects()	{ return Collide_Effects; };
};
#endif __EFFECT_ANIMATION_H__