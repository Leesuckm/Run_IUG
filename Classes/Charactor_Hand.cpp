#include "CharactorCreator.h"
#include "Charactor_Hand.h"
#include "EffectSprite.h"
#include "LightEffect.h"

extern bool g_bViewing;

Charactor_Hand::Charactor_Hand(Charactor_Data _charactor_data) {
	m_Charactor_data = _charactor_data;
	m_HandState = TwoHand_Off;

	m_Pickup_Lefthand = new ESorSP;
}

Charactor_Hand::~Charactor_Hand() {

}

void Charactor_Hand::Holeinone_LeftHand(std::string sPath,std::string item, b2Body* Charactorbody, _moveState state, Layer* layer, b2World* _world, LightEffect* _effect) {
	m_world = _world;
	m_Layer = layer;
	m_effect = _effect;
	std::string sPath_n;
	m_HandState = LeftHand_On;

	m_Charactorbody = Charactorbody;

	auto spritecache = SpriteFrameCache::getInstance();
	spritecache->addSpriteFramesWithFile(sPath);

	Vector<SpriteFrame*> animFrames;
	char str[100];
	for (int i = 0; i < 4; i++) {
		if (item == "Pickaxe") {
			sprintf(str, "pickaxe_idle_%d.png", i);
			animFrames.pushBack(spritecache->getSpriteFrameByName(str));
			sPath_n = "Images/sprites/pickaxe_idle/fox_pickaxe_idle_n.png";
		}
		else if (item == "Bomb") {
			sprintf(str, "throw_bomb.png");
			animFrames.pushBack(spritecache->getSpriteFrameByName(str));
			sPath_n = "Images/sprites/bomb_throw/fox_bomb_throw_n.png";
			break;
		}
		else if (item == "Sword") {
			sprintf(str, "sword_idle_%d.png", i);
			animFrames.pushBack(spritecache->getSpriteFrameByName(str));
			sPath_n = "Images/sprites/sword_idle/fox_sword_idle_n.png";
		}
		else if (item == "Gun") {
			sprintf(str, "gun_idle_%d.png", i);
			animFrames.pushBack(spritecache->getSpriteFrameByName(str));
			sPath_n = "Images/sprites/gun_idle/fox_gun_idle_n.png";
		}
		
	}

	
	Animation* animation = Animation::createWithSpriteFrames(animFrames, 0.1);

	//m_sPlayer->setAnchorPoint(Vec2(0.5, 0.5));
	Animate* animate = Animate::create(animation);
	RepeatForever* rep = RepeatForever::create(animate);
	if (m_Charactor_data.StageNumber == SHELTER || m_Charactor_data.bStage_blind) {
		m_Pickup_Lefthand->ESpriteData = EffectSprite::createWithSpriteFrame(animFrames.front());
		m_Pickup_Lefthand->ESpriteData->runAction(rep);
		m_Pickup_Lefthand->ESpriteData->setColor(_effect->getAmbientLightColor());
		m_Pickup_Lefthand->ESpriteData->setEffect(_effect, sPath_n);
		layer->addChild(m_Pickup_Lefthand->ESpriteData, 3);

		m_Charactorbody->SetUserData(m_Pickup_Lefthand->ESpriteData);
	}
	else {
		m_Pickup_Lefthand->SpriteData = Sprite::createWithSpriteFrame(animFrames.front());
		m_Pickup_Lefthand->SpriteData->runAction(rep);
		layer->addChild(m_Pickup_Lefthand->SpriteData, 3);

		m_Charactorbody->SetUserData(m_Pickup_Lefthand->SpriteData);
	}
	

	/*
	m_texture = Director::getInstance()->getTextureCache()->addImage(sPath);

	Animation* animation = Animation::create();
	animation->setDelayPerUnit(0.1f);
	
	for (int i = 0; i < 4; i++) {
		animation->addSpriteFrameWithTexture(m_texture, Rect(i * 36, 0, 36, 32));
	}
	m_Pickup_Lefthand = EffectSprite::create("");
	m_Pickup_Lefthand->setAnchorPoint(Vec2(0.5, 0.3));
	if (g_bViewing) m_Pickup_Lefthand->setFlippedX(false);
	else if(!g_bViewing) m_Pickup_Lefthand->setFlippedX(true);
	layer->addChild(m_Pickup_Lefthand, 3);*/



	//Charactor_Hand::CreateHandObj(sPath);
}

