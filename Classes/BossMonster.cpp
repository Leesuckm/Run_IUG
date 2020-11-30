#include "BossMonster.h"

BossMonster::BossMonster() {}

BossMonster::~BossMonster()
{
}

void BossMonster::CreateBossMap(Layer* _layer, Charactor_Data* _charactor_data) {
	m_layer = _layer;
	m_Charactor_data = _charactor_data;
	
	SpriteFrameCache* SFC = SpriteFrameCache::getInstance();
	SFC->addSpriteFramesWithFile("Images/Background/FallingLava_.plist");

	Vector<SpriteFrame*> animFrames;
	char str[100];
	for (int i = 0; i < 3; i++) {
		sprintf(str, "FallingLava_%d.png", i);
		animFrames.pushBack(SFC->getSpriteFrameByName(str));
	}

	Sprite* FallingLava_Sprite = Sprite::createWithSpriteFrame(animFrames.front());
	Animation* animation = Animation::createWithSpriteFrames(animFrames, 0.3f);
	Animate* animate = Animate::create(animation);
	RepeatForever* rep = RepeatForever::create(animate);

	FallingLava_Sprite->setAnchorPoint(Vec2::ZERO);
	FallingLava_Sprite->setPosition(Vec2::ZERO);
	FallingLava_Sprite->runAction(rep);

	m_layer->addChild(FallingLava_Sprite, 0);

	m_BossMap_Background = Sprite::create("Images/Background/BossMap_Background.png");
	m_BossMap_Background->setAnchorPoint(Vec2::ZERO);
	m_BossMap_Background->setPosition(Vec2::ZERO);
	m_layer->addChild(m_BossMap_Background, 0);

	CreateBossEye();
	CreateBossCore();
	/*
	m_eye_position[0] = Vec2(640, 250);
	m_eye_position[1] = Vec2(480, 450);
	m_eye_position[2] = Vec2(800, 450);
	m_eye_position[3] = Vec2(640, 600);

	for (int i = 0; i < 4; i++) {
		m_Boss_eye_out[i] = Sprite::create("Images/Monsters/Boss/Boss_eye_out.png");
		m_Boss_eye_out[i]->setPosition(m_eye_position[i]);
		m_layer->addChild(m_Boss_eye_out[i], 0);

		m_Boss_eye_in[i] = Sprite::create("Images/Monsters/Boss/Boss_eye_in.png");
		m_Boss_eye_in[i]->setPosition(m_eye_position[i]);
		m_Boss_eye_in[i]->setScale(0.5f);
		m_layer->addChild(m_Boss_eye_in[i], 1);
	}*/
}

void BossMonster::CreateBossEye() {
	SpriteFrameCache* SFC = SpriteFrameCache::getInstance();

	SFC->addSpriteFramesWithFile("Images/Monsters/Boss/Boss_idle_.plist");

	Vector<SpriteFrame*> animFrames;
	char str[100];
	for (int i = 0; i < 5; i++) {
		sprintf(str, "Boss_Idle_%d.png", i);
		animFrames.pushBack(SFC->getSpriteFrameByName(str));
	}

	m_Boss_eye_Sprite = Sprite::createWithSpriteFrame(animFrames.front());
	Animation* animation = Animation::createWithSpriteFrames(animFrames, 0.3f);
	Animate* animate = Animate::create(animation);
	RepeatForever* rep = RepeatForever::create(animate);

	m_eye_position = Vec2(640, 430);
	m_Boss_eye_Sprite->setAnchorPoint(Vec2(0.5, 0.5));
	m_Boss_eye_Sprite->setPosition(m_eye_position);
	m_Boss_eye_Sprite->runAction(rep);

	m_layer->addChild(m_Boss_eye_Sprite, 1);
}

