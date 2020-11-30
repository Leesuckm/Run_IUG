#include "MonsterCreator.h"
#include "EffectSprite.h"
#include "LightEffect.h"
#include <random>

MonsterCreator::MonsterCreator() {
	m_Monster = new Monster_Property;
	m_Recognize = new Monster_Property;
}
MonsterCreator::MonsterCreator(Charactor_Data _charactor_data, LightEffect* _effect) {
	m_Charactor_data = _charactor_data;

	m_Monster = new Monster_Property;
	m_Recognize = new Monster_Property;

	m_Skel_sprite = new ESorSP;
	m_effect = _effect;
}

MonsterCreator::~MonsterCreator() {
	delete m_Monster;
	delete m_Recognize;
	delete m_Skel_sprite;
	delete m_oldsprite;
}

b2Body* MonsterCreator::CreateMonster(b2World* world, Vec2 position, Layer* layer) {
	std::string sPath = "Images/Monsters/skel_idle/skel_idle_.plist";
	std::string sPath_n = "Images/Monsters/skel_idle/skel_idle_n.png";
	auto spritecache = SpriteFrameCache::getInstance();
	spritecache->addSpriteFramesWithFile(sPath);

	Vector<SpriteFrame*> animFrames;
	char str[100];
	for (int i = 0; i < 4; i++) {
		sprintf(str, "skel_idle_%d.png", i);
		animFrames.pushBack(spritecache->getSpriteFrameByName(str));
	}

	Sprite* Skel_sprite = Sprite::createWithSpriteFrame(animFrames.front());
	Animation* animation = Animation::createWithSpriteFrames(animFrames, 0.1f);
	Animate* animate = Animate::create(animation);
	RepeatForever* rep = RepeatForever::create(animate);
	Skel_sprite->runAction(rep);
//	Skel_sprite->setColor(m_effect->getAmbientLightColor());
//	Skel_sprite->setEffect(m_effect, sPath_n);
	layer->addChild(Skel_sprite, 3);

	/*
	m_texture = Director::getInstance()->getTextureCache()->addImage("Images/Monsters/skel_idle.png");

	Animation* animation = Animation::create();
	animation->setDelayPerUnit(0.1f);

	for (int i = 0; i < 4; i++) {
		animation->addSpriteFrameWithTexture(m_texture, Rect(i * 64, 0, 64, 64));
	}

	Sprite* m_MonsterSprite = Sprite::createWithTexture(m_texture, Rect(0, 0, 64, 64));
	m_MonsterSprite->setAnchorPoint(Vec2(0.5, 0.3));
	m_MonsterSprite->setScale(0.5f);
	layer->addChild(m_MonsterSprite, 3);

	Animate* animate = Animate::create(animation);
	RepeatForever* rep = RepeatForever::create(animate);
	m_MonsterSprite->runAction(rep);*/

//	b2BodyDef m_MonsterBodyDef;

	m_Monster->body_def.type = b2_dynamicBody;

	b2PolygonShape m_MonsterPolygon;
	
	b2Vec2 body_vertices[7];
	body_vertices[0] = b2Vec2(0, -0.4f);
	body_vertices[1] = b2Vec2(0.2, -0.35f);
	body_vertices[2] = b2Vec2(0.4, -0.3f);
	body_vertices[3] = b2Vec2(0.4, 0.4f);
	body_vertices[4] = b2Vec2(-0.4, 0.4f);
	body_vertices[5] = b2Vec2(-0.4, -0.3f);
	body_vertices[6] = b2Vec2(-0.2, -0.35f);

	m_MonsterPolygon.Set(body_vertices, 7);

	b2CircleShape circle;
	circle.m_radius = 0.3f;

	m_Monster->fixture_def.shape = &m_MonsterPolygon; // 여기에 추가된 Shape를 써서 기존 body에 또 fixture를 넣어야만 적용됨.
	m_Monster->fixture_def.density = 10.0f;
	m_Monster->fixture_def.friction = 1.0f;
	m_Monster->fixture_def.restitution = 0.0;

	m_Monster->fixture_def.filter.categoryBits = 0x0010;
	m_Monster->fixture_def.filter.maskBits = 0x000F;

	m_Monster->body_def.position.Set(position.x / 32, position.y / 32);
	m_Monster->body_def.userData = Skel_sprite;

	m_Monster->body = world->CreateBody(&m_Monster->body_def);
	m_Monster->body->CreateFixture(&m_Monster->fixture_def);

	m_Monster->body->SetIsCh(false);
	m_Monster->body->SetHp(4);
	m_Monster->body->SetFixedRotation(true);
	
	MonsterCreator::RecognizeMonster(world, layer);
	
	return m_Monster->body;

	m_MoveState = MS_STOP;
}

