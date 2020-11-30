#include "TiledBodyCreator.h"
#include "HelloWorldScene.h"
#include "LightEffect.h"
#include "EffectSprite.h"

USING_NS_CC;

#define PTMRATIO 32

typedef enum {
	POLYGON_FIXTURE,
	POLYLINE_FIXTURE,
    RECT_FIXTURE,
	CIRCLE_FIXTURE,
	UNKNOWN_FIXTURE
} fixtureTypes;

TiledBodyCreator::~TiledBodyCreator() {
	for (int i = 0; i < m_MapSize.height; i++) {
		delete[] bd[i];
	}
	delete[] bd;
}

void TiledBodyCreator::initCollisionMap(TMXTiledMap* map, b2World* world, _StageNumber _stage, bool _blind)
{
	// tile map에 있는 Collision 오브젝트를 가져옴
	//auto collisionGroup = map->getObjectGroup("Collision");
	collisionGround = map->getLayer("Ground");
	collisionItem = map->getLayer("Item");

	m_stageNumber = _stage;
	m_Blind = _blind;

	if (m_stageNumber != BOSS) {
		m_MapSize = Size(60, 32);
	}
	else if (m_stageNumber == BOSS) {
		collisionGround64 = map->getLayer("Ground_64");
		m_MapSize = Size(40, 24);
	}

	if (m_stageNumber == SHELTER) {
		collisionTreasureChest = map->getLayer("Treasure_chest");
	}

	// ValueVector 는 typedef std::vector<Value> Value라는 클래스의 vector 구조이다.
	//cocos2d::ValueVector collisionObjects = collisionGroup->getObjects();
	m_world = world;
	texture = Director::getInstance()->getTextureCache()->addImage("Images/brokenblock.png");
	m_brokenblock = Sprite::createWithTexture(texture, Rect(0, 0, 32, 32));
	m_brokenblock2 = Sprite::createWithTexture(texture, Rect(32, 0, 64, 32));
		
	bd = new b2BodyDef*[m_MapSize.height];
	for (int i = 0; i < m_MapSize.height; i++) {
		bd[i] = new b2BodyDef[m_MapSize.width];
	}
//	b2BodyDef bd[32][60];
	int k = m_MapSize.height - 1;
	for (int i = 0; i < m_MapSize.height; i++) {
		for (int j = 0; j < m_MapSize.width; j++) {
			
			bd[i][j].userData = collisionGround->getTileAt(Vec2(j, k));
			if (collisionItem->getTileAt(Vec2(j, k)) != nullptr) {			// 아이템 위치 Layer가 Background와 Item이 서로 다르므로 Sprite가 서로 겹치지 않도록 주의해야한다.
				bd[i][j].userData = collisionItem->getTileAt(Vec2(j, k));	// Item Layer에 최소 하나의 Object가 있어야 한다.
			}
			if (_stage == SHELTER) {
				if (collisionTreasureChest->getTileAt(Vec2(j, k)) != nullptr) {
					bd[i][j].userData = collisionTreasureChest->getTileAt(Vec2(j, k));
				}
			}
			if (_stage == BOSS) {
				if (collisionGround64->getTileAt(Vec2(j, k)) != nullptr) {
					bd[i][j].userData = collisionGround64->getTileAt(Vec2(j, k));
				}
			}
			if (bd[i][j].userData != nullptr) {
				bd[i][j].position = b2Vec2(j, i);
			}
		}
		k--;
	}

	/*
	각 Objects의 충돌 Filter Bits
	
	Charactor
		Kategorie_Bits = 0x0001
		Mask_Bits	   = 0xFFFF
	Item
		Kategorie_Bits = 0x1000
		Mask_Bits	   = 0x000F
	Obstacle
		Kategorie_Bits = 0x0100
		Mask_Bits	   = 0x000F
	Monster
		Kategorie_Bits = 0x0010
		Mask_Bits	   = 0x000F
	Block
		Kategorie_Bits = 0x0002
		Mask_Bits	   = 0xFFFF
	Pickaxe
		Kategorie_Bits = 0x2000
		Mask_Bits	   = 0x000E
	Bomb
		Kategorie_Bits = 0x3000
		Mask_Bits	   = 0x000E
	Bomb_bullet
		Kategorie_Bits = 0x0003
		Mask_Bits	   = 0x000F

	*/

	k = m_MapSize.height - 1;
	for (int i = 0; i < m_MapSize.height; i++) {
		for (int j = 0; j < m_MapSize.width; j++) {
			//collisionBody = world->CreateBody(&bd[i][j]); // 타일을 넣는 시점.
			collisionBody = world->CreateBody(&bd[i][j]);
			collisionBody->SetIsCh(false);
			cocos2d::Value objectValue;

			collisionBody->setNewobj(false);
			auto position = Vec2(0, 0); // 타일을 월드에 추가한 뒤에 body를 놓을 위치 설정 body의 위치는 스프라이트가 올라간 위치를 기준으로 한다.
			float width = 32 / PTMRATIO;
			float height = 32 / PTMRATIO;

			b2PolygonShape *rectshape = new b2PolygonShape();
			b2Vec2 vertices[4];
			int vindex = 4;

			auto fix = new FixtureDef();
			b2Fixture* SensorFixture;

			m_nTileGid = this->collisionGround->getTileGIDAt(Vec2(j, k));
			if (m_nTileGid != 0 && m_nTileGid != 2 && m_nTileGid != 6 && m_nTileGid != 9 && m_nTileGid != 14) {
				log("da");
			}

			switch (m_nTileGid) {
			case 2 : // 물약
				initBlockShape(vertices, Size(0.2, 0.3), rectshape, vindex, POTION_CATEGORY, 0x000E, 6, 1000, b2_dynamicBody, &lb_Heartlist);		
				break;
			case 5: // 상점 아저씨 위치
				initBlockShape(vertices, Size(0.0f, 0.0f), rectshape, vindex, 0x0700, 0x000E, 70, 1000, b2_dynamicBody, &lb_Shopmanlist);
				break;
			case 6 : // 장애물
				initBlockShape(vertices, Size(0.2f, 0.3f), rectshape, vindex, 0x0100, 0x000F, 5, 1000, b2_staticBody, &lb_Obstaclelist);
				break;
			case 9: // 테두리 하드 블럭
			case 40:
			case 65:
				initBlockShape(vertices, Size(0.0f, 0.0f), rectshape, vindex, 0x0022, 0xFFFF, 0, 3000, b2_staticBody, nullptr);
				break;
			case 12:
				initBlockShape(vertices, Size(0.0f, 0.0f), rectshape, vindex, LGUNTRAP_CATEGORY, 0xFFFF, 0, 3000, b2_staticBody, nullptr);
				break;
			case 13:
				initBlockShape(vertices, Size(0.0f, 0.0f), rectshape, vindex, RGUNTRAP_CATEGORY, 0xFFFF, 0, 3000, b2_staticBody, nullptr);
				break;
			case 14: // 기본 블럭
			case 55:
			case 67:
			case 69:
				initBlockShape(vertices, Size(0.0f, 0.0f), rectshape, vindex, 0x0002, 0xFFFF, 0, 27, b2_staticBody, nullptr);
				break;
			case 15:
				initBlockShape(vertices, Size(0.0f, 0.0f), rectshape, vindex, BLADEBLOCK_CATEGORY, 0xFFFF, 0, 3, b2_staticBody, nullptr);
				break;
			case 16: // 골드 블럭
			case 56:
				initBlockShape(vertices, Size(0.0f, 0.0f), rectshape, vindex, 0x0007, 0xFFFF, 0, 3, b2_staticBody, nullptr);
				break;
			}

			// ItemLayer
			m_nTileGid = this->collisionItem->getTileGIDAt(Vec2(j, k));
			if (m_nTileGid != 0) {
				log("gid = %f", m_nTileGid);
			}		
			switch (m_nTileGid) {
			case 77:    // 폭탄
				initBlockShape(vertices, Size(0.0f, 0.0f), rectshape, vindex, 0x3000, 0x000E, 9, 1000, b2_dynamicBody, &lb_Bomblist);
				break;

			case 78: // 사다리			
				initBlockShape(vertices, Size(0.0f, 0.0f), rectshape, vindex, 0x7000, 0x000E, 13, 1000, b2_dynamicBody, &lb_Ladderlist);
				break;

			case 79:	// 곡괭이			
				initBlockShape(vertices, Size(0.2f, 0.3f), rectshape, vindex, 0x2000, 0x000E, 7, 1000, b2_dynamicBody, &lb_Pickaxelist);
				break;

			case 80:  // 검				
				initBlockShape(vertices, Size(0.2f, 0.3f), rectshape, vindex, 0x4000, 0x000E, 11, 1000, b2_dynamicBody, &lb_Swordlist);

				break;
			case 81:  // 권총
				initBlockShape(vertices, Size(0.2f, 0.3f), rectshape, vindex, 0x5000, 0x000E, 12, 1000, b2_dynamicBody, &lb_Gunlist);
				break;
			case 82:  // Red Key
				initBlockShape(vertices, Size(0.0f, 0.0f), rectshape, vindex, REDKEY_CATEGORY, 0x000E, 90, 1000, b2_dynamicBody, &lb_RedKeylist);
				break;			
			case 83:  // Blue Key
				initBlockShape(vertices, Size(0.0f, 0.0f), rectshape, vindex, BLUEKEY_CATEGORY, 0x000E, 91, 1000, b2_dynamicBody, &lb_BlueKeylist);
				break;
			case 84:  // Green Key
				initBlockShape(vertices, Size(0.0f, 0.0f), rectshape, vindex, GREENKEY_CATEGORY, 0x000E, 92, 1000, b2_dynamicBody, &lb_GreenKeylist);
				break;
			}

			// TreasureChest
			if (_stage == SHELTER) {
				m_nTileGid = this->collisionTreasureChest->getTileGIDAt(Vec2(j, k));
				if (m_nTileGid != 0) {
					log("gid = %f", m_nTileGid);
				}

				switch (m_nTileGid) {
				case 73:    // BLUE Treasure Chest
					vertices[0].x = position.x + 0.0f;
					vertices[0].y = position.y + 0.0f;

					vertices[1].x = position.x + 0.0f;
					vertices[1].y = position.y + (height + 3.0f);

					vertices[2].x = position.x + (width + 5.0f);
					vertices[2].y = position.y + (height + 3.0f);

					vertices[3].x = position.x + (width + 5.0f);
					vertices[3].y = position.y + 0.0f;

					collisionBody->SetHp(1000);
					collisionBody->SetType(b2_dynamicBody);

					rectshape->Set(vertices, vindex);
					fix->fixture.shape = rectshape;
					fix->fixture.isSensor = false;
					fix->fixture.filter.categoryBits = BLUECHEST_CATEGORY;
					fix->fixture.filter.maskBits = 0x000E;

					SensorFixture = collisionBody->CreateFixture(&fix->fixture);
					SensorFixture->SetUserData((void*)40);

					Treasure_Chest_Blue = collisionBody;
					
					break;

				case 74:    // GREEN Treasure Chest
					vertices[0].x = position.x + 0.0f;
					vertices[0].y = position.y + 0.0f;

					vertices[1].x = position.x + 0.0f;
					vertices[1].y = position.y + (height + 3.0f);

					vertices[2].x = position.x + (width + 5.0f);
					vertices[2].y = position.y + (height + 3.0f);

					vertices[3].x = position.x + (width + 5.0f);
					vertices[3].y = position.y + 0.0f;

					collisionBody->SetHp(1000);
					collisionBody->SetType(b2_dynamicBody);

					rectshape->Set(vertices, vindex);
					fix->fixture.shape = rectshape;
					fix->fixture.isSensor = false;
					fix->fixture.filter.categoryBits = GREENCHEST_CATEGORY;
					fix->fixture.filter.maskBits = 0x000E;

					SensorFixture = collisionBody->CreateFixture(&fix->fixture);
					SensorFixture->SetUserData((void*)41);

					Treasure_Chest_Green = collisionBody;
					//lb_Bomblist.push_back(collisionBody);
					break;

				case 75:    // Red Treasure Chest
					vertices[0].x = position.x + 0.0f;
					vertices[0].y = position.y + 0.0f;

					vertices[1].x = position.x + 0.0f;
					vertices[1].y = position.y + (height + 3.0f);

					vertices[2].x = position.x + (width + 5.0f);
					vertices[2].y = position.y + (height + 3.0f);

					vertices[3].x = position.x + (width + 5.0f);
					vertices[3].y = position.y + 0.0f;

					collisionBody->SetHp(1000);
					collisionBody->SetType(b2_dynamicBody);

					rectshape->Set(vertices, vindex);
					fix->fixture.shape = rectshape;
					fix->fixture.isSensor = false;
					fix->fixture.filter.categoryBits = REDCHEST_CATEGORY;
					fix->fixture.filter.maskBits = 0x000E;

					SensorFixture = collisionBody->CreateFixture(&fix->fixture);
					SensorFixture->SetUserData((void*)42);

					Treasure_Chest_Red = collisionBody;
					//lb_Bomblist.push_back(collisionBody);
					break;
				}
			}
			if (_stage == BOSS) {
				m_nTileGid = this->collisionGround64->getTileGIDAt(Vec2(j, k));
				if (m_nTileGid != 0) {
					log("gid = %f", m_nTileGid);
				}
				switch (m_nTileGid) {
				case 85:
					break;
				case 86:
					break;
				case 87:
				case 88:
					initBlockShape(vertices, Size(-1.0f, -1.0f), rectshape, vindex, 0x0002, 0xFFFF, 0, 27, b2_staticBody, nullptr);
					break;

				}
			}
			//collisionBody->CreateFixture(&fix->fixture);
		}
		k--;
	}
}