void Charactor_Hand::CreateHandObj(std::string sPath) {
	//월드 매개변수로 가져와서 무기바디 만들고 조인트로 캐릭터바디에 붙이기
	b2BodyDef HandObjBodyDef;

	HandObjBodyDef.type = b2_dynamicBody;
	HandObjBodyDef.position.Set(m_Charactorbody->GetPosition().x, m_Charactorbody->GetPosition().y);
	//HandObjBodyDef.userData = HandObjSprite;

	b2PolygonShape HandObjShape;
	HandObjShape.SetAsBox(0.1f, 0.1f);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &HandObjShape;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.1f;
	fixtureDef.restitution = 0.0;
	fixtureDef.isSensor = false;

	fixtureDef.filter.categoryBits = 0x0003;
	fixtureDef.filter.maskBits = 0x0010; // 몬스터만 충돌

	m_HandObjbody = m_world->CreateBody(&HandObjBodyDef);
	m_HandObjbody->CreateFixture(&fixtureDef);

	m_HandObjbody->SetIsCh(false);
	m_HandObjbody->SetFixedRotation(false);
	//-------------------------------------------------------------------------------------------------------------
	m_texture = Director::getInstance()->getTextureCache()->addImage("Images/Items/sword1.png");

	Sprite* ItemObjSprite = Sprite::createWithTexture(m_texture, Rect(0, 0, 26, 26));
	m_Layer->addChild(ItemObjSprite, 3);

	b2BodyDef ItemObjBodyDef;
	ItemObjBodyDef.type = b2_dynamicBody;
	ItemObjBodyDef.position.Set(m_HandObjbody->GetPosition().x, m_HandObjbody->GetPosition().y);
	ItemObjBodyDef.userData = ItemObjSprite;

	b2PolygonShape ItemObjShape;
	//ItemObjShape.SetAsBox(0.1f, 1.0f);

	auto posi = Vec2(0, 0);
	b2Vec2 vertices[4];
	vertices[0].x = posi.x + 0.0f;
	vertices[0].y = posi.y + 0.0f;

	vertices[1].x = posi.x + 0.0f;
	vertices[1].y = posi.y + 1;

	vertices[2].x = posi.x + (1 - 0.8);
	vertices[2].y = posi.y + 1;

	vertices[3].x = posi.x + (1 - 0.8);
	vertices[3].y = posi.y + 0.0f;

	ItemObjShape.Set(vertices, 4);
	b2FixtureDef ItemfixtureDef;
	ItemfixtureDef.shape = &ItemObjShape;
	ItemfixtureDef.density = 0.1f;
	ItemfixtureDef.friction = 0.1f;
	ItemfixtureDef.restitution = 0.0;
	ItemfixtureDef.isSensor = false;

	ItemfixtureDef.filter.categoryBits = 0x0003;
	ItemfixtureDef.filter.maskBits = 0x0010; // 몬스터만 충돌

	m_ItemObjbody = m_world->CreateBody(&ItemObjBodyDef);
	m_ItemObjbody->CreateFixture(&ItemfixtureDef);

	m_ItemObjbody->SetHp(1000);
	m_ItemObjbody->SetIsCh(false);

	b2Fixture* SensorFixture;
	SensorFixture = m_ItemObjbody->CreateFixture(&ItemfixtureDef);
	SensorFixture->SetUserData((void*)2);

	b2WeldJointDef weldJointDef;
	weldJointDef.Initialize(m_HandObjbody, m_ItemObjbody, m_HandObjbody->GetPosition() - b2Vec2(1.5, 1.3));
	m_world->CreateJoint(&weldJointDef);

	b2WheelJointDef Wjd;
	Wjd.Initialize(m_Charactorbody, m_HandObjbody, m_HandObjbody->GetPosition(), b2Vec2(0.0f, 1.0f));
	Wjd.motorSpeed = -1.0f;
	Wjd.maxMotorTorque = 5.0f;
	Wjd.enableMotor = true;
	Wjd.frequencyHz = 50.0f;

	b2WheelJoint* Wheel = (b2WheelJoint*)m_world->CreateJoint(&Wjd);
}