void MonsterCreator::RecognizeMonster(b2World* world, Layer* layer) {
	m_texture = Director::getInstance()->getTextureCache()->addImage("Images/Monsters/Monster_Recognize.png");

	Sprite* m_RecognizeSprite = Sprite::createWithTexture(m_texture, Rect(0, 0, 150, 37));
	m_RecognizeSprite->setAnchorPoint(Vec2(0.5, 0.3));
	//m_RecognizeSprite->setScale(5.0f);
	layer->addChild(m_RecognizeSprite, 3);

	m_Recognize->body_def.type = b2_dynamicBody;

	b2CircleShape circle;
	circle.m_radius = 0.2f;
	m_Recognize->fixture_def.isSensor = false;
	m_Recognize->fixture_def.shape = &circle;
	m_Recognize->fixture_def.density = 0.1f;
	m_Recognize->fixture_def.friction = 0.0f;
	m_Recognize->fixture_def.restitution = 0.0;

	m_Recognize->fixture_def.filter.categoryBits = 0x0030;
	m_Recognize->fixture_def.filter.maskBits = 0x0002;

	m_Recognize->body_def.position.Set(m_Monster->body->GetPosition().x, m_Monster->body->GetPosition().y);
	m_Recognize->body_def.userData = m_RecognizeSprite;

	m_Recognize->body = world->CreateBody(&m_Recognize->body_def);
	m_Recognize->body->CreateFixture(&m_Recognize->fixture_def);

	m_Recognize->body->SetHp(10);
	m_Recognize->body->SetIsCh(false);
	// weldhjoint로 붙일 body의 SetFixedRotation을 true로 해버리면 붙질 않는다. false로 해두어야한다.
	m_Recognize->body->SetFixedRotation(false); 

	b2Fixture* SensorFixture;
	SensorFixture = m_Recognize->body->CreateFixture(&m_Recognize->fixture_def);
	SensorFixture->SetUserData((void*)8);

	b2WeldJointDef weldJointDef;
	weldJointDef.Initialize(m_Monster->body, m_Recognize->body, m_Monster->body->GetPosition());
	world->CreateJoint(&weldJointDef);
}