void TiledBodyCreator::initBlockShape(b2Vec2* vertices, Size size, b2PolygonShape* rectshape, int vindex, INT16 category, INT16 maskbits, int setdata, int hp, b2BodyType type, std::list<b2Body*>* list) {

	auto fix = new FixtureDef();
	b2Fixture* SensorFixture;

	vertices[0].x = 0 + size.width;
	vertices[0].y = 0 + 0.0f;

	vertices[1].x = 0 + size.width;
	vertices[1].y = 0 + (1 - size.height);

	vertices[2].x = 0 + (1 - size.width);
	vertices[2].y = 0 + (1 - size.height);

	vertices[3].x = 0 + (1 - size.width);
	vertices[3].y = 0 + 0.0f;
	rectshape->Set(vertices, vindex);

	fix->fixture.shape = rectshape;
	fix->fixture.filter.categoryBits = category;
	fix->fixture.filter.maskBits = maskbits;
	//fix->fixture.filter.groupIndex = -1;

	collisionBody->SetHp(hp);			// 초기 물약의 Hp (캐릭터로부터 공격받을시에 어느정도의 공격을 버틸 수 있는지)
	collisionBody->SetType(type);

	SensorFixture = collisionBody->CreateFixture(&fix->fixture);

	if (setdata != 0) {
		SensorFixture->SetUserData((void*)setdata);
	}

	if (list != nullptr) {
		list->push_back(collisionBody);
	}
}


Sprite* TiledBodyCreator::CreatePickaxe(b2Vec2 position, b2World* world, Layer* layer) {
	b2BodyDef bd;
	//Texture2D* texture = Director::getInstance()->getTextureCache()->addImage("Images/pickaxe.png");
	//Sprite* Pickaxe_Sprite = Sprite::createWithTexture(texture, Rect(0, 0, 40, 32));
	EffectSprite* Pickaxe_Sprite = EffectSprite::create("n_Images/item/pickaxe.png");
	Pickaxe_Sprite->setEffect(m_LightEffect, "n_Images/item/pickaxe_n.png");
	Pickaxe_Sprite->setColor(m_LightEffect->getAmbientLightColor());
	Pickaxe_Sprite->setAnchorPoint(Vec2(0, 0));
	bd.userData = Pickaxe_Sprite;

	layer->addChild(Pickaxe_Sprite, Obj_ZOrder);
	bd.position = position;
	
	itemBody = world->CreateBody(&bd);
	itemBody->SetIsCh(false);
	
	itemBody->setNewobj(true);
	b2PolygonShape *rectshape = new b2PolygonShape();
	b2Vec2 vertices[4];
	int vindex = 4;

	auto posi = Vec2(0, 0); // 타일을 월드에 추가한 뒤에 body를 놓을 위치 설정 body의 위치는 스프라이트가 올라간 위치를 기준으로 한다.
	float width = 32 / PTMRATIO;
	float height = 32 / PTMRATIO;

	auto fix = new FixtureDef();
	b2Fixture* SensorFixture;

	vertices[0].x = posi.x + 0.2f;
	vertices[0].y = posi.y + 0.0f;

	vertices[1].x = posi.x + 0.2f;
	vertices[1].y = posi.y + (height - 0.3);

	vertices[2].x = posi.x + (width - 0.3);
	vertices[2].y = posi.y + (height - 0.3);

	vertices[3].x = posi.x + (width - 0.3);
	vertices[3].y = posi.y + 0.0f;

	itemBody->SetHp(1000);
	itemBody->SetType(b2_dynamicBody);

	rectshape->Set(vertices, vindex);
	fix->fixture.shape = rectshape;
	fix->fixture.isSensor = false;
	fix->fixture.filter.categoryBits = 0x2000;
	fix->fixture.filter.maskBits = 0x000E;

	SensorFixture = itemBody->CreateFixture(&fix->fixture);
	SensorFixture->SetUserData((void*)7);

	lb_Pickaxelist.push_back(itemBody);

	return Pickaxe_Sprite;
}