void BossMonster::CreateBossCore() {
	SpriteFrameCache* SFC = SpriteFrameCache::getInstance();

	SFC->addSpriteFramesWithFile("Images/Monsters/Boss/BossCore_idle_.plist");

	Vector<SpriteFrame*> animFrames;
	char str[100];
	for (int i = 0; i < 5; i++) {
		sprintf(str, "BossCore_idle_%d.png", i);
		animFrames.pushBack(SFC->getSpriteFrameByName(str));
	}

	m_Boss_Core_Sprite = Sprite::createWithSpriteFrame(animFrames.front());
	Animation* animation = Animation::createWithSpriteFrames(animFrames, 0.3f);
	Animate* animate = Animate::create(animation);
	RepeatForever* rep = RepeatForever::create(animate);

	m_Core_position = Vec2(640, 430);
	m_Boss_Core_Sprite->setAnchorPoint(Vec2(0.5, 0.5));
	m_Boss_Core_Sprite->setPosition(m_Core_position);
	m_Boss_Core_Sprite->runAction(rep);

	m_layer->addChild(m_Boss_Core_Sprite, 1);
	m_BossCore_Hp = 1000;
}


void BossMonster::MonsterPatrol(b2World* world, b2Body* Charactor, float movetime, Layer* layer) {
	m_fPlayTime += movetime;
	if (m_BossCore_Hp >= 0 && m_Boss_ActionTime <= m_fPlayTime) {
		for (; true;) {
			int SelectPattern = rand() % 10;
			if (SelectPattern >= 0 && SelectPattern < 2) {
				m_BossState = BOSS_IDLE;
				//if (m_BeforeBossState == BOSS_IDLE) continue;
				this->SpriteChange(m_BossState);
				m_fPlayTime = 0;
				m_Boss_ActionTime = 5;
				break;
			}
			else if (SelectPattern >= 2 && SelectPattern < 4) {
				m_BossState = BOSS_PATTERN_A;
				if (m_BeforeBossState != BOSS_IDLE || m_BossState == m_BeforeBossState) continue;
				this->SpriteChange(m_BossState);
				m_fPlayTime = 0;
				m_Boss_ActionTime = 5;
				this->Pattern_A();
				break;
			}
			else if (SelectPattern >= 4 && SelectPattern < 6) {
				m_BossState = BOSS_PATTERN_B;
				if (m_BeforeBossState != BOSS_IDLE || m_BossState == m_BeforeBossState) continue;
				this->SpriteChange(m_BossState);
				m_fPlayTime = 0;
				m_Boss_ActionTime = 10;
				Sprite* Sp = (Sprite*)Charactor->GetUserData();
				this->Pattern_B(Sp->getPosition());
				break;
			}
			else if (SelectPattern >= 6 && SelectPattern < 8) {
				m_BossState = BOSS_PATTERN_C;
				if (m_BeforeBossState != BOSS_IDLE || m_BossState == m_BeforeBossState) continue;
				this->SpriteChange(m_BossState);
				m_fPlayTime = 0;
				m_Boss_ActionTime = 5;
				break;
			}
			else if (SelectPattern >= 8 && SelectPattern < 10) {
				m_BossState = BOSS_PATTERN_D;
				if (m_BeforeBossState != BOSS_IDLE || m_BossState == m_BeforeBossState) continue;
				this->SpriteChange(m_BossState);
				m_fPlayTime = 0;
				m_Boss_ActionTime = 5;
				break;
			}
		}
	}
	Rect rect;
	Sprite* Charactor_Sp = (Sprite*)Charactor->GetUserData();
	switch (m_BossState) {
	case BOSS_IDLE:
		break;
	case BOSS_PATTERN_A:
		for (Sprite* Sp : m_Lava_vc) {
			Sp->setPosition(Vec2(Sp->getPosition().x, Sp->getPosition().y - (movetime * 300)));
			rect = Sp->getBoundingBox();

			log("Lava %f %f", Sp->getPosition().x, Sp->getPosition().y);
			if (rect.intersectsRect(Charactor_Sp->getBoundingBox())) {
				m_Charactor_data->n_Hp_Count--;
			}
		}
		break;
	case BOSS_PATTERN_B:
		m_Lava_Punch->setPosition(Vec2(m_Lava_Punch->getPosition().x - (movetime * 400), m_Lava_Punch->getPosition().y));

		rect = m_Lava_Punch->getBoundingBox();

		if (rect.intersectsRect(Charactor_Sp->getBoundingBox())) {
			m_Charactor_data->n_Hp_Count--;
		}
		else if (rect.intersectsRect(m_Boss_Core_Sprite->getBoundingBox())) {
			m_layer->removeChild(m_Lava_Punch, true);
			m_BossCore_Hp -= 100;
			m_BossState = BOSS_PAIN;
			SpriteChange(m_BossState);
		}

		break;
	case BOSS_PATTERN_C:
		break;
	case BOSS_PATTERN_D:
		break;

	}
}

