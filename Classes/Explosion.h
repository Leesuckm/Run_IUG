#ifndef __EXPLOSION_H__
#define __EXPLOSION_H__
#pragma once
#include "HelloWorldScene.h"

class Explosion{
private:
	Layer* m_layer;

	std::list<Bomb> Bombs;
	std::vector<BombChip> chips;

	cocos2d::Texture2D* m_texture;
	Sprite* m_explosion;

	Vec2 m_Exposition;
	b2AABB aabb;

	int m_numRays = 32;
	float m_blastPower = 100;

public:
	Explosion();
	~Explosion();

	void CreateExplosion(b2Body* body, float time, Layer* layer);
	bool burst(b2World* world, float dt, Layer* layer);
	void boom(b2World* world, b2Vec2 position);
	void cleanChips(b2World* world);

	int getChips_size() { return chips.size(); };
	int getBombs_size() { return Bombs.size(); };
	std::vector<BombChip>* getChips() { return &chips; };
	std::list<Bomb>* getBombs() { return &Bombs; };
};
#endif __EXPLOSION_H__