Sprite* TiledBodyCreator::CreateSword(b2Vec2 position, b2World* world, Layer* layer) {
	b2BodyDef bd;
	//Texture2D* texture = Director::getInstance()->getTextureCache()->addImage("Images/pickaxe.png");
	//Sprite* Pickaxe_Sprite = Sprite::createWithTexture(texture, Rect(0, 0, 40, 32));
	EffectSprite* Sword_Sprite = EffectSprite::create("n_Images/item/sword.png");
	Sword_Sprite->setEffect(m_LightEffect, "n_Images/item/sword_n.png");
	Sword_Sprite->setColor(m_LightEffect->getAmbientLightColor());
	Sword_Sprite->setAnchorPoint(Vec2(0, 0));
	bd.userData = Sword_Sprite;

	layer->addChild(Sword_Sprite, Obj_ZOrder);
	bd.position = position;

	itemBody = world->CreateBody(&bd);
	itemBody->SetIsCh(false);

	itemBody->setNewobj(true);
	b2PolygonShape *rectshape = new b2PolygonShape();
	b2Vec2 vertices[4];
	int vindex = 4;

	auto posi = Vec2(0, 0); // 타일을 월드에 추가한 뒤에 body를 놓을 위치 설정 body의 위치는 스프라이트가 올라간 위치를 기준으로 한다.
	float width = 32 / PTMRATIO;
	float height = 32 / PTMRATIO;

	auto fix = new FixtureDef();
	b2Fixture* SensorFixture;

	vertices[0].x = posi.x + 0.2f;
	vertices[0].y = posi.y + 0.0f;

	vertices[1].x = posi.x + 0.2f;
	vertices[1].y = posi.y + (height - 0.3);

	vertices[2].x = posi.x + (width - 0.3);
	vertices[2].y = posi.y + (height - 0.3);

	vertices[3].x = posi.x + (width - 0.3);
	vertices[3].y = posi.y + 0.0f;

	itemBody->SetHp(1000);
	itemBody->SetType(b2_dynamicBody);

	rectshape->Set(vertices, vindex);
	fix->fixture.shape = rectshape;
	fix->fixture.isSensor = false;
	fix->fixture.filter.categoryBits = 0x4000;
	fix->fixture.filter.maskBits = 0x000E;

	SensorFixture = itemBody->CreateFixture(&fix->fixture);
	SensorFixture->SetUserData((void*)11);

	lb_Swordlist.push_back(itemBody);

	return Sword_Sprite;
}

Sprite* TiledBodyCreator::CreateGun(b2Vec2 position, b2World* world, Layer* layer) {
	b2BodyDef bd;
	//Texture2D* texture = Director::getInstance()->getTextureCache()->addImage("Images/pickaxe.png");
	//Sprite* Pickaxe_Sprite = Sprite::createWithTexture(texture, Rect(0, 0, 40, 32));
	EffectSprite* Gun_Sprite = EffectSprite::create("n_Images/item/gun.png");
	Gun_Sprite->setEffect(m_LightEffect, "n_Images/item/gun_n.png");
	Gun_Sprite->setColor(m_LightEffect->getAmbientLightColor());
	Gun_Sprite->setAnchorPoint(Vec2(0, 0));
	bd.userData = Gun_Sprite;

	layer->addChild(Gun_Sprite, Obj_ZOrder);
	bd.position = position;

	itemBody = world->CreateBody(&bd);
	itemBody->SetIsCh(false);

	itemBody->setNewobj(true);
	b2PolygonShape *rectshape = new b2PolygonShape();
	b2Vec2 vertices[4];
	int vindex = 4;

	auto posi = Vec2(0, 0); // 타일을 월드에 추가한 뒤에 body를 놓을 위치 설정 body의 위치는 스프라이트가 올라간 위치를 기준으로 한다.
	float width = 32 / PTMRATIO;
	float height = 32 / PTMRATIO;

	auto fix = new FixtureDef();
	b2Fixture* SensorFixture;

	vertices[0].x = posi.x + 0.2f;
	vertices[0].y = posi.y + 0.0f;

	vertices[1].x = posi.x + 0.2f;
	vertices[1].y = posi.y + (height - 0.3);

	vertices[2].x = posi.x + (width - 0.3);
	vertices[2].y = posi.y + (height - 0.3);

	vertices[3].x = posi.x + (width - 0.3);
	vertices[3].y = posi.y + 0.0f;

	itemBody->SetHp(1000);
	itemBody->SetType(b2_dynamicBody);

	rectshape->Set(vertices, vindex);
	fix->fixture.shape = rectshape;
	fix->fixture.isSensor = false;
	fix->fixture.filter.categoryBits = 0x5000;
	fix->fixture.filter.maskBits = 0x000E;

	SensorFixture = itemBody->CreateFixture(&fix->fixture);
	SensorFixture->SetUserData((void*)12);

	lb_Gunlist.push_back(itemBody);

	return Gun_Sprite;
}

Sprite* TiledBodyCreator::CreateBullet(b2Vec2 position, b2World* world, Layer* layer) {
	b2BodyDef bd;
	//Texture2D* texture = Director::getInstance()->getTextureCache()->addImage("Images/pickaxe.png");
	//Sprite* Pickaxe_Sprite = Sprite::createWithTexture(texture, Rect(0, 0, 40, 32));
	EffectSprite* Bullet_Sprite = EffectSprite::create("n_Images/item/bullet.png");
	Bullet_Sprite->setEffect(m_LightEffect, "n_Images/item/bullet_n.png");
	Bullet_Sprite->setColor(m_LightEffect->getAmbientLightColor());
	Bullet_Sprite->setAnchorPoint(Vec2(0, 0));
	bd.userData = Bullet_Sprite;

	layer->addChild(Bullet_Sprite, Obj_ZOrder);
	bd.position = position;

	bulletBody = world->CreateBody(&bd);
	bulletBody->SetIsCh(false);

	bulletBody->setNewobj(true);
	b2CircleShape CircleShape;

	auto posi = Vec2(0, 0); // 타일을 월드에 추가한 뒤에 body를 놓을 위치 설정 body의 위치는 스프라이트가 올라간 위치를 기준으로 한다.
	float width = 32 / PTMRATIO;
	float height = 32 / PTMRATIO;

	auto fix = new FixtureDef();
	b2Fixture* SensorFixture;

	bulletBody->SetHp(100);
	bulletBody->SetGravityScale(0.1);
	bulletBody->SetType(b2_dynamicBody);

	CircleShape.m_radius = 0.3f;
	fix->fixture.shape = &CircleShape;
	fix->fixture.isSensor = true;
	fix->fixture.filter.categoryBits = 0x6000;
	fix->fixture.filter.maskBits = 0x000E;

	SensorFixture = bulletBody->CreateFixture(&fix->fixture);
	SensorFixture->SetUserData((void*)13);

	lb_Bulletlist.push_back(bulletBody);

	return Bullet_Sprite;
}

Sprite* TiledBodyCreator::CreateBullet(b2Vec2 position, b2World* world, Layer* layer, bool sprite) {
	b2BodyDef bd;
	Sprite* Bullet_Sprite = Sprite::create("n_Images/item/bullet.png");
	Bullet_Sprite->setAnchorPoint(Vec2(0, 0));
	bd.userData = Bullet_Sprite;

	layer->addChild(Bullet_Sprite, Obj_ZOrder);
	bd.position = position;

	bulletBody = world->CreateBody(&bd);
	bulletBody->SetIsCh(false);

	bulletBody->setNewobj(true);
	b2CircleShape CircleShape;

	auto posi = Vec2(0, 0);
	float width = 32 / PTMRATIO;
	float height = 32 / PTMRATIO;

	auto fix = new FixtureDef();
	b2Fixture* SensorFixture;

	bulletBody->SetHp(100);
	bulletBody->SetGravityScale(0.1);
	bulletBody->SetType(b2_dynamicBody);

	CircleShape.m_radius = 0.3f;
	fix->fixture.shape = &CircleShape;
	fix->fixture.isSensor = true;
	fix->fixture.filter.categoryBits = 0x6000;
	fix->fixture.filter.maskBits = 0x000E;

	SensorFixture = bulletBody->CreateFixture(&fix->fixture);
	SensorFixture->SetUserData((void*)13);

	lb_Bulletlist.push_back(bulletBody);

	return Bullet_Sprite;
}