void BossMonster::SpriteChange(BossState _bossState) {
	if (m_BeforeBossState == BOSS_PATTERN_C || m_BeforeBossState == BOSS_PAIN) {
		m_layer->removeChild(m_Boss_Core_Sprite, true);

		SpriteFrameCache* SFC = SpriteFrameCache::getInstance();

		SFC->addSpriteFramesWithFile("Images/Monsters/Boss/BossCore_idle_.plist");

		Vector<SpriteFrame*> animFrames;
		char str[100];
		for (int i = 0; i < 5; i++) {
			sprintf(str, "BossCore_idle_%d.png", i);
			animFrames.pushBack(SFC->getSpriteFrameByName(str));
		}

		m_Boss_Core_Sprite = Sprite::createWithSpriteFrame(animFrames.front());
		Animation* animation = Animation::createWithSpriteFrames(animFrames, 0.3f);
		Animate* animate = Animate::create(animation);
		RepeatForever* rep = RepeatForever::create(animate);

		m_Core_position = Vec2(640, 430);
		m_Boss_Core_Sprite->setAnchorPoint(Vec2(0.5, 0.5));
		m_Boss_Core_Sprite->setPosition(m_Core_position);
		m_Boss_Core_Sprite->runAction(rep);

		m_layer->addChild(m_Boss_Core_Sprite, 1);
	}

	if (_bossState == BOSS_PATTERN_C || _bossState == BOSS_PAIN) {
		m_layer->removeChild(m_Boss_Core_Sprite, true);
		this->AnimationState(_bossState);
	}
	else{ 
		m_layer->removeChild(m_Boss_eye_Sprite, true);
		this->AnimationState(_bossState);
	}
}