// 몬스터 body 각자의 움직임을 서로 다르게, 그리고 랜덤하게 움직이기 위해선
// 서로 다르게 움직일 body 상태에 따른 Sprite 변화 및 움직임을 위한 state,
// 어느시간동안 하나의 움직임을 취할건지 time이 필요하다.
void MonsterCreator::MonsterPatrol(b2World* world, std::list<Monster> *Monsters, b2Body* Charactor, float movetime, Layer* layer, int foot) {
	
	//Monster* monster = new Monster(nullptr, nullptr, nullptr, MS_STOP, 0);
	EffectSprite* Recognize_sp = static_cast<EffectSprite*>(Charactor->GetUserData());
	
	std::list<Monster>::iterator iter;
	for (iter = Monsters->begin(); iter != Monsters->end();) {
		if (iter->obj_name == "Skel") {
			// 몬스터 Hp가 0미만이라면 죽은것이때문에 list에서 제거한다.
			if (iter->body->GetHp() <= 0) {
				Monsters->erase(iter++);
				continue;
			}
			Sprite* spriteData = static_cast<Sprite*>(iter->recognize_body->GetUserData());
			Rect rect = spriteData->getBoundingBox();

			iter->dt += movetime;
			b2Vec2 vel = iter->body->GetLinearVelocity();
			float desiredVel = 0;

			// 캐릭터가 몬스터의 인식범위 내에 들어갈 시.
			if (iter->time <= iter->dt && rect.intersectsRect(Recognize_sp->getBoundingBox()) && iter->state != MS_DEATH) {
				//log("Monster_Recognize");
				b2Vec2 Charactor_position = Charactor->GetPosition();
				b2Vec2 Monster_position = iter->body->GetPosition();

				// 캐릭터가 몬스터의 위치에서 왼쪽에 위치할때.
				if (Charactor_position.x < Monster_position.x) {
					iter->state = MS_LEFT;
					MonsterCreator::SpriteChange(iter, layer);
					iter->dt = 0;
					desiredVel = -10;
				}
				else {
					iter->state = MS_RIGHT;
					MonsterCreator::SpriteChange(iter, layer);
					iter->dt = 0;
					desiredVel = 10;
				}
			}

			if (iter->time <= iter->dt && iter->state != MS_DEATH) {
				iter->time = rand() % (5 + 1 - 2) + 2;
				int p = rand() % 10;
				if (p >= 0 && p <= 2) {
					iter->state = MS_RIGHT;
					MonsterCreator::SpriteChange(iter, layer);
					iter->dt = 0;
					desiredVel = 10;
				}
				if (p >= 3 && p <= 5) {
					iter->state = MS_LEFT;
					MonsterCreator::SpriteChange(iter, layer);
					iter->dt = 0;
					desiredVel = -10;
				}
				if (p >= 6 && p <= 9) {
					iter->state = MS_STOP;
					MonsterCreator::SpriteChange(iter, layer);
					iter->dt = 0;
					desiredVel = 0;
				}
			}
			float velChange = desiredVel - vel.x;
			float impulse = iter->body->GetMass() * velChange;

			switch (iter->state) {
			case MS_RIGHT:
				//iter->body->ApplyLinearImpulse(b2Vec2(0.1, 0), iter->body->GetWorldCenter(), true);
				iter->body->SetLinearVelocity(b2Vec2(3, -3));
				break;
			case MS_LEFT:
				iter->body->SetLinearVelocity(b2Vec2(-3, -3));
				break;
			case MS_STOP:
				break;
			}
			iter->before_position = iter->body->GetPosition();
			++iter;
		}
		else { ++iter; }
	}
}