b2Body* TiledBodyCreator::CreateBomb(b2Vec2 position, b2World* world, Layer* layer) {
	b2BodyDef bd;

	auto spritecache = SpriteFrameCache::getInstance();
	spritecache->addSpriteFramesWithFile("n_Images/item/bomb_.plist");

	Vector<SpriteFrame*> animFrames;
	char str[100];
	for (int i = 0; i < 3; i++) {
		sprintf(str, "bomb_%d.png", i);
		animFrames.pushBack(spritecache->getSpriteFrameByName(str));
	}

	EffectSprite* Bomb_Sprite = EffectSprite::createWithSpriteFrame(animFrames.front());
	Animation* animation = Animation::createWithSpriteFrames(animFrames, 1.0f);
	Animate* animate = Animate::create(animation);
	RepeatForever* rep = RepeatForever::create(animate);
	Bomb_Sprite->runAction(rep);
	Bomb_Sprite->setAnchorPoint(Vec2::ZERO);
	Bomb_Sprite->setEffect(m_LightEffect, "n_Images/item/bomb_n.png");
	Bomb_Sprite->setColor(m_LightEffect->getAmbientLightColor());
	layer->addChild(Bomb_Sprite, Obj_ZOrder);

	/*
	Texture2D* texture = Director::getInstance()->getTextureCache()->addImage("Images/Items/bomb.png");

	Animation* animation = Animation::create();
	animation->setDelayPerUnit(1.0);

	for (int i = 0; i < 3; i++) {
		animation->addSpriteFrameWithTexture(texture,
			Rect(i * 32, 0, 32, 34));
	}

	Sprite* Bomb_Sprite = Sprite::createWithTexture(texture, Rect(0, 0, 32, 34));
	Bomb_Sprite->setAnchorPoint(Vec2(0, 0));

	Animate* animate = Animate::create(animation);
	RepeatForever* rep = RepeatForever::create(animate);

	Bomb_Sprite->runAction(rep);

	layer->addChild(Bomb_Sprite, 3);*/

	bd.position = position;
	bd.userData = Bomb_Sprite;

	bombBody = world->CreateBody(&bd);
	bombBody->SetHp(100);
	bombBody->SetIsCh(false);

	bombBody->setNewobj(true);
	b2PolygonShape *rectshape = new b2PolygonShape();
	b2Vec2 vertices[4];
	int vindex = 4;

	auto posi = Vec2(0, 0); // 타일을 월드에 추가한 뒤에 body를 놓을 위치 설정 body의 위치는 스프라이트가 올라간 위치를 기준으로 한다.
	float width = 32 / PTMRATIO;
	float height = 32 / PTMRATIO;

	auto fix = new FixtureDef();
	b2Fixture* SensorFixture;

	vertices[0].x = posi.x + 0.0f;
	vertices[0].y = posi.y + 0.0f;

	vertices[1].x = posi.x + 0.0f;
	vertices[1].y = posi.y + height;

	vertices[2].x = posi.x + width;
	vertices[2].y = posi.y + height;

	vertices[3].x = posi.x + width;
	vertices[3].y = posi.y + 0.0f;

	bombBody->SetHp(1000);
	bombBody->SetType(b2_dynamicBody);

	rectshape->Set(vertices, vindex);
	fix->fixture.shape = rectshape;
	fix->fixture.isSensor = false;
	fix->fixture.filter.categoryBits = 0x3000;
	fix->fixture.filter.maskBits = 0x000F;

	SensorFixture = bombBody->CreateFixture(&fix->fixture);
	SensorFixture->SetUserData((void*)9);

	return bombBody;
}

b2Body* TiledBodyCreator::CreateObj(std::string obj_name, b2Vec2 position, b2World* world, Layer* layer) {
	b2Body* body;

	if (m_stageNumber == SHELTER || m_Blind) {
		if (obj_name == "Ladder") {
			body = WhatObj("n_Images/Item/longladder.png", "n_Images/Item/longladder_n.png", &lb_LLadderlist, position, world, layer, Size(32, 32), Size(0.3, -3), LLADDER_CATEGORY, 0x000E, 13);
			// 사다리는 사다리 몸체 body와 더 올라가는것을 물리적으로 막아주는 상한선 body가 붙어 쌍으로 생성된다.
			b2Body* Roofbody;
			b2BodyDef bodydef;
			bodydef.type = b2_dynamicBody;
			bodydef.position = body->GetPosition() + b2Vec2(0.5f, 5.5);

			b2PolygonShape polygonshape;
			polygonshape.SetAsBox(1.0, 0.1);

			b2FixtureDef Fixture;
			Fixture.isSensor = true;
			Fixture.shape = &polygonshape;
			Fixture.density = 1.0f;
			Fixture.friction = 0.0f;
			Fixture.restitution = 0;
			Fixture.filter.categoryBits = 0x7077;
			Fixture.filter.maskBits = 0xFFFF;

			Roofbody = m_world->CreateBody(&bodydef);
			Roofbody->CreateFixture(&Fixture);

			b2WeldJointDef weldJointDef;
			weldJointDef.Initialize(body, Roofbody, body->GetPosition());
			m_world->CreateJoint(&weldJointDef);
		}
		if (obj_name == "Gold") {
			body = WhatObj("n_Images/item/Gold.png", "n_Images/item/Gold_n.png", &lb_Goldlist, position, world, layer, Size(32, 32), Size(0.3, 0.3), GOLD_CATEGORY, 0x000E, 77);
		}
		if (obj_name == "Potion") {
			body = WhatObj("n_Images/item/potion.png", "n_Images/item/potion_n.png", &lb_Heartlist, position, world, layer, Size(32, 32), Size(0.3, 0.3), POTION_CATEGORY, 0x000E, 6);
		}
		if (obj_name == "Bomb") {
			body = WhatObj("n_Images/item/bomb.png", "n_Images/item/bomb_n.png", &lb_Bomblist, position, world, layer, Size(32, 32), Size(0.3, 0.3), BOMB_CATEGORY, 0x000E, 9);
		}
		if (obj_name == "Pickaxe") {
			body = WhatObj("n_Images/item/pickaxe.png", "n_Images/item/pickaxe_n.png", &lb_Pickaxelist, position, world, layer, Size(32, 32), Size(0.3, 0.3), PICKAXE_CATEGORY, 0x000E, 7);
		}
		if (obj_name == "Sword") {
			body = WhatObj("n_Images/item/sword.png", "n_Images/item/sword_n.png", &lb_Swordlist, position, world, layer, Size(32, 32), Size(0.3, 0.3), SWORD_CATEGORY, 0x000E, 11);
		}
		if (obj_name == "Gun") {
			body = WhatObj("n_Images/item/gun.png", "n_Images/item/gun_n.png", &lb_Gunlist, position, world, layer, Size(32, 32), Size(0.3, 0.3), GUN_CATEGORY, 0x000E, 12);
		}
		if (obj_name == "Ladder_item") {
			body = WhatObj("n_Images/item/Ladder.png", "n_Images/item/Ladder_n.png", &lb_Ladderlist, position, world, layer, Size(32, 32), Size(0.3, 0.3), LADDER_CATEGORY, 0x000E, 13);
		}
	}

	else {
		if (obj_name == "Ladder") {
			body = WhatObj("n_Images/Item/longladder.png", &lb_LLadderlist, position, world, layer, Size(32, 32), Size(0.3, -0.1), LLADDER_CATEGORY, 0x000E, 13);
			// 사다리는 사다리 몸체 body와 더 올라가는것을 물리적으로 막아주는 상한선 body가 붙어 쌍으로 생성된다.
			b2Body* Roofbody;
			b2BodyDef bodydef;
			bodydef.type = b2_dynamicBody;
			bodydef.position = body->GetPosition() + b2Vec2(0.5f, 5.5);

			b2PolygonShape polygonshape;
			polygonshape.SetAsBox(1.0, 0.1);

			b2FixtureDef Fixture;
			Fixture.isSensor = true;
			Fixture.shape = &polygonshape;
			Fixture.density = 1.0f;
			Fixture.friction = 0.0f;
			Fixture.restitution = 0;
			Fixture.filter.categoryBits = 0x7077;
			Fixture.filter.maskBits = 0xFFFF;

			Roofbody = m_world->CreateBody(&bodydef);
			Roofbody->CreateFixture(&Fixture);

			b2WeldJointDef weldJointDef;
			weldJointDef.Initialize(body, Roofbody, body->GetPosition());
			m_world->CreateJoint(&weldJointDef);
		}
		if (obj_name == "Gold") {
			body = WhatObj("n_Images/item/Gold.png", &lb_Goldlist, position, world, layer, Size(32, 32), Size(0.3, 0.3), GOLD_CATEGORY, 0x000E, 77);
		}
		if (obj_name == "Potion") {
			body = WhatObj("n_Images/item/potion.png", &lb_Heartlist, position, world, layer, Size(32, 32), Size(0.3, 0.3), POTION_CATEGORY, 0x000E, 6);
		}
		if (obj_name == "Bomb") {
			body = WhatObj("n_Images/item/bomb.png", &lb_Bomblist, position, world, layer, Size(32, 32), Size(0.3, 0.3), BOMB_CATEGORY, 0x000E, 9);
		}
		if (obj_name == "Pickaxe") {
			body = WhatObj("n_Images/item/pickaxe.png", &lb_Pickaxelist, position, world, layer, Size(32, 32), Size(0.3, 0.3), PICKAXE_CATEGORY, 0x000E, 7);
		}
		if (obj_name == "Sword") {
			body = WhatObj("n_Images/item/sword.png", &lb_Swordlist, position, world, layer, Size(32, 32), Size(0.3, 0.3), SWORD_CATEGORY, 0x000E, 11);
		}
		if (obj_name == "Gun") {
			body = WhatObj("n_Images/item/gun.png", &lb_Gunlist, position, world, layer, Size(32, 32), Size(0.3, 0.3), GUN_CATEGORY, 0x000E, 12);
		}
		if (obj_name == "Ladder_item") {
			body = WhatObj("n_Images/item/Ladder.png", &lb_Ladderlist, position, world, layer, Size(32, 32), Size(0.3, 0.3), LADDER_CATEGORY, 0x000E, 13);
		}
	}

	return body;
}