ESorSP* BossMonster::AnimationState(BossState state) {
	int nSheetmx;
	std::string sPath;
	
	switch (state) {
	case BOSS_IDLE:
		nSheetmx = 5;
		sPath = "Images/Monsters/Boss/Boss_idle_.plist";
		break;
	case BOSS_PATTERN_A:
		nSheetmx = 4;
		sPath = "Images/Monsters/Boss/Boss_Pattern_A_.plist";
		break;
	case BOSS_PATTERN_B:
		nSheetmx = 4;
		sPath = "Images/Monsters/Boss/Boss_Pattern_B_.plist";
		break;
	case BOSS_PATTERN_C:
		nSheetmx = 5;
		sPath = "Images/Monsters/Boss/Boss_Pattern_C_.plist";
		break;
	case BOSS_PATTERN_D:
		nSheetmx = 4;
		sPath = "Images/Monsters/Boss/Boss_Pattern_D_.plist";
		break;
	case BOSS_PAIN:
		nSheetmx = 2;
		sPath = "Images/Monsters/Boss/Boss_Pain.plist";
		break;
	}

	SpriteFrameCache* spritecache = SpriteFrameCache::getInstance();
	spritecache->addSpriteFramesWithFile(sPath);

	Vector<SpriteFrame*> animFrames;
	char str[100];
	for (int i = 0; i < nSheetmx; i++) {
		switch (state) {
		case BOSS_IDLE:
			sprintf(str, "Boss_Idle_%d.png", i);
			animFrames.pushBack(spritecache->getSpriteFrameByName(str));
			break;
		case BOSS_PATTERN_A:
			sprintf(str, "Boss_Pattern_A_%d.png", i);
			animFrames.pushBack(spritecache->getSpriteFrameByName(str));
			break;
		case BOSS_PATTERN_B:
			sprintf(str, "Boss_Pattern_B_%d.png", i);
			animFrames.pushBack(spritecache->getSpriteFrameByName(str));
			break;
		case BOSS_PATTERN_C:
			sprintf(str, "Boss_Pattern_C_%d.png", i);
			animFrames.pushBack(spritecache->getSpriteFrameByName(str));
			break;
		case BOSS_PATTERN_D:
			sprintf(str, "Boss_Pattern_D_%d.png", i);
			animFrames.pushBack(spritecache->getSpriteFrameByName(str));
			break;
		case BOSS_PAIN:
			sprintf(str, "BossCore_Pain_%d.png", i);
			animFrames.pushBack(spritecache->getSpriteFrameByName(str));
			break;
		}
	}

	if (state == BOSS_PATTERN_C) {
		m_Boss_Core_Sprite = Sprite::createWithSpriteFrame(animFrames.front());
		Animation* animation = Animation::createWithSpriteFrames(animFrames, 0.3f);
		Animate* animate = Animate::create(animation);
		RepeatForever* repF;
		repF = RepeatForever::create(animate);
		m_Boss_eye_Sprite->runAction(repF);

		m_Boss_Core_Sprite->runAction(repF);
		m_Boss_Core_Sprite->setPosition(m_Core_position);
		m_layer->addChild(m_Boss_Core_Sprite, 1);
	}

	else if (state == BOSS_IDLE) {
		m_Boss_eye_Sprite = Sprite::createWithSpriteFrame(animFrames.front());
		Animation* animation = Animation::createWithSpriteFrames(animFrames, 0.3f);
		Animate* animate = Animate::create(animation);
		RepeatForever* repF;
		repF = RepeatForever::create(animate);
		m_Boss_eye_Sprite->runAction(repF);

		m_Boss_eye_Sprite->setPosition(m_Core_position);
		m_layer->addChild(m_Boss_eye_Sprite, 1);
	}
	else if (state == BOSS_PAIN) {
		m_Boss_Core_Sprite = Sprite::createWithSpriteFrame(animFrames.front());
		Animation* animation = Animation::createWithSpriteFrames(animFrames, 0.3f);
		Animate* animate = Animate::create(animation);
		Repeat* rep;
		rep = Repeat::create(animate, 1);
		m_Boss_Core_Sprite->runAction(rep);

		m_Boss_Core_Sprite->setPosition(m_Core_position);
		m_layer->addChild(m_Boss_Core_Sprite, 1);
	}
	else {
		m_Boss_eye_Sprite = Sprite::createWithSpriteFrame(animFrames.front());
		Animation* animation = Animation::createWithSpriteFrames(animFrames, 0.3f);
		Animate* animate = Animate::create(animation);
		Repeat* rep;
		rep = Repeat::create(animate, 1);
		m_Boss_eye_Sprite->runAction(rep);

		m_Boss_eye_Sprite->setPosition(m_Core_position);
		m_layer->addChild(m_Boss_eye_Sprite, 1);
	}
	
	m_BeforeBossState = state;
	return nullptr;
}

void BossMonster::Pattern_A() {
	int FallingLava_count = rand() % (8 + 1 - 2) + 2;

	SpriteFrameCache* spritecache = SpriteFrameCache::getInstance();
	spritecache->addSpriteFramesWithFile("Images/Monsters/Boss/LavaFall_.plist");

	Vector<SpriteFrame*> animframes;
	char str[100];
	for (int i = 0; i < 3; i++) {
		sprintf(str, "LavaFall_%d.png", i);
		animframes.pushBack(spritecache->getSpriteFrameByName(str));
	}

	for (int i = 0; i < FallingLava_count; i++) {
		Sprite* FallingLava = Sprite::createWithSpriteFrame(animframes.front());
		Animation* animation = Animation::createWithSpriteFrames(animframes, 0.3f);
		Animate* animate = Animate::create(animation);
		RepeatForever* rep = RepeatForever::create(animate);
		FallingLava->runAction(rep);

		FallingLava->setPosition(Vec2(100 * (rand() % 10), 800));
		
		m_layer->addChild(FallingLava, 1);

		m_Lava_vc.pushBack(FallingLava);
	}
}

void BossMonster::Pattern_B(Vec2 position) {
	m_Lava_Punch = Sprite::create("Images/Monsters/Boss/Lava_Punch.png");
	m_Lava_Punch->setPosition(Vec2(1200, position.y));
	m_layer->addChild(m_Lava_Punch);
}