ESorSP* MonsterCreator::AnimationState(_moveState state, Layer* layer) {
	int nSheetmx;
	Size size;
	std::string sPath;
	std::string sPath_n;
	switch (state) {
	case MS_LEFT:
		nSheetmx = 4;
		size.width = 32;
		size.height = 41;
		sPath = "Images/Monsters/skel_run/skel_run_.plist";
		sPath_n = "Images/Monsters/skel_run/skel_run_n.png";
		break;
	case MS_RIGHT:
		nSheetmx = 4;
		size.width = 32;
		size.height = 41;
		sPath = "Images/Monsters/skel_run/skel_run_.plist";
		sPath_n = "Images/Monsters/skel_run/skel_run_n.png";
		break;
	case MS_STOP:
		nSheetmx = 4;
		size.width = 31.5;
		size.height = 42;
		sPath = "Images/Monsters/skel_idle/skel_idle_.plist";
		sPath_n = "Images/Monsters/skel_idle/skel_idle_n.png";
		break;
	case MS_PAIN:
		nSheetmx = 2;
		size.width = 40;
		size.height = 45;
		sPath = "Images/Monsters/skel_pain/skel_pain_.plist";
		sPath_n = "Images/Monsters/skel_pain/skel_pain_n.png";
		break;
	case MS_DEATH:
		nSheetmx = 7;
		size.width = 38.86;
		size.height = 47;
		sPath = "Images/Monsters/skel_death/skel_death_.plist";
		sPath_n = "Images/Monsters/skel_death/skel_death_n.png";
		break;
	default:
		log("Monster state error");
		break;
	}

	auto spritecache = SpriteFrameCache::getInstance();
	spritecache->addSpriteFramesWithFile(sPath);

	Vector<SpriteFrame*> animFrames;
	char str[100];
	for (int i = 0; i < nSheetmx; i++) {
		switch (state) {
		case MS_LEFT:
		case MS_RIGHT:
			sprintf(str, "skel_run_%d.png", i);
			animFrames.pushBack(spritecache->getSpriteFrameByName(str));
			break;
		case MS_STOP:
			sprintf(str, "skel_idle_%d.png", i);
			animFrames.pushBack(spritecache->getSpriteFrameByName(str));
			break;
		case MS_PAIN:
			sprintf(str, "skel_pain_%d.png", i);
			animFrames.pushBack(spritecache->getSpriteFrameByName(str));
			break;
		case MS_DEATH:
			sprintf(str, "skel_death_%d.png", i);
			animFrames.pushBack(spritecache->getSpriteFrameByName(str));
			break;
		default:
			log("Monster state error");
			break;
		}
	}
	

	Animation* animation = Animation::createWithSpriteFrames(animFrames, 0.1f);
	Animate* animate = Animate::create(animation);
	RepeatForever* rep = RepeatForever::create(animate);

	// 맵에 따라 몬스터의 Sprite를 Light 효과를 받는 EffectSprite로 할 것인지, 그냥 Sprite로 할 것인지 따로 객체를 생성해 줘야 한다.
	if (m_Charactor_data.StageNumber == SHELTER || m_Charactor_data.bStage_blind) {
		m_Skel_sprite->ESpriteData = EffectSprite::createWithSpriteFrame(animFrames.front());
		m_Skel_sprite->ESpriteData->runAction(rep);
		m_Skel_sprite->ESpriteData->setScale(0.5f);
		m_Skel_sprite->ESpriteData->setAnchorPoint(Vec2(0.5f, 0.3f));
		m_Skel_sprite->ESpriteData->setColor(m_effect->getAmbientLightColor());
		m_Skel_sprite->ESpriteData->setEffect(m_effect, sPath_n);
		layer->addChild(m_Skel_sprite->ESpriteData, 3);
	}
	else {
		m_Skel_sprite->SpriteData = Sprite::createWithSpriteFrame(animFrames.front());
		m_Skel_sprite->SpriteData->runAction(rep);
		m_Skel_sprite->SpriteData->setScale(0.5f);
		m_Skel_sprite->SpriteData->setAnchorPoint(Vec2(0.5f, 0.3f));
		//	Skel_sprite->setColor(m_effect->getAmbientLightColor());
		//	Skel_sprite->setEffect(m_effect, sPath_n);
		layer->addChild(m_Skel_sprite->SpriteData, 3);
	}

	/*
	auto texture = Director::getInstance()->getTextureCache()->addImage(sPath);

	auto animation = Animation::create();
	animation->setDelayPerUnit(0.1f);

	for (int i = 0; i < nSheetmx; i++) {
		animation->addSpriteFrameWithTexture(texture,
			Rect(i * size.width, 0, size.width, size.height));
	}

	m_MonsterSprite = EffectSprite::create("ads");
	m_MonsterSprite->setAnchorPoint(Vec2(0.5f, 0.3f));
	m_MonsterSprite->setScale(0.5f);
	layer->addChild(m_MonsterSprite, 3);

	auto animate = Animate::create(animation);
	auto rep = RepeatForever::create(animate);

	m_MonsterSprite->runAction(rep);*/

	return m_Skel_sprite;
}

void MonsterCreator::SpriteChange(std::list<Monster>::iterator mob, Layer* layer) {
	m_oldsprite = new ESorSP;
	m_newSprite = MonsterCreator::AnimationState(mob->state, layer);

	if (m_Charactor_data.StageNumber == SHELTER || m_Charactor_data.bStage_blind) {
		
		m_oldsprite->ESpriteData = static_cast<EffectSprite*>(mob->body->GetUserData());
		mob->body->SetUserData(nullptr);
		layer->removeChild(m_oldsprite->ESpriteData, true);

		mob->body->SetUserData(m_newSprite->ESpriteData);

		if (mob->state == MS_LEFT) {
			m_newSprite->ESpriteData->setFlippedX(true);
		}

		else if (mob->state == MS_RIGHT) {
			m_newSprite->ESpriteData->setFlippedX(false);
		}
	}
	else {
		m_oldsprite->SpriteData = static_cast<Sprite*>(mob->body->GetUserData());
		mob->body->SetUserData(nullptr);
		layer->removeChild(m_oldsprite->SpriteData, true);

		mob->body->SetUserData(m_newSprite->SpriteData);

		if (mob->state == MS_LEFT) {
			m_newSprite->SpriteData->setFlippedX(true);
		}

		else if (mob->state == MS_RIGHT) {
			m_newSprite->SpriteData->setFlippedX(false);
		}
	}
}