b2Body* TiledBodyCreator::WhatObj(std::string sPath, std::string sPath_n, std::list<b2Body*>* lb_list, b2Vec2 position, b2World* world, Layer* layer, Size _spsize, Size _objsize, INT16 _category, INT16 _maskbits, int setdata) {
	b2BodyDef bd;
	//Texture2D* texture = Director::getInstance()->getTextureCache()->addImage("Images/pickaxe.png");
	//Sprite* Pickaxe_Sprite = Sprite::createWithTexture(texture, Rect(0, 0, 40, 32));

	EffectSprite* Obj_Sprite = EffectSprite::create(sPath);
	Obj_Sprite->setAnchorPoint(Vec2(0, 0));
	Obj_Sprite->setColor(m_LightEffect->getAmbientLightColor());
	Obj_Sprite->setEffect(m_LightEffect, sPath_n);
	bd.userData = Obj_Sprite;

	layer->addChild(Obj_Sprite, Obj_ZOrder);
	bd.position = position;

	itemBody = world->CreateBody(&bd);
	itemBody->SetIsCh(false);

	itemBody->setNewobj(true);
	b2PolygonShape *rectshape = new b2PolygonShape();
	b2Vec2 vertices[4];
	int vindex = 4;

	auto posi = Vec2(0, 0); // 타일을 월드에 추가한 뒤에 body를 놓을 위치 설정 body의 위치는 스프라이트가 올라간 위치를 기준으로 한다.
	float width = 32 / PTMRATIO;
	float height = 32 / PTMRATIO;

	auto fix = new FixtureDef();
	b2Fixture* SensorFixture;

	vertices[0].x = posi.x + 0.0f;
	vertices[0].y = posi.y + 0.0f;

	vertices[1].x = posi.x + 0.0f;
	vertices[1].y = posi.y + (height - _objsize.height);

	vertices[2].x = posi.x + (width - _objsize.width);
	vertices[2].y = posi.y + (height - _objsize.height);

	vertices[3].x = posi.x + (width - _objsize.width);
	vertices[3].y = posi.y + 0.0f;

	itemBody->SetHp(1000);
	itemBody->SetType(b2_dynamicBody);

	rectshape->Set(vertices, vindex);
	fix->fixture.shape = rectshape;
	fix->fixture.isSensor = false;
	fix->fixture.filter.categoryBits = _category;
	fix->fixture.filter.maskBits = _maskbits;

	SensorFixture = itemBody->CreateFixture(&fix->fixture);
	SensorFixture->SetUserData((void*)setdata);

	lb_list->push_back(itemBody);

	return itemBody;
}

b2Body* TiledBodyCreator::WhatObj(std::string sPath, std::list<b2Body*>* lb_list, b2Vec2 position, b2World* world, Layer* layer, Size _spsize, Size _objsize, INT16 _category, INT16 _maskbits, int setdata) {
	b2BodyDef bd;
	//Texture2D* texture = Director::getInstance()->getTextureCache()->addImage("Images/pickaxe.png");
	//Sprite* Pickaxe_Sprite = Sprite::createWithTexture(texture, Rect(0, 0, 40, 32));

	Sprite* Obj_Sprite = Sprite::create(sPath);
	Obj_Sprite->setAnchorPoint(Vec2(0, 0));
	bd.userData = (Sprite*)Obj_Sprite;

	layer->addChild(Obj_Sprite, Obj_ZOrder);
	bd.position = position;

	itemBody = world->CreateBody(&bd);
	itemBody->SetIsCh(false);

	itemBody->setNewobj(true);
	b2PolygonShape *rectshape = new b2PolygonShape();
	b2Vec2 vertices[4];
	int vindex = 4;

	auto posi = Vec2(0, 0); // 타일을 월드에 추가한 뒤에 body를 놓을 위치 설정 body의 위치는 스프라이트가 올라간 위치를 기준으로 한다.
	float width = 32 / PTMRATIO;
	float height = 32 / PTMRATIO;

	auto fix = new FixtureDef();
	b2Fixture* SensorFixture;

	vertices[0].x = posi.x + 0.0f;
	vertices[0].y = posi.y + 0.0f;

	vertices[1].x = posi.x + 0.0f;
	vertices[1].y = posi.y + (height - _objsize.height);

	vertices[2].x = posi.x + (width - _objsize.width);
	vertices[2].y = posi.y + (height - _objsize.height);

	vertices[3].x = posi.x + (width - _objsize.width);
	vertices[3].y = posi.y + 0.0f;

	itemBody->SetHp(1000);
	itemBody->SetType(b2_dynamicBody);

	rectshape->Set(vertices, vindex);
	fix->fixture.shape = rectshape;
	fix->fixture.isSensor = false;
	fix->fixture.filter.categoryBits = _category;
	fix->fixture.filter.maskBits = _maskbits;

	SensorFixture = itemBody->CreateFixture(&fix->fixture);
	SensorFixture->SetUserData((void*)setdata);

	lb_list->push_back(itemBody);

	return itemBody;
}



