#ifndef __TILED_BODY_CREATOR_H__
#define __TILED_BODY_CREATOR_H__

#include "cocos2d.h"
#include <Box2D/Box2D.h>

USING_NS_CC;

class HelloWorldScene;
class EffectSprite;
class LightEffect;

enum _StageNumber;

class FixtureDef {
public:
    FixtureDef()
    : next(nullptr) {}
    
    ~FixtureDef() {
        delete next;
        delete fixture.shape;
    }
    
    FixtureDef *next;
    b2FixtureDef fixture;
    int callbackData;
};

class BodyDef {
public:
    BodyDef()
    : fixtures(nullptr) {}
    
    ~BodyDef() {
        if (fixtures)
            delete fixtures;
    }
    
    FixtureDef *fixtures;
    Point anchorPoint;
};

class TiledBodyCreator : public cocos2d::Scene
{
private :
	Layer* m_layer;

	Size m_MapSize;

	b2BodyDef** bd;
	int m_nTileGid;
	b2Body* collisionBody;
	b2Body* itemBody;
	b2Body* bulletBody;
	b2Body* bombBody;
	b2Body* LadderBody;

	b2Body* ShopBody;

	b2Body* Treasure_Chest_Red;
	b2Body* Treasure_Chest_Blue;
	b2Body* Treasure_Chest_Green;

	b2World* m_world;

	Texture2D* texture;
	Sprite* m_brokenblock;
	Sprite* m_brokenblock2;

	TMXLayer* collisionGround;
	TMXLayer* collisionItem;
	TMXLayer* collisionGround64;
	TMXLayer* collisionTreasureChest;

	_StageNumber m_stageNumber;
	bool m_Blind;

	LightEffect* m_LightEffect;
	EffectSprite* m_EffectSprite;

public:

	~TiledBodyCreator();
	std::list<b2Body*> lb_Heartlist; 
	std::list<b2Body*> lb_Obstaclelist; 
	std::list<b2Body*> lb_Pickaxelist;
	std::list<b2Body*> lb_Swordlist;
	std::list<b2Body*> lb_Gunlist;
	std::list<b2Body*> lb_Bulletlist;
	std::list<b2Body*> lb_Bomblist;
	std::list<b2Body*> lb_Ladderlist;
	std::list<b2Body*> lb_LLadderlist;
	std::list<b2Body*> lb_Goldlist;
	std::list<b2Body*> lb_Shopmanlist;
	std::list<b2Body*> lb_RedKeylist;
	std::list<b2Body*> lb_BlueKeylist;
	std::list<b2Body*> lb_GreenKeylist;

	TMXLayer* getLayer() { return collisionGround; };
	TMXLayer* getItemLayer() { return collisionItem; };

	b2Body* getItemBody() { return itemBody; }; // 가져오는 Item body는 어디까지나 캐릭터가 가지고 있는 Item의 body이다. (drop button or z key)
	b2Body* getBulletBody() { return bulletBody; };
	b2Body* getBombBody() { return bombBody; };
	b2Body* getLadderBody() { return LadderBody; };
	
	b2Body* getRedTreasureChest() { return Treasure_Chest_Red; };
	b2Body* getBlueTreasureChest() { return Treasure_Chest_Blue; };
	b2Body* getGreenTreasureChest() { return Treasure_Chest_Green; };

	Sprite* getRedTreasureChestSp()	    { return static_cast<Sprite*>(Treasure_Chest_Red->GetUserData()); };
	Sprite* getBlueTreasureChestSp()	{ return static_cast<Sprite*>(Treasure_Chest_Blue->GetUserData()); };
	Sprite* getGreenTreasureChestSp()	{ return static_cast<Sprite*>(Treasure_Chest_Green->GetUserData()); };

	Sprite* getBrokenBlock() { return m_brokenblock; };
	Sprite* getBrokenBlock2() { return m_brokenblock2; };

	LightEffect* getLightEffect() { return m_LightEffect; };
	void setLightEffect(LightEffect* _lightEffect) { m_LightEffect = _lightEffect; };

	b2Body* getShopBody() { return ShopBody; };

	void initCollisionMap(TMXTiledMap* map, b2World* world, _StageNumber _stage, bool _Blind);
	void initBlockShape(b2Vec2* vertices, Size size, b2PolygonShape* rectshape, int vindex, INT16 category, INT16 maskbits, int setdata, int hp, b2BodyType type, std::list<b2Body*>* list);
	
	Sprite* CreatePickaxe(b2Vec2 position, b2World* world, Layer* layer);

	Sprite* CreateSword(b2Vec2 position, b2World* world, Layer* layer);
	Sprite* CreateGun(b2Vec2 position, b2World* world, Layer* layer);
	Sprite* CreateBullet(b2Vec2 position, b2World* world, Layer* layer);
	Sprite* CreateBullet(b2Vec2 position, b2World* world, Layer* layer, bool sprite);

	b2Body* CreateBomb(b2Vec2 position, b2World* world, Layer* layer);
	b2Body* CreateObj(std::string obj_name, b2Vec2 position, b2World* world, Layer* layer);
	b2Body* WhatObj(std::string sPath, std::string sPath_n, std::list<b2Body*>* lb_list, b2Vec2 position, b2World* world, Layer* layer, Size _spsize, Size _objsize, INT16 _category, INT16 _maskbits, int setdata);
	b2Body* WhatObj(std::string sPath, std::list<b2Body*>* lb_list, b2Vec2 position, b2World* world, Layer* layer, Size _spsize, Size _objsize, INT16 _category, INT16 _maskbits, int setdata);

	LightEffect* SpriteChange(Layer* layer, b2World* world);
	void UserDataChange(Layer* layer, b2World* world);
	void setEffectSprite(b2Body* body, std::string& sPath, std::string& sPath_n, std::string layertype);
	void SpriteTrade(Layer* layer, b2Body* body, EffectSprite* _EffectSprite, std::string layertype);
	void setEffectAnimationSprite(b2Body* body, std::string& sPath, std::string& sPath_n, int nSheet_mx,std::string layertype);


	static FixtureDef* createFixture(ValueMap object);
	static FixtureDef* createPolygon(ValueMap object);
	static FixtureDef* createPolyline(ValueMap object);
	static FixtureDef* createCircle(ValueMap object);
	static FixtureDef* createRect(ValueMap object);
	
};

#endif // __TILED_BODY_CREATOR_H__