ESorSP* Charactor_Hand::AnimationState(std::string Obj_name, _HandState state, Layer* layer, bool viewing) {
	int nSheetmx;
	Size size;
	std::string sPath;
	std::string sPath_n;
	if (Obj_name == "Pickaxe") {
		switch (state) {
		case LeftHand_On:
			nSheetmx = 6;
			sPath = "Images/sprites/pickaxe_run/fox_pickaxe_run_.plist";
			sPath_n = "Images/sprites/pickaxe_run/fox_pickaxe_run_n.png";
			size.width = 36;
			size.height = 32;
			break;
		case RightHand_On:
			nSheetmx = 6;
			sPath = "Images/sprites/pickaxe_run/fox_pickaxe_run_.plist";
			sPath_n = "Images/sprites/pickaxe_run/fox_pickaxe_run_n.png";
			size.width = 36;
			size.height = 32;
			break;
		case Hand_On:
			nSheetmx = 4;
			sPath = "Images/sprites/pickaxe_idle/fox_pickaxe_idle_.plist";
			sPath_n = "Images/sprites/pickaxe_idle/fox_pickaxe_idle_n.png";
			size.width = 36;
			size.height = 32;
			break;
		case JumpHand_On:
			nSheetmx = 1;
			sPath = "Images/sprites/pickaxe_jump/fox_pickaxe_jump1_.plist";
			sPath_n = "Images/sprites/pickaxe_jump/fox_pickaxe_jump1_n.png";
			size.width = 36;
			size.height = 32;
			break;
		case JumpHand_Down:
			nSheetmx = 1;
			sPath = "Images/sprites/pickaxe_jump/fox_pickaxe_jump2_.plist";
			sPath_n = "Images/sprites/pickaxe_jump/fox_pickaxe_jump2_n.png";
			size.width = 36;
			size.height = 32;
			break;
		case UPDOWN:
			nSheetmx = 2;
			sPath = "Images/sprites/latern_item_climb/fox_climb_pickaxe_.plist";
			sPath_n = "Images/sprites/latern_item_climb/fox_climb_pickaxe_n.png";
			size.width = 33;
			size.height = 32;
			break;
		case ATTACK:
			nSheetmx = 3;
			sPath = "Images/sprites/pickaxe_attack/fox_pickaxe_attack_.plist";
			sPath_n = "Images/sprites/pickaxe_attack/fox_pickaxe_attack_n.png";
			size.width = 39;
			size.height = 32;
			break;
		case UP_ATTACK:
			nSheetmx = 3;
			sPath = "Images/sprites/pickaxe_attack_up/fox_pickaxe_attack_up_.plist";
			sPath_n = "Images/sprites/pickaxe_attack_up/fox_pickaxe_attack_up_n.png";
			size.width = 33;
			size.height = 32;
			break;
		case UNDER_ATTACK:
			nSheetmx = 3;
			sPath = "Images/sprites/pickaxe_attack_under/fox_pickaxe_attack_under_.plist";
			sPath_n = "Images/sprites/pickaxe_attack_under/fox_pickaxe_attack_under_n.png";
			size.width = 39;
			size.height = 32;
			break;
		case THROW_BOMB:
			nSheetmx = 1;
			sPath = "Images/sprites/bomb_throw/throw_bomb_.plist";
			sPath_n = "Images/sprites/bomb_throw/throw_bomb_n.png";
			size.width = 40;
			size.height = 32;
			break;
		case PAIN:
			nSheetmx = 1;
			sPath = "Images/sprites/lantern_pain/fox_pain_pickaxe.plist";
			sPath_n = "Images/sprites/lantern_pain/fox_pain_pickaxe_n.png";
			size.width = 32;
			size.height = 32;
			break;
		default:
			log("state error");
			break;
		}
	}

	else if (Obj_name == "Sword") {
		switch (state) {
		case LeftHand_On:
			nSheetmx = 6;
			sPath = "Images/sprites/sword_run/fox_sword_run_.plist";
			sPath_n = "Images/sprites/sword_run/fox_sword_run_n.png";
			size.width = 36;
			size.height = 32;
			break;
		case RightHand_On:
			nSheetmx = 6;
			sPath = "Images/sprites/sword_run/fox_sword_run_.plist";
			sPath_n = "Images/sprites/sword_run/fox_sword_run_n.png";
			size.width = 36;
			size.height = 32;
			break;
		case Hand_On:
			nSheetmx = 4;
			sPath = "Images/sprites/sword_idle/fox_sword_idle_.plist";
			sPath_n = "Images/sprites/sword_idle/fox_sword_idle_n.png";
			size.width = 36;
			size.height = 32;
			break;
		case JumpHand_On:
			nSheetmx = 1;
			sPath = "Images/sprites/sword_jump/fox_sword_jump1_.plist";
			sPath_n = "Images/sprites/sword_jump/fox_sword_jump1_n.png";
			size.width = 36;
			size.height = 32;
			break;
		case JumpHand_Down:
			nSheetmx = 1;
			sPath = "Images/sprites/sword_jump/fox_sword_jump2_.plist";
			sPath_n = "Images/sprites/sword_jump/fox_sword_jump2_n.png";
			size.width = 36;
			size.height = 32;
			break;
		case UPDOWN:
			nSheetmx = 2;
			sPath = "Images/sprites/latern_item_climb/fox_climb_sword_.plist";
			sPath_n = "Images/sprites/latern_item_climb/fox_climb_sword_n.png";
			size.width = 33;
			size.height = 32;
			break;
		case ATTACK:
			nSheetmx = 3;
			sPath = "Images/sprites/sword_attack/fox_sword_attack__.plist";
			sPath_n = "Images/sprites/sword_attack/fox_sword_attack__n.png";
			size.width = 39;
			size.height = 32;
			break;
		case THROW_BOMB:
			nSheetmx = 1;
			sPath = "Images/sprites/bomb_throw/throw_bomb_.plist";
			sPath_n = "Images/sprites/bomb_throw/throw_bomb_n.png";
			size.width = 40;
			size.height = 32;
			break;
		case PAIN:
			nSheetmx = 1;
			sPath = "Images/sprites/lantern_pain/fox_pain_sword.plist";
			sPath_n = "Images/sprites/lantern_pain/fox_pain_sword_n.png";
			size.width = 32;
			size.height = 32;
			break;
		default:
			log("state error");
			break;
		}
	}

	else if (Obj_name == "Gun") {
		switch (state) {
		case LeftHand_On:
			nSheetmx = 6;
			sPath = "Images/sprites/gun_run/fox_gun_run_.plist";
			sPath_n = "Images/sprites/gun_run/fox_gun_run_n.png";
			size.width = 36;
			size.height = 32;
			break;
		case RightHand_On:
			nSheetmx = 6;
			sPath = "Images/sprites/gun_run/fox_gun_run_.plist";
			sPath_n = "Images/sprites/gun_run/fox_gun_run_n.png";
			size.width = 36;
			size.height = 32;
			break;
		case Hand_On:
			nSheetmx = 4;
			sPath = "Images/sprites/gun_idle/fox_gun_idle_.plist";
			sPath_n = "Images/sprites/gun_idle/fox_gun_idle_n.png";
			size.width = 36;
			size.height = 32;
			break;
		case JumpHand_On:
			nSheetmx = 1;
			sPath = "Images/sprites/gun_jump/fox_gun_jump1_.plist";
			sPath_n = "Images/sprites/gun_jump/fox_gun_jump1_n.png";
			size.width = 36;
			size.height = 32;
			break;
		case JumpHand_Down:
			nSheetmx = 1;
			sPath = "Images/sprites/gun_jump/fox_gun_jump2_.plist";
			sPath_n = "Images/sprites/gun_jump/fox_gun_jump2_n.png";
			size.width = 36;
			size.height = 32;
			break;
		case UPDOWN:
			nSheetmx = 2;
			sPath = "Images/sprites/latern_item_climb/fox_climb_gun_.plist";
			sPath_n = "Images/sprites/latern_item_climb/fox_climb_gun_n.png";
			size.width = 33;
			size.height = 32;
			break;
		case ATTACK:
			nSheetmx = 3;
			sPath = "Images/sprites/gun_attack/fox_gun_attack_.plist";
			sPath_n = "Images/sprites/gun_attack/fox_gun_attack_n.png";
			size.width = 39;
			size.height = 32;
			break;
		case THROW_BOMB:
			nSheetmx = 1;
			sPath = "Images/sprites/bomb_throw/throw_bomb_.plist";
			sPath_n = "Images/sprites/bomb_throw/throw_bomb_n.png";
			size.width = 40;
			size.height = 32;
			break;
		case PAIN:
			nSheetmx = 1;
			sPath = "Images/sprites/lantern_pain/fox_pain_gun.plist";
			sPath_n = "Images/sprites/lantern_pain/fox_pain_gun_n.png";
			size.width = 32;
			size.height = 32;
			break;
		default:
			log("state error");
			break;
		}
	}

	auto spritecache = SpriteFrameCache::getInstance();
	spritecache->addSpriteFramesWithFile(sPath);

	Vector<SpriteFrame*> animFrames;
	char str[100];
	for (int i = 0; i < nSheetmx; i++) {
		if (Obj_name == "Pickaxe") {
			switch (state) {
			case LeftHand_On:
			case RightHand_On:
				sprintf(str, "pickaxe_run_%d.png", i);
				animFrames.pushBack(spritecache->getSpriteFrameByName(str));
				break;
			case Hand_On:
				sprintf(str, "pickaxe_idle_%d.png", i);
				animFrames.pushBack(spritecache->getSpriteFrameByName(str));
				break;
			case UPDOWN:
				sprintf(str, "fox_climb_pickaxe_%d.png", i);
				animFrames.pushBack(spritecache->getSpriteFrameByName(str));
				break;
			case JumpHand_On:
				sprintf(str, "pickaxe_jump1.png");
				animFrames.pushBack(spritecache->getSpriteFrameByName(str));
				break;
			case JumpHand_Down:
				sprintf(str, "pickaxe_jump2.png");
				animFrames.pushBack(spritecache->getSpriteFrameByName(str));
				break;
			case ATTACK:
				sprintf(str, "pickaxe_attack_%d.png", i);
				animFrames.pushBack(spritecache->getSpriteFrameByName(str));
				break;
			case UP_ATTACK:
				sprintf(str, "pickaxe_attack_up_%d.png", i);
				animFrames.pushBack(spritecache->getSpriteFrameByName(str));
				break;
			case UNDER_ATTACK:
				sprintf(str, "pickaxe_attack_under_%d.png", i);
				animFrames.pushBack(spritecache->getSpriteFrameByName(str));
				break;
			case THROW_BOMB:
				sprintf(str, "throw_bomb.png");
				animFrames.pushBack(spritecache->getSpriteFrameByName(str));
				break;
			case PAIN:
				sprintf(str, "fox_pain_pickaxe.png");
				animFrames.pushBack(spritecache->getSpriteFrameByName(str));
				break;
			default:
				log("state error");
				break;
			}
		}
		else if (Obj_name == "Sword") {
			switch (state) {
			case LeftHand_On:
			case RightHand_On:
				sprintf(str, "sword_run_%d.png", i);
				animFrames.pushBack(spritecache->getSpriteFrameByName(str));
				break;
			case Hand_On:
				sprintf(str, "sword_idle_%d.png", i);
				animFrames.pushBack(spritecache->getSpriteFrameByName(str));
				break;
			case UPDOWN:
				sprintf(str, "fox_climb_sword_%d.png", i);
				animFrames.pushBack(spritecache->getSpriteFrameByName(str));
				break;
			case JumpHand_On:
				sprintf(str, "sword_jump1.png");
				animFrames.pushBack(spritecache->getSpriteFrameByName(str));
				break;
			case JumpHand_Down:
				sprintf(str, "sword_jump2.png");
				animFrames.pushBack(spritecache->getSpriteFrameByName(str));
				break;
			case ATTACK:
				sprintf(str, "sword_attack_%d.png", i);
				animFrames.pushBack(spritecache->getSpriteFrameByName(str));
				break;
			case THROW_BOMB:
				sprintf(str, "throw_bomb.png");
				animFrames.pushBack(spritecache->getSpriteFrameByName(str));
				break;
			case PAIN:
				sprintf(str, "fox_pain_sword.png");
				animFrames.pushBack(spritecache->getSpriteFrameByName(str));
				break;
			default:
				log("state error");
				break;
			}
		}
		else if (Obj_name == "Gun") {
			switch (state) {
			case LeftHand_On:
			case RightHand_On:
				sprintf(str, "gun_run_%d.png", i);
				animFrames.pushBack(spritecache->getSpriteFrameByName(str));
				break;
			case Hand_On:
				sprintf(str, "gun_idle_%d.png", i);
				animFrames.pushBack(spritecache->getSpriteFrameByName(str));
				break;
			case UPDOWN:
				sprintf(str, "fox_climb_gun_%d.png", i);
				animFrames.pushBack(spritecache->getSpriteFrameByName(str));
				break;
			case JumpHand_On:
				sprintf(str, "gun_jump1.png");
				animFrames.pushBack(spritecache->getSpriteFrameByName(str));
				break;
			case JumpHand_Down:
				sprintf(str, "gun_jump2.png");
				animFrames.pushBack(spritecache->getSpriteFrameByName(str));
				break;
			case ATTACK:
				sprintf(str, "gun_attack_%d.png", i);
				animFrames.pushBack(spritecache->getSpriteFrameByName(str));
				break;
			case THROW_BOMB:
				sprintf(str, "throw_bomb.png");
				animFrames.pushBack(spritecache->getSpriteFrameByName(str));
				break;
			case PAIN:
				sprintf(str, "fox_pain_gun.png");
				animFrames.pushBack(spritecache->getSpriteFrameByName(str));
				break;
			default:
				log("state error");
				break;
			}
		}
	}

	
	Animation* animation;
	if (Obj_name == "Sword" && state == ATTACK) {
		animation = Animation::createWithSpriteFrames(animFrames, 0.05);
	}
	else {
		animation = Animation::createWithSpriteFrames(animFrames, 0.1);
	}
	

	Animate* animate = Animate::create(animation);
	RepeatForever* rep = RepeatForever::create(animate);
	if (m_Charactor_data.StageNumber == SHELTER || m_Charactor_data.bStage_blind) {
		m_Pickup_Lefthand->ESpriteData = EffectSprite::createWithSpriteFrame(animFrames.front());
		m_Pickup_Lefthand->ESpriteData->runAction(rep);
		m_Pickup_Lefthand->ESpriteData->setEffect(m_effect, sPath_n);
		layer->addChild(m_Pickup_Lefthand->ESpriteData, 3);

		m_Pickup_Lefthand->ESpriteData->setPosition(Vec2(1, 1));
		m_Pickup_Lefthand->ESpriteData->setAnchorPoint(Vec2(0.5f, 0.5f));

		if (state == ATTACK && viewing)
			m_Pickup_Lefthand->ESpriteData->setAnchorPoint(Vec2(0.3f, 0.3f));
		else if (state == ATTACK && !viewing)
			m_Pickup_Lefthand->ESpriteData->setAnchorPoint(Vec2(0.7f, 0.3f));

		if (g_bViewing)
			m_Pickup_Lefthand->ESpriteData->setFlippedX(false);
		else if (!g_bViewing)
			m_Pickup_Lefthand->ESpriteData->setFlippedX(true);

		m_Charactorbody->SetUserData(m_Pickup_Lefthand->ESpriteData);
	}
	else {
		m_Pickup_Lefthand->SpriteData = EffectSprite::createWithSpriteFrame(animFrames.front());
		m_Pickup_Lefthand->SpriteData->runAction(rep);
		layer->addChild(m_Pickup_Lefthand->SpriteData, 3);

		m_Pickup_Lefthand->SpriteData->setPosition(Vec2(1, 1));
		m_Pickup_Lefthand->SpriteData->setAnchorPoint(Vec2(0.5f, 0.5f));

		if (state == ATTACK && viewing)
			m_Pickup_Lefthand->SpriteData->setAnchorPoint(Vec2(0.3f, 0.3f));
		else if (state == ATTACK && !viewing)
			m_Pickup_Lefthand->SpriteData->setAnchorPoint(Vec2(0.7f, 0.3f));

		if (g_bViewing)
			m_Pickup_Lefthand->SpriteData->setFlippedX(false);
		else if (!g_bViewing)
			m_Pickup_Lefthand->SpriteData->setFlippedX(true);

		m_Charactorbody->SetUserData(m_Pickup_Lefthand->SpriteData);
	}

	return m_Pickup_Lefthand;
}