LightEffect* TiledBodyCreator::SpriteChange(Layer* layer, b2World* world) {
	m_layer = layer;

	m_LightEffect = LightEffect::create();
	m_LightEffect->retain();

	m_LightEffect->setLightPos(Vec3(200, 150, 10));
	m_LightEffect->setLightColor(Color3B(255,183,32));
	m_LightEffect->setLightCutoffRadius(130);
	m_LightEffect->setLightHalfRadius(0.8f);
	m_LightEffect->setBrightness(4.0f);
	m_LightEffect->setAmbientLightColor(Color3B(0, 0, 0));

	for (b2Body* b = world->GetBodyList(); b; b = b->GetNext()) {
		if (b->GetUserData() != nullptr) {
			INT16 category = b->GetFixtureList()->GetFilterData().categoryBits;

			if (category == BLOCK_CATEGORY) {
				
				if (m_stageNumber >= 0 && m_stageNumber <= STAGE_1_3 || m_stageNumber == SHELTER) {
					std::string blockPath = "n_Images/block.png";
					std::string blockPath_n = "n_Images/block_n.png";
					setEffectSprite(b, blockPath, blockPath_n, "Ground");
				}
				else if (m_stageNumber >= STAGE_2_1 && m_stageNumber <= STAGE_2_3) {
					std::string blockPath = "n_Images/block/IceBlock.png";
					std::string blockPath_n = "n_Images/block/IceBlock_n.png";
					setEffectSprite(b, blockPath, blockPath_n, "Ground");
				}

			}
			else if(category == GBLOCK_CATEGORY){

				if (m_stageNumber >= 0 && m_stageNumber <= STAGE_1_3 || m_stageNumber == SHELTER) {
					std::string blockPath = "n_Images/block/gold_block_1.png";
					std::string blockPath_n = "n_Images/block/gold_block_1_n.png";
					setEffectSprite(b, blockPath, blockPath_n, "Ground");
				}
				else if (m_stageNumber >= STAGE_2_1 && m_stageNumber <= STAGE_2_3) {
					std::string blockPath = "n_Images/block/Ice_GoldBlock.png";
					std::string blockPath_n = "n_Images/block/Ice_GoldBlock_n.png";
					setEffectSprite(b, blockPath, blockPath_n, "Ground");
				}

			}
			else if (category == HARDBLOCK_CATEGORY) {
				if (m_stageNumber >= 0 && m_stageNumber <= STAGE_1_3 || m_stageNumber == SHELTER) {
					std::string blockPath = "n_Images/block/hard_block_1.png";
					std::string blockPath_n = "n_Images/block/hard_block_1_n.png";
					setEffectSprite(b, blockPath, blockPath_n, "Ground");
				}
				else if (m_stageNumber >= STAGE_2_1 && m_stageNumber <= STAGE_2_3) {
					std::string blockPath = "n_Images/block/Ice_HardBlock.png";
					std::string blockPath_n = "n_Images/block/Ice_HardBlock_n.png";
					setEffectSprite(b, blockPath, blockPath_n, "Ground");
				}

			}
			else if (category == BLADEBLOCK_CATEGORY) {
				if (m_stageNumber >= 0 && m_stageNumber <= STAGE_1_3 || m_stageNumber == SHELTER) {
					std::string blockPath = "n_Images/obstacle/FallingBlade_BaseBlock.png";
					std::string blockPath_n = "n_Images/obstacle/FallingBlade_BaseBlock_n.png";
					setEffectSprite(b, blockPath, blockPath_n, "Ground");
				}
				else if (m_stageNumber >= STAGE_2_1 && m_stageNumber <= STAGE_2_3) {
					std::string blockPath = "n_Images/obstacle/FallingBlade_BaseBlock.png";
					std::string blockPath_n = "n_Images/obstacle/FallingBlade_BaseBlock_n.png";
					setEffectSprite(b, blockPath, blockPath_n, "Ground");
				}
			}
			else if (category == LGUNTRAP_CATEGORY) {
				if (m_stageNumber >= 0 && m_stageNumber <= STAGE_1_3 || m_stageNumber == SHELTER) {
					std::string blockPath = "n_Images/obstacle/GunTrap_Fire_0.png";
					std::string blockPath_n = "n_Images/obstacle/GunTrap_Fire_0_n.png";
					setEffectSprite(b, blockPath, blockPath_n, "Ground");
				}
				else if (m_stageNumber >= STAGE_2_1 && m_stageNumber <= STAGE_2_3) {
					std::string blockPath = "n_Images/obstacle/GunTrap_Fire_0.png";
					std::string blockPath_n = "n_Images/obstacle/GunTrap_Fire_0_n.png";
					setEffectSprite(b, blockPath, blockPath_n, "Ground");
				}
			}
			else if (category == RGUNTRAP_CATEGORY) {
				if (m_stageNumber >= 0 && m_stageNumber <= STAGE_1_3 || m_stageNumber == SHELTER) {
					std::string blockPath = "n_Images/obstacle/GunTrap_Fire_0.png";
					std::string blockPath_n = "n_Images/obstacle/GunTrap_Fire_0_n.png";
					setEffectSprite(b, blockPath, blockPath_n, "Ground");
				}
				else if (m_stageNumber >= STAGE_2_1 && m_stageNumber <= STAGE_2_3) {
					std::string blockPath = "n_Images/obstacle/GunTrap_Fire_0.png";
					std::string blockPath_n = "n_Images/obstacle/GunTrap_Fire_0_n.png";
					setEffectSprite(b, blockPath, blockPath_n, "Ground");
				}
			}
			else if (category == POTION_CATEGORY) {
				std::string potionPath = "n_Images/item/potion.png";
				std::string potionPath_n = "n_Images/item/potion_n.png";
				setEffectSprite(b, potionPath, potionPath_n, "Ground");
			}
			else if (category == OBSTACLE_CATEGORY) {
				std::string obstaclePath = "n_Images/obstacle/obstacle.png";
				std::string obstaclePath_n = "n_Images/obstacle/obstacle_n.png";
				setEffectSprite(b, obstaclePath, obstaclePath_n, "Ground");
			}
			else if (category == PICKAXE_CATEGORY) {
				std::string pickaxePath = "n_Images/item/pickaxe.png";
				std::string pickaxePath_n = "n_Images/item/pickaxe_n.png";
				setEffectSprite(b, pickaxePath, pickaxePath_n, "Item");
			}
			else if (category == BOMB_CATEGORY) {
				std::string bombPath = "n_Images/item/bomb.png";
				std::string bombPath_n = "n_Images/item/bomb_n.png";
				setEffectSprite(b, bombPath, bombPath_n, "Item");
			}
			else if (category == SWORD_CATEGORY) {
				std::string swordPath = "n_Images/item/sword.png";
				std::string swordPath_n = "n_Images/item/sword_n.png";
				setEffectSprite(b, swordPath, swordPath_n, "Item");
			}
			else if (category == GUN_CATEGORY) {
				std::string gunPath = "n_Images/item/gun.png";
				std::string gunPath_n = "n_Images/item/gun_n.png";
				setEffectSprite(b, gunPath, gunPath_n, "Item");
			}
			else if (category == BULLET_CATEGORY) {
				std::string bulletPath = "n_Images/item/bullet.png";
				std::string bulletPath_n = "n_Images/item/bullet_n.png";
				setEffectSprite(b, bulletPath, bulletPath_n, "Item");
			}
			else if (category == LADDER_CATEGORY) {
				std::string ladderPath = "n_Images/item/ladder.png";
				std::string ladderPath_n = "n_Images/item/ladder_n.png";
				setEffectSprite(b, ladderPath, ladderPath_n, "Item");
			}
			else if (category == SHOP_CATEGORY) {
				std::string ladderPath = "n_Images/npc/shopman.png";
				std::string ladderPath_n = "n_Images/npc/shopman_n.png";
				setEffectSprite(b, ladderPath, ladderPath_n, "Ground");
				ShopBody = b;
			}
			else if (category == REDKEY_CATEGORY) {
				std::string RedKeyPath = "n_Images/item/Red_Key.png";
				std::string RedKeyPath_n = "n_Images/item/Red_Key_n.png";
				setEffectSprite(b, RedKeyPath, RedKeyPath_n, "Item");
			}
			else if (category == BLUEKEY_CATEGORY) {
				std::string BlueKeyPath = "n_Images/item/Blue_Key.png";
				std::string BlueKeyPath_n = "n_Images/item/Blue_Key_n.png";
				setEffectSprite(b, BlueKeyPath, BlueKeyPath_n, "Item");
			}
			else if (category == GREENKEY_CATEGORY) {
				std::string GreenKeyPath = "n_Images/item/Green_Key.png";
				std::string GreenKeyPath_n = "n_Images/item/Green_Key_n.png";
				setEffectSprite(b, GreenKeyPath, GreenKeyPath_n, "Item");
			}
			else if (category == REDCHEST_CATEGORY) {
				std::string RedChestPath = "n_Images/item/Treasure_chest_Red.png";
				std::string RedChestPath_n = "n_Images/item/Treasure_chest_Red_n.png";
				setEffectSprite(b, RedChestPath, RedChestPath_n, "Treasure");
			}
			else if (category == BLUECHEST_CATEGORY) {
				std::string BlueChestPath = "n_Images/item/Treasure_chest_Blue.png";
				std::string BlueChestPath_n = "n_Images/item/Treasure_chest_Blue_n.png";
				setEffectSprite(b, BlueChestPath, BlueChestPath_n, "Treasure");
			}
			else if (category == GREENCHEST_CATEGORY) {
				std::string GreenChestPath = "n_Images/item/Treasure_chest_Green.png";
				std::string GreenChestPath_n = "n_Images/item/Treasure_chest_Green_n.png";
				setEffectSprite(b, GreenChestPath, GreenChestPath_n, "Treasure");
			}
			else {
 				log("map inits be out of category %x", category);
			}
		}
	}
	//m_LightEffect->autorelease();
	//m_LightEffect->release();
	if (_heapchk() != _HEAPOK) {
		DebugBreak();
	}
	return m_LightEffect;
}

