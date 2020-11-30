#ifndef __GameUI_H__
#define __GameUI_H__
#pragma once
#include "HelloWorldScene.h"

enum _moveState;
class PauseLayer;

class GameUI{
private:
	b2World* m_world;
	Layer* m_layer;
	Size m_layerSize;

	b2Body* m_Charactorbody;
	Charactor_Data* m_Charactor_data;

	Sprite* m_Caution_Sprite;
	Sprite* m_Caution_BackSprite;


	int m_nHeartCount;
	int m_nHeartPosition;

	int m_nBombCount;
	int m_nLadderCount;

	Sprite* m_sHeart;
	Sprite* m_sBomb;
	Sprite* m_sLadder;

	Label* m_HeartLabel;
	Label* m_HeartMaxLabel;
	std::string m_Heart;

	Label* m_BombLabel;
	std::string m_Bomb;

	Label* m_LadderLabel;
	std::string m_Ladder;

	Sprite* m_Gold_Sprite;
	Label* m_Gold_Label;
	std::string m_Gold_Num;

	Sprite* m_RedKey_Sprite;
	Sprite* m_BlueKey_Sprite;
	Sprite* m_GreenKey_Sprite;

	bool m_bCaution_OnOff;
	bool m_bCaution_Turnoff;
	float m_fCaution_time;

	Label* m_MessageComment;

	Label* m_ItemShopMessage;
	bool m_bMessage = false;

	Label* m_StageMessage;
	bool m_bStageMessage_TurnOff;

	Menu* m_pPause;
	MenuItemImage* m_pPause_Image;


public:
	GameUI();
	GameUI(b2World* world, Layer* layer, b2Body* CharactorBody, Charactor_Data* Charactor_data);
	~GameUI();

	float m_StageMessageTime;

	void CreateUI();
	void CreatePauseButton();
	Sprite* CreateCautionMessage();
	Sprite* CreateCautionBackGround();
	void CreateStageMessage(int _StageNumber);

	Sprite* getCautionSprite()		{ return m_Caution_Sprite; };
	bool getCautionOnOff()			{ return m_bCaution_OnOff; };
	void setCautionOnOff(bool tf)	{ m_bCaution_OnOff = tf; };

	bool getCautionTurnoff()		{ return m_bCaution_Turnoff; };
	void setCautionTurnoff(bool tf) { m_bCaution_Turnoff = tf; };

	float getCautionTime()			{ return m_fCaution_time; };
	void setCautionTime(float time) { m_fCaution_time = time; };

	Sprite* CreateGoldUi(int _nGold);
	void GoldManager(Layer* layer, int _nGold);

	void CreateKeyUi();

	void CharactorStateSprite();
	Sprite* CreateStateSprite(std::string sPath, Vec2 pos, Size size);

	void Heartmanager(Charactor_Data* _charactor_data);
	void Bombmanager(int bombcount);
	void Laddermanager(int laddercount);

	Sprite* getRedKeySprite()		{ return m_RedKey_Sprite; };
	Sprite* getBlueKeySprite()		{ return m_BlueKey_Sprite; };
	Sprite* getGreenKeySprite()		{ return m_GreenKey_Sprite; };

	Label* getGoldLabel()			 { return m_Gold_Label; };
	Label* getItemMessage()			{ return m_MessageComment; };
	Label* getStageMessage()		{ return m_StageMessage; };

	bool getMessage() { return m_bMessage; };
	void setMessage(bool bMessage) { m_bMessage = bMessage; }

	bool getStageMessageTurnOff()		 { return m_bStageMessage_TurnOff; };
	void setStageMessageTurnOff(bool tf) { m_bStageMessage_TurnOff = tf; };

	void ItemShopMessage(std::string comment);
	void MessageOutput(std::string comment);
	bool TemporaryMessageManager(float* max, float dt);
	void MessageDelete(Label* _label);

	void doPause(Ref* pSender);
	
};
#endif __GameUI_H__