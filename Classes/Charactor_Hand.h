#pragma once

enum _HandState {
	Hand_On,
	LeftHand_On,
	RightHand_On,
	LeftHand_Off,
	RightHand_Off,
	TwoHand_On,
	TwoHand_Off,
	JumpHand_On,
	JumpHand_Down,
	UPDOWN,
	ATTACK,
	UP_ATTACK,
	UNDER_ATTACK,
	THROW_BOMB,
	PAIN,
};

enum _moveState;
class CharactorCreator;
class EffectSprite;
class LightSprite;

class Charactor_Hand{
private:
	Charactor_Data m_Charactor_data;

	_HandState m_HandState;
	cocos2d::Texture2D* m_texture;
	ESorSP* m_Pickup_Lefthand;
	LightEffect* m_effect;

	b2Body* m_Charactorbody;
	b2Body* m_HandObjbody;
	b2Body* m_ItemObjbody;
	b2JointDef m_jointdef;
	b2World* m_world;
	Layer* m_Layer;

	int m_nDurability;

public:
	Charactor_Hand(Charactor_Data _charactor_data);
	~Charactor_Hand();

	void CreateHandObj(std::string sPath);

	ESorSP* getSprite() { return m_Pickup_Lefthand; }

	int getDurability() { return m_nDurability; }
	_HandState getHandState() { return m_HandState; }

	void setDurability(int durability) { m_nDurability = durability; }

	void Holeinone_LeftHand(std::string sPath,std::string item, b2Body* Charactorbody, _moveState state, Layer* layer, b2World* _world, LightEffect* _effect);
	void Holeinone_RightHand();

	ESorSP* AnimationState(std::string Obj_name, _HandState state, Layer* layer, bool viewing);
};