void TiledBodyCreator::UserDataChange(Layer* layer, b2World* world) {
	for (b2Body* b = world->GetBodyList(); b; b = b->GetNext()) {
		if (b->GetUserData() != nullptr) {
			INT16 category = b->GetFixtureList()->GetFilterData().categoryBits;
			Sprite* removeSp = (Sprite*)b->GetUserData();
			
			if (category == BLOCK_CATEGORY) {
				if (m_stageNumber >= 0 && m_stageNumber <= STAGE_1_3) {
					collisionGround->removeChild(removeSp, true);
					std::string blockPath = "n_Images/block.png";
					Sprite* SpData = Sprite::create(blockPath);
					SpData->setAnchorPoint(Vec2::ZERO);
					layer->addChild(SpData, Obj_ZOrder);
					b->SetUserData(SpData);
				}
				else if (m_stageNumber >= STAGE_2_1 && m_stageNumber <= STAGE_2_3) {
					collisionGround->removeChild(removeSp, true);
					std::string blockPath = "n_Images/block/IceBlock.png";
					Sprite* SpData = Sprite::create(blockPath);
					SpData->setAnchorPoint(Vec2::ZERO);
					layer->addChild(SpData, Obj_ZOrder);
					b->SetUserData(SpData);
				}
			}
			else if (category == GBLOCK_CATEGORY) {

				if (m_stageNumber >= 0 && m_stageNumber <= STAGE_1_3) {
					collisionGround->removeChild(removeSp, true);
					std::string blockPath = "n_Images/block/gold_block_1.png";
					Sprite* SpData = Sprite::create(blockPath);
					SpData->setAnchorPoint(Vec2::ZERO);
					layer->addChild(SpData, Obj_ZOrder);
					b->SetUserData(SpData);
					
				}
				else if (m_stageNumber >= STAGE_2_1 && m_stageNumber <= STAGE_2_3) {
					collisionGround->removeChild(removeSp, true);
					std::string blockPath = "n_Images/block/Ice_GoldBlock.png";
					Sprite* SpData = Sprite::create(blockPath);
					SpData->setAnchorPoint(Vec2::ZERO);
					layer->addChild(SpData, Obj_ZOrder);
					b->SetUserData(SpData);
				}

			}
			else if (category == HARDBLOCK_CATEGORY) {
				if (m_stageNumber >= 0 && m_stageNumber <= STAGE_1_3) {
					collisionGround->removeChild(removeSp, true);
					std::string blockPath = "n_Images/block/hard_block_1.png";
					Sprite* SpData = Sprite::create(blockPath);
					SpData->setAnchorPoint(Vec2::ZERO);
					layer->addChild(SpData, Obj_ZOrder);
					b->SetUserData(SpData);
					
				}
				else if (m_stageNumber >= STAGE_2_1 && m_stageNumber <= STAGE_2_3) {
					collisionGround->removeChild(removeSp, true);
					std::string blockPath = "n_Images/block/Ice_HardBlock.png";
					Sprite* SpData = Sprite::create(blockPath);
					SpData->setAnchorPoint(Vec2::ZERO);
					layer->addChild(SpData, Obj_ZOrder);
					b->SetUserData(SpData);
					
				}

			}
			else if (category == BLADEBLOCK_CATEGORY) {
				if (m_stageNumber >= 0 && m_stageNumber <= STAGE_1_3) {
					collisionGround->removeChild(removeSp, true);
					std::string blockPath = "n_Images/block/FallingBlade_BaseBlock.png";
					Sprite* SpData = Sprite::create(blockPath);
					SpData->setAnchorPoint(Vec2::ZERO);
					layer->addChild(SpData, Obj_ZOrder);
					b->SetUserData(SpData);

				}
				else if (m_stageNumber >= STAGE_2_1 && m_stageNumber <= STAGE_2_3) {
					collisionGround->removeChild(removeSp, true);
					std::string blockPath = "n_Images/block/FallingBlade_BaseBlock.png";
					Sprite* SpData = Sprite::create(blockPath);
					SpData->setAnchorPoint(Vec2::ZERO);
					layer->addChild(SpData, Obj_ZOrder);
					b->SetUserData(SpData);

				}
			}
			else if (category == LGUNTRAP_CATEGORY) {
				if (m_stageNumber >= 0 && m_stageNumber <= STAGE_1_3) {
					collisionGround->removeChild(removeSp, true);
					std::string blockPath = "n_Images/block/LGunTrap_BaseBlock.png";
					Sprite* SpData = Sprite::create(blockPath);
					SpData->setAnchorPoint(Vec2(0.5, 0));
					SpData->setFlippedX(true);
					layer->addChild(SpData, Obj_ZOrder);
					b->SetUserData(SpData);
				}
				else if (m_stageNumber >= STAGE_2_1 && m_stageNumber <= STAGE_2_3) {
					collisionGround->removeChild(removeSp, true);
					std::string blockPath = "n_Images/block/LGunTrap_BaseBlock.png";
					Sprite* SpData = Sprite::create(blockPath);
					SpData->setAnchorPoint(Vec2::ZERO);
					SpData->setFlippedX(true);
					layer->addChild(SpData, Obj_ZOrder);
					b->SetUserData(SpData);

				}
			}
			else if (category == RGUNTRAP_CATEGORY) {
				if (m_stageNumber >= 0 && m_stageNumber <= STAGE_1_3) {
					collisionGround->removeChild(removeSp, true);
					std::string blockPath = "n_Images/block/GunTrap_BaseBlock.png";
					Sprite* SpData = Sprite::create(blockPath);
					SpData->setAnchorPoint(Vec2::ZERO);
					layer->addChild(SpData, Obj_ZOrder);
					b->SetUserData(SpData);

				}
				else if (m_stageNumber >= STAGE_2_1 && m_stageNumber <= STAGE_2_3) {
					collisionGround->removeChild(removeSp, true);
					std::string blockPath = "n_Images/block/GunTrap_BaseBlock.png";
					Sprite* SpData = Sprite::create(blockPath);
					SpData->setAnchorPoint(Vec2::ZERO);
					layer->addChild(SpData, Obj_ZOrder);
					b->SetUserData(SpData);
				}
			}
			else if (category == POTION_CATEGORY) {
				collisionGround->removeChild(removeSp, true);
				std::string potionPath = "n_Images/item/potion.png";
				Sprite* SpData = Sprite::create(potionPath);
				SpData->setAnchorPoint(Vec2::ZERO);
				layer->addChild(SpData, Obj_ZOrder);
				b->SetUserData(SpData);
			}
			else if (category == OBSTACLE_CATEGORY) {
				collisionGround->removeChild(removeSp, true);
				std::string obstaclePath = "n_Images/obstacle/obstacle.png";
				Sprite* SpData = Sprite::create(obstaclePath);
				SpData->setAnchorPoint(Vec2::ZERO);
				layer->addChild(SpData, Obj_ZOrder);
				b->SetUserData(SpData);
			}
			else if (category == PICKAXE_CATEGORY) {
				collisionItem->removeChild(removeSp, true);
				std::string pickaxePath = "n_Images/item/pickaxe.png";
				Sprite* SpData = Sprite::create(pickaxePath);
				SpData->setAnchorPoint(Vec2::ZERO);
				layer->addChild(SpData, Obj_ZOrder);
				b->SetUserData(SpData);
			}
			else if (category == BOMB_CATEGORY) {
				collisionItem->removeChild(removeSp, true);
				std::string bombPath = "n_Images/item/bomb.png";
				Sprite* SpData = Sprite::create(bombPath);
				SpData->setAnchorPoint(Vec2::ZERO);
				layer->addChild(SpData, Obj_ZOrder);
				b->SetUserData(SpData);
			}
			else if (category == SWORD_CATEGORY) {
				collisionItem->removeChild(removeSp, true);
				std::string swordPath = "n_Images/item/sword.png";
				Sprite* SpData = Sprite::create(swordPath);
				SpData->setAnchorPoint(Vec2::ZERO);
				layer->addChild(SpData, Obj_ZOrder);
				b->SetUserData(SpData);
			}
			else if (category == GUN_CATEGORY) {
				collisionItem->removeChild(removeSp, true);
				std::string gunPath = "n_Images/item/gun.png";
				Sprite* SpData = Sprite::create(gunPath);
				SpData->setAnchorPoint(Vec2::ZERO);
				layer->addChild(SpData, Obj_ZOrder);
				b->SetUserData(SpData);
			}
			else if (category == BULLET_CATEGORY) {
				collisionItem->removeChild(removeSp, true);
				std::string bulletPath = "n_Images/item/bullet.png";
				Sprite* SpData = Sprite::create(bulletPath);
				SpData->setAnchorPoint(Vec2::ZERO);
				layer->addChild(SpData, Obj_ZOrder);
				b->SetUserData(SpData);
			}
			else if (category == LADDER_CATEGORY) {
				collisionItem->removeChild(removeSp, true);
				std::string ladderPath = "n_Images/item/ladder.png";
				Sprite* SpData = Sprite::create(ladderPath);
				SpData->setAnchorPoint(Vec2::ZERO);
				layer->addChild(SpData, Obj_ZOrder);
				b->SetUserData(SpData);
			}
			else if (category == SHOP_CATEGORY) {
				collisionGround->removeChild(removeSp, true);
				std::string shopPath = "n_Images/npc/shopman.png";
				Sprite* SpData = Sprite::create(shopPath);
				SpData->setAnchorPoint(Vec2::ZERO);
				layer->addChild(SpData, Obj_ZOrder);
				b->SetUserData(SpData);
				ShopBody = b;
			}
			else if (category == REDKEY_CATEGORY) {
				collisionItem->removeChild(removeSp, true);
				std::string RedKeyPath = "n_Images/item/Red_Key.png";
				Sprite* SpData = Sprite::create(RedKeyPath);
				SpData->setAnchorPoint(Vec2::ZERO);
				layer->addChild(SpData, Obj_ZOrder);
				b->SetUserData(SpData);
			}
			else if (category == BLUEKEY_CATEGORY) {
				collisionItem->removeChild(removeSp, true);
				std::string BlueKeyPath = "n_Images/item/Blue_Key.png";
				Sprite* SpData = Sprite::create(BlueKeyPath);
				SpData->setAnchorPoint(Vec2::ZERO);
				layer->addChild(SpData, Obj_ZOrder);
				b->SetUserData(SpData);
			}
			else if (category == GREENKEY_CATEGORY) {
				collisionItem->removeChild(removeSp, true);
				std::string GreenKeyPath = "n_Images/item/Green_Key.png";
				Sprite* SpData = Sprite::create(GreenKeyPath);
				SpData->setAnchorPoint(Vec2::ZERO);
				layer->addChild(SpData, Obj_ZOrder);
				b->SetUserData(SpData);
			}
			else if (category == REDCHEST_CATEGORY) {
				collisionTreasureChest->removeChild(removeSp, true);
				std::string RedChestPath = "n_Images/item/Treasure_chest_Red.png";
				Sprite* SpData = Sprite::create(RedChestPath);
				SpData->setAnchorPoint(Vec2::ZERO);
				layer->addChild(SpData, Obj_ZOrder);
				b->SetUserData(SpData);
			}
			else if (category == BLUECHEST_CATEGORY) {
				collisionTreasureChest->removeChild(removeSp, true);
				std::string BlueChestPath = "n_Images/item/Treasure_chest_Blue.png";
				Sprite* SpData = Sprite::create(BlueChestPath);
				SpData->setAnchorPoint(Vec2::ZERO);
				layer->addChild(SpData, Obj_ZOrder);
				b->SetUserData(SpData);
			}
			else if (category == GREENCHEST_CATEGORY) {
				collisionTreasureChest->removeChild(removeSp, true);
				std::string GreenChestPath = "n_Images/item/Treasure_chest_Green.png";
				Sprite* SpData = Sprite::create(GreenChestPath);
				SpData->setAnchorPoint(Vec2::ZERO);
				layer->addChild(SpData, Obj_ZOrder);
				b->SetUserData(SpData);
			}
			else {
				log("map inits be out of category %x", category);
			}
		}
	}
}


