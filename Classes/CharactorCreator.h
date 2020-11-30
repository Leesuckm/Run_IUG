#ifndef __CHARACTOR_CREATOR_H__
#define __CHARACTOR_CREATOR_H__
#pragma once
#include "HelloWorldScene.h"

class Charactor_Hand;
class EffectSprite;
class LightEffect;

class CharactorCreator : public cocos2d::Scene{
private:
	Layer* m_wlayer;
	Layer* m_clayer;
	
	Charactor_Data m_Charactor_data;

	cocos2d::Texture2D* m_texture;
	//cocos2d::Sprite* m_sPlayer;
	ESorSP* m_sPlayer;
	ESorSP* m_oldsprite;
	ESorSP* m_newSprite;
	cocos2d::Sprite* m_sPlayer_n;
	cocos2d::Sprite* m_sRecognize;
	Sprite* SP;

	b2Body* m_Charactorbody;
	b2Body* m_Camerabody;
	b2BodyDef m_MyBodyDef;
	b2BodyDef m_MyCameraDef;

	b2FixtureDef myFixtureDef;
	b2PolygonShape polygonShape;

	Charactor_Hand* m_Charactor_hand;
	b2World* m_world;

	int m_nHeartCount;
	int m_nHeartPosition;

	int m_nBombCount;
	int m_nLadderCount;

	Sprite* m_sHeart;
	Sprite* m_sBomb;
	Sprite* m_sLadder;

	Label* m_HeartLabel;
	std::string m_Heart;

	Label* m_BombLabel;
	std::string m_Bomb;

	Label* m_LadderLabel;
	std::string m_Ladder;

	bool m_bCan_Move;
	bool m_isLeft;
	bool m_isStop;
	bool m_isRight;
	bool m_isJump;
	bool m_isRightJump;
	bool m_isLeftJump;

	int m_nJumpStep;
	int m_jumpTimeout;

	LightEffect* m_effect;

protected:
	int m_Delaytime;
	_moveState m_MoveState;

public:
	CharactorCreator();
	CharactorCreator(Layer* _wlayer, Layer* _clayer);
	~CharactorCreator();
	Sprite* getPs() { return SP; }
	ESorSP* getPlayer()						{ return m_sPlayer; }
	Sprite* getRecognize()					{ return m_sRecognize; }
	Sprite* getHeartSprite()				{ return m_sHeart; }
	Sprite* getBombSprite()					{ return m_sBomb; }
	Sprite* getLadderSprite()				{ return m_sLadder; }
	int getHeartCount()						{ return m_nHeartCount; }
	Label* getHeartLabel() { return m_HeartLabel; }
	_moveState getMoveState()				{ return m_MoveState; }
	b2Body* getCharactorBody()				{ return m_Charactorbody; }
	b2Body* getCameraBody()					{ return m_Camerabody; }
	b2FixtureDef getFixtureDef()			{ return myFixtureDef; }
	Charactor_Hand* getCharactor_Hand()		{ return m_Charactor_hand; }

	void setMoveState(_moveState state)		{ m_MoveState = state; }
	void setCharactorBody(b2Body* body)		{ m_Charactorbody = body; }
	void setFixtureFriction(float friction) { myFixtureDef.friction = friction; }

	void PlusHeart() { CreateHeart(1); }

	virtual ESorSP* AnimationState(_moveState state,  Layer* layer);
	void SpriteChange(_moveState state, Layer* layer);

	void CreateCharactor(b2World* _world, Charactor_Data _Charactor_Data, Layer* layer, LightEffect* _effect);
	void CreateHeart(int nHeart);
	void CreateBomb(int nBomb);
	void CreateLadder(int nLadder);

	void Heartmanager(Layer* layer, int heartcount);
	void Bombmanager(Layer* layer, int bombcount);
	void Laddermanager(Layer* layer, int laddercount);
	void CreateCameraSprite(b2World* _world);
	void CharactorSensor(b2Vec2* vertices, int size, b2Vec2 position, int part, float fric, bool sensor);

	ESorSP* Pickup(std::string* Item, _moveState state, Layer* layer);
	ESorSP* Pickup(std::string* Item, _moveState state, Layer* layer, bool _blind);

	void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
};
#endif __CHARACTOR_CREATOR_H__
