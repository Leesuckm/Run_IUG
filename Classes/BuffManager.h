#ifndef __BuffManager_H__
#define __BuffManager_H__
#pragma once

#include "HelloWorldScene.h"

enum BuffState;

enum DebuffState;

class BuffManager {
private:
	Layer* m_layer;
	Layer* m_glayer;
	Size m_winSize;

	BuffSlot slotin;

	Sprite* m_SlotSprite[3];
	std::queue<Sprite*> m_Sprites_slot;

	BuffState m_BuffState;

	DebuffState m_DebuffState;

	int m_nSlot_num;

	// Buff
	bool m_HpMaxUp;
	bool m_Haste;
	bool m_JumpUp;
	bool m_PowerUp;

	// Debuff
	bool m_HpMaxDown;
	bool m_Slow;
	bool m_JumpDown;
	bool m_PowerDown;

	bool m_LavaUp;

public:
	BuffManager();
	BuffManager(Layer* _glayer, Layer* _layer, Size _size);
	~BuffManager();

	BuffState getBuffState() { return m_BuffState; };
	DebuffState getDebuffState() { return m_DebuffState; };

	bool getHpMaxUp() { return m_HpMaxUp; };
	bool getHaste() { return m_Haste; };
	bool getJumpUp() { return m_JumpUp; };

	void CreateBuffSlot();
	void OpenTreasureChest(_KeyState key);

	void BuffInSlot(Charactor_Data* _charactor_data, b2Vec2 _position);
	void BuffApplication(Charactor_Data* _charactor_data);

	void CharactorInitBuff(Charactor_Data* _charactor_data);
};
#endif __BuffManager_H__