void TiledBodyCreator::setEffectSprite(b2Body* body, std::string& sPath, std::string& sPath_n, std::string layertype) {
	INT16 category = body->GetFixtureList()->GetFilterData().categoryBits;
	if (m_Blind && category == LGUNTRAP_CATEGORY || category == RGUNTRAP_CATEGORY || category == BLADEBLOCK_CATEGORY || layertype == "Item" || layertype == "Treasure") {
		EffectSprite* m_EffectSprite = EffectSprite::create(sPath);
		m_EffectSprite->setAnchorPoint(Vec2(0, 0));
		if (category == LGUNTRAP_CATEGORY) {
			m_EffectSprite->setAnchorPoint(Vec2(0.5, 0));
			m_EffectSprite->setFlippedX(true);
		}
		m_EffectSprite->setEffect(m_LightEffect, sPath_n);
		m_EffectSprite->setColor(m_LightEffect->getAmbientLightColor());
		m_layer->addChild(m_EffectSprite, Obj_ZOrder);
		SpriteTrade(m_layer, body, m_EffectSprite, layertype);
	}
	else {
		SpriteTrade(m_layer, body, nullptr, layertype);
	}
}

void TiledBodyCreator::SpriteTrade(Layer* layer, b2Body* body, EffectSprite* _EffectSprite, std::string layertype) {
	Sprite* beforeSp = static_cast<Sprite*>(body->GetUserData());
	INT16 category = body->GetFixtureList()->GetFilterData().categoryBits;
	if (layertype == "Ground") {
		collisionGround->removeChild(beforeSp, true);

		/* Blind Map은 특정(현재 GUNTRAP OR FALLINGBLADE) Sprite를 제외한 
		모든 body Userdata를 nullptr로 대체해 아무것도 없는 투명한 상태를 유지한다.
		설계되어있는 맵의 Sprite를 통째로 가져오면 body는 각 위치에 존재하므로 
		sprite는 world (0, 0)에 고정되어 Map body를 통째로 좌표를 일일히 찍어가며 구현해 줄 필요가 없다.*/
		if (m_Blind && category != LGUNTRAP_CATEGORY && category != RGUNTRAP_CATEGORY && category != BLADEBLOCK_CATEGORY) {
			body->SetUserData(nullptr);
		}
		else {
			body->SetUserData(_EffectSprite);
		}
	}
	else if (layertype == "Item") {
		collisionItem->removeChild(beforeSp, true);
		body->SetUserData(_EffectSprite);
	}
	else if (layertype == "Treasure") {
		collisionTreasureChest->removeChild(beforeSp, true);
		body->SetUserData(_EffectSprite);
	}
	else if (layertype == "wlayer") {
		EffectSprite* beforeESp = static_cast<EffectSprite*>(body->GetUserData());
		layer->removeChild(beforeESp, true);
		body->SetUserData(_EffectSprite);
	}
}

void TiledBodyCreator::setEffectAnimationSprite(b2Body* body, std::string& sPath, std::string& sPath_n, int nSheet_mx, std::string layertype) {
	auto spritecache = SpriteFrameCache::getInstance();
	spritecache->addSpriteFramesWithFile(sPath);

	Vector<SpriteFrame*> animFrames;
	char str[100];
	for (int i = 0; i < nSheet_mx; i++) {
		sprintf(str, "meltblock_%d.png", i);
		animFrames.pushBack(spritecache->getSpriteFrameByName(str));
	}

	EffectSprite* SpriteData = EffectSprite::createWithSpriteFrame(animFrames.front());
	Animation* animation = Animation::createWithSpriteFrames(animFrames, 0.2f);
	Animate* animate = Animate::create(animation);
	Repeat* rep = Repeat::create(animate, 1.4);
	//RepeatForever* rep = RepeatForever::create(animate);
	SpriteData->runAction(rep);
	SpriteData->setColor(m_LightEffect->getAmbientLightColor());
	SpriteData->setEffect(m_LightEffect, sPath_n);
	SpriteData->setAnchorPoint(Vec2(0, 0));
	SpriteData->setPosition(Vec2(100, 100));
	m_layer->removeChild((EffectSprite*)body->GetUserData());
	m_layer->addChild(SpriteData, 5);
	
	body->SetUserData(SpriteData);
	//SpriteTrade(m_layer, body, SpriteData, "wlayer");
}


FixtureDef* TiledBodyCreator::createFixture(cocos2d::ValueMap object)
{
	int fixtureType = RECT_FIXTURE;
	for(auto propObj : object)
	{
		if(propObj.first == "points") {
			fixtureType = POLYGON_FIXTURE;
		} else if(propObj.first == "polylinePoints") {
			fixtureType = POLYLINE_FIXTURE;
		}
	}
	if(object["type"].asString() == "Circle") {
		fixtureType = CIRCLE_FIXTURE;
	}


	if(fixtureType == POLYGON_FIXTURE) {
		return createPolygon(object);
	} else if(fixtureType == POLYLINE_FIXTURE) {
		return createPolyline(object);
	} else if(fixtureType == CIRCLE_FIXTURE) {
		return createCircle(object);
	} else if(fixtureType == RECT_FIXTURE) {
		return createRect(object);
	}
}

FixtureDef* TiledBodyCreator::createPolygon(ValueMap object)
{
	ValueVector pointsVector = object["points"].asValueVector();
	auto position = Point(object["x"].asFloat() / PTMRATIO, object["y"].asFloat() / PTMRATIO);

	b2PolygonShape *polyshape = new b2PolygonShape();
	b2Vec2 vertices[b2_maxPolygonVertices];
	int vindex = 0;

	if(pointsVector.size() > b2_maxPolygonVertices) {
		CCLOG("Skipping TMX polygon at x=%d,y=%d for exceeding %d vertices", object["x"].asInt(), object["y"].asInt(), b2_maxPolygonVertices);
		return NULL;
	}

	auto fix = new FixtureDef();

	for(Value point : pointsVector) {
		vertices[vindex].x = (point.asValueMap()["x"].asFloat() / PTMRATIO + position.x);
        vertices[vindex].y = (-point.asValueMap()["y"].asFloat() / PTMRATIO + position.y);
		vindex++;
	}

	polyshape->Set(vertices, vindex);
	fix->fixture.shape = polyshape;

	return fix;
}


FixtureDef* TiledBodyCreator::createPolyline(ValueMap object)
{
	ValueVector pointsVector = object["polylinePoints"].asValueVector();
	auto position = Point(object["x"].asFloat() / PTMRATIO, object["y"].asFloat() / PTMRATIO);

	b2ChainShape *polylineshape = new b2ChainShape();
	
	int verticesCapacity=32;
	b2Vec2* vertices = (b2Vec2*)calloc(verticesCapacity, sizeof(b2Vec2));

	int vindex = 0;

	auto fix = new FixtureDef();

	for(Value point : pointsVector) {
		if(vindex>=verticesCapacity)
		{
			verticesCapacity+=32;
			vertices = (b2Vec2*)realloc(vertices, verticesCapacity*sizeof(b2Vec2));
		}
		vertices[vindex].x = (point.asValueMap()["x"].asFloat() / PTMRATIO + position.x);
        vertices[vindex].y = (-point.asValueMap()["y"].asFloat() / PTMRATIO + position.y);
		vindex++;
	}

	polylineshape->CreateChain(vertices, vindex);
	fix->fixture.shape = polylineshape;

	return fix;
}

FixtureDef* TiledBodyCreator::createCircle(ValueMap object)
{
	auto position = Point(object["x"].asFloat() / PTMRATIO, object["y"].asFloat() / PTMRATIO);
	float radius = object["width"].asFloat()/2 / PTMRATIO;

	b2CircleShape *circleshape = new b2CircleShape();
	circleshape->m_radius = radius;
	circleshape->m_p.Set(position.x + radius, position.y + radius);

	auto fix = new FixtureDef();
	fix->fixture.shape = circleshape;

	return fix;
}

FixtureDef* TiledBodyCreator::createRect(ValueMap object)
{
	auto position = Vec2(0,0);
	float width = object["width"].asFloat() / PTMRATIO;
	float height = object["height"].asFloat() / PTMRATIO;

	b2PolygonShape *rectshape = new b2PolygonShape();
	b2Vec2 vertices[4];
	int vindex = 4;

	vertices[0].x = position.x + 0.0f;
	vertices[0].y = position.y + 0.0f;

	vertices[1].x = position.x + 0.0f;
	vertices[1].y = position.y + height;

	vertices[2].x = position.x + width;
	vertices[2].y = position.y + height;

	vertices[3].x = position.x + width;
	vertices[3].y = position.y + 0.0f;

	auto fix = new FixtureDef();
	rectshape->Set(vertices, vindex);
	fix->fixture.shape = rectshape;

	return fix;
}