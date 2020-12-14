#include "BossMonster.h"
#include "soundManager.h"

const std::string SEF_FIRE_SHOOTING = "Sounds/FireShooting.mp3";
const std::string SEF_COLUMN_FIRE = "Sounds/Column_Fire.mp3";


BossMonster::BossMonster() {}

BossMonster::~BossMonster()
{
}

void BossMonster::CreateBossMap(Layer* _layer, b2World* _world, Charactor_Data* _charactor_data) {
	m_layer = _layer;
	m_world = _world;
	m_Charactor_data = _charactor_data;
	m_nCharactor_beforeHp = m_Charactor_data->n_Hp_Count;
	
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


void BossMonster::MonsterPatrol(b2World* world, b2Body* Charactor, float movetime, EffectAnimation* _effectAnimation, Layer* layer) {
	m_fPlayTime += movetime;
	m_Collide_Effect = _effectAnimation;
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
				this->CreateFallingLava();
				break;
			}
			else if (SelectPattern >= 4 && SelectPattern < 6) {
				m_BossState = BOSS_PATTERN_B;
				if (m_BeforeBossState != BOSS_IDLE || m_BossState == m_BeforeBossState) continue;
				this->SpriteChange(m_BossState);
				m_fPlayTime = 0;
				m_Boss_ActionTime = 3;
				Sprite* Sp = (Sprite*)Charactor->GetUserData();
				this->CreateLavaPunch(Sp->getPosition());
				break;
			}
			else if (SelectPattern >= 6 && SelectPattern < 8) {
				m_BossState = BOSS_PATTERN_C;
				if (m_BeforeBossState != BOSS_IDLE || m_BossState == m_BeforeBossState) continue;
				this->SpriteChange(m_BossState);
				this->CreateLightningBall();
				this->CreateLightningBall();
				this->CreateLightningBall();
				this->CreateLightningBall();
				this->CreateLightningBall();
				this->CreateLightningBall();
				m_fPlayTime = 0;
				m_Boss_ActionTime = 20;
				break;
			}
			else if (SelectPattern >= 8 && SelectPattern < 10) {
				m_BossState = BOSS_PATTERN_D;
				if (m_BeforeBossState != BOSS_IDLE || m_BossState == m_BeforeBossState) continue;
				this->SpriteChange(m_BossState);
				this->CreateFireColumn();
				m_fPlayTime = 0;
				m_Boss_ActionTime = 10;
				break;
			}
		}
	}
	Rect rect;
	Sprite* Charactor_Sp = (Sprite*)Charactor->GetUserData();

	// 현재 Frame의 hp가 이전 Frame의 hp보다 작다면 공격을 받은 것 이므로 비교할 대상인 이전 Frame hp 값을 업데이트해주고 연속적인 공격을 받지 않도록 캐릭터가 공격받은 상태로 업데이트 해준다.
	if (m_Charactor_data->n_Hp_Count < m_nCharactor_beforeHp) { 
		m_nCharactor_beforeHp = m_Charactor_data->n_Hp_Count;
		m_bCharactor_pain = true;
		Charactor_Sp->setColor(Color3B::RED);
	}

	switch (m_BossState) {
	case BOSS_IDLE:
		this->Pattern_IDLE();
		break;
	case BOSS_PATTERN_A:
		this->Pattern_A(Charactor_Sp, movetime);
		break;
	case BOSS_PATTERN_B:
		this->Pattern_B(Charactor_Sp, movetime);
		break;
	case BOSS_PATTERN_C:
		if (!m_bCharactor_pain && m_fPlayTime > 2) {
			this->Pattern_C(Charactor_Sp, Charactor->GetPosition());
		}
		break;
	case BOSS_PATTERN_D:
		if (!m_bCharactor_pain) { // 평소 아프지 않은 상태에서만 캐릭터는 보스몬스터 공격에 충격을 받을 수 있다.(모든 프레임마다 공격을 받지 않도록 하기 위해 피격쿨타임이 필요하다.(Monster & Obstacle과 동일))
			this->Pattern_D(Charactor_Sp);
		}
		break;
	}
	if (m_bCharactor_pain && (m_fPainDelay_time -= movetime) <= 0) {
		m_fPainDelay_time = 1.5f;
		m_bCharactor_pain = false;
	}

	if (m_Collide_Effect->getCollideEffects()->size() > 0) {
		std::list<Effecter>::iterator iter;
		for (iter = m_Collide_Effect->getCollideEffects()->begin(); iter != m_Collide_Effect->getCollideEffects()->end();) {
			if ((iter->animation_time -= movetime) <= 0) {
				m_layer->removeChild(iter->EffectSprite, true);
				iter = m_Collide_Effect->getCollideEffects()->erase(iter);
			}
			else ++iter;
		}
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

void BossMonster::Pattern_IDLE() {
	for (Sprite* FallingLava : m_Lava_vc) {
		m_layer->removeChild(FallingLava, true);
	}
	m_Lava_vc.clear();

	if (m_layer->getChildByName("LavaPunch") != nullptr) {
		m_layer->removeChildByName("LavaPunch", true);
	}
	if (m_layer->getChildByName("LavaPunchCrash") != nullptr) {
		m_layer->removeChildByName("LavaPunchCrash", true);
	}

	for (Sprite* Columns : m_Column_vc) {
		m_layer->removeChild(Columns, true);
	}
	for (Sprite* Fires : m_Fire_vc) {
		m_layer->removeChild(Fires, true);
	}
	m_Column_vc.clear();
	m_Fire_vc.clear();
}

void BossMonster::Pattern_A(Sprite* _charactorSprite, float _movetime) {
	for (Sprite* Sp : m_Lava_vc) {
		Sp->setPosition(Vec2(Sp->getPosition().x, Sp->getPosition().y - (_movetime * 300)));
		Rect rect = Sp->getBoundingBox();

		log("Lava %f %f", Sp->getPosition().x, Sp->getPosition().y);
		if (!m_bCharactor_pain) {
			if (rect.intersectsRect(_charactorSprite->getBoundingBox())) {
				m_Charactor_data->n_Hp_Count--;
			}
		}
	}
}

void BossMonster::Pattern_B(Sprite* _charactorSprite, float _movetime) {
	m_Lava_Punch->setPosition(Vec2(m_Lava_Punch->getPosition().x - (_movetime * 500), m_Lava_Punch->getPosition().y));

	Rect rect = m_Lava_Punch->getBoundingBox();
	if (!m_bCharactor_pain) {
		if (rect.intersectsRect(_charactorSprite->getBoundingBox())) {
			m_Charactor_data->n_Hp_Count--;
		}
		else if (rect.intersectsRect(m_Boss_Core_Sprite->getBoundingBox())) {
			m_BossCore_Hp -= 100;
			m_BossState = BOSS_PAIN;
			SpriteChange(m_BossState);
			this->LavaPunchCrash();
		}
	}
}

void BossMonster::Pattern_C(Sprite* _charactorSprite, b2Vec2 _vector) {
	if (m_layer->getChildByName("CreateLightningBall") != nullptr) {
		for (b2Body* LightningBall : m_Lightningbody_vc) {
			Sprite* oldBallSprite = (Sprite*)LightningBall->GetUserData();
			m_layer->removeChild(oldBallSprite, true);

			Sprite* newBallSprite = Sprite::create();
			newBallSprite = this->CreateAnimation("Images/Monsters/Boss/Lightning_Ball_.plist", "Lightning_Ball_%d.png", 10, 0.2, newBallSprite, m_Boss_Core_Sprite->getPosition(), "LightningBall", true);
			LightningBall->SetUserData(newBallSprite);

			float x = rand() % 80 - 40;
			float y = rand() % 80 - 40;
			LightningBall->ApplyForce(b2Vec2(x, y), LightningBall->GetWorldCenter(), true);
		}
	}

	for (b2Body* LightningBall : m_Lightningbody_vc) {
		Sprite* LightningSprite = (Sprite*)LightningBall->GetUserData();

		if (LightningSprite->getBoundingBox().intersectsCircle(_charactorSprite->getPosition(), 0.1)) {
			m_Charactor_data->n_Hp_Count--;
		}
	}
	/*
	if (m_layer->getChildByName("CreateLightningBall") != nullptr) {
		m_layer->removeChildByName("CreateLightningBall", true);
		m_Lightning_Ball = this->CreateAnimation("Images/Monsters/Boss/Lightning_Ball_.plist", "Lightning_Ball_%d.png", 10, 0.2, m_Lightning_Ball, m_Boss_Core_Sprite->getPosition(), "LightningBall", true);
		m_Lightning_Ball->setScale(1.0f);
		m_LightningBall_body->SetUserData(m_Lightning_Ball);
		m_LightningBall_body->GetPosition();

		float x = rand() % 80 - 40;
		float y = rand() % 80 - 40;
		m_LightningBall_body->ApplyForce(b2Vec2(x, y), m_LightningBall_body->GetWorldCenter(), true);

		Rect rect = m_Lightning_Ball->getBoundingBox();
	}
	if (m_Lightning_Ball->getBoundingBox().intersectsCircle(_charactorSprite->getPosition(), 0.1)) {
		m_Charactor_data->n_Hp_Count--;
	}*/
}

void BossMonster::Pattern_D(Sprite* _charactorSprite) {
	if (m_Column_vc.front()->getPosition().y < 64 && m_fPlayTime < 3) {
		for (Sprite* columns : m_Column_vc) {
			columns->setPosition(Vec2(columns->getPosition().x, columns->getPosition().y + 2));
		}
	}
	else if (m_fPlayTime > 3 && m_fPlayTime < 5.5) {
		if (!m_fire_enable) {
			CSoundManager::getInstance()->playEffect(SEF_COLUMN_FIRE);
		}
		int i = 0;
		for (Sprite* columns : m_Column_vc) {
			Sprite* Fire = m_Fire_vc.at(i);
			m_fire_enable = true;
			Fire->setVisible(true);
			Fire->setPosition(columns->getPosition().x - 10, columns->getPosition().y + 200);
			if (Fire->getBoundingBox().intersectsRect(_charactorSprite->getBoundingBox())) {
				m_Charactor_data->n_Hp_Count--;

				m_Collide_Effect->CollideEffect("Images/effects/Fire_Collide_Effect_.plist","Fire_Collide_Effect_%d.png", 5, _charactorSprite->getPosition(), 0.5);
			}
			i++;
		}
	}
	else if (m_fPlayTime > 5.5) {
		int i = 0;
		for (Sprite* columns : m_Column_vc) {
			Sprite* Fire = m_Fire_vc.at(i);
			Fire->setVisible(false);
			columns->setPosition(Vec2(columns->getPosition().x, columns->getPosition().y - 3));
			i++;
		}
	}

}

void BossMonster::CreateFallingLava() {
	int FallingLava_count = rand() % (8 + 1 - 2) + 2;

	SpriteFrameCache* spritecache = SpriteFrameCache::getInstance();
	spritecache->addSpriteFramesWithFile("Images/Monsters/Boss/LavaFall.plist");

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

void BossMonster::CreateLavaPunch(Vec2 _position) {
	m_Lava_Punch = Sprite::create("Images/Monsters/Boss/Lava_Punch.png");
	m_Lava_Punch->setPosition(Vec2(1200, _position.y));
	m_layer->addChild(m_Lava_Punch, 1, "LavaPunch");
}

void BossMonster::CreateLightningBall() {
	Sprite* Create_Lightning_Ball = Sprite::create();
	Create_Lightning_Ball = this->CreateAnimation("Images/Monsters/Boss/Created_Lightning_Ball_.plist", "Created_Lightning_Ball_%d.png", 13, 0.3, Create_Lightning_Ball, m_Boss_Core_Sprite->getPosition(), "CreateLightningBall", false);
	
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(Create_Lightning_Ball->getPosition().x / 32, Create_Lightning_Ball->getPosition().y / 32);

	bodyDef.userData = Create_Lightning_Ball;

	b2Body* LightningBall_body;
	LightningBall_body = m_world->CreateBody(&bodyDef);
	LightningBall_body->SetGravityScale(0.0f);
	LightningBall_body->SetHp(3000);
	
	b2CircleShape circle;
	circle.m_radius = 0.7f;

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &circle;
	fixtureDef.density = 0.1f;
	fixtureDef.friction = 0.0f;
	fixtureDef.restitution = 1.1f;
	fixtureDef.filter.categoryBits = LIGHTNING_CATEGORY;
	fixtureDef.filter.maskBits = 0x0022;

	LightningBall_body->CreateFixture(&fixtureDef);

	m_Lightningbody_vc.push_back(LightningBall_body);
}

void BossMonster::CreateFireColumn() {
	int nFireColumn_count = rand() % (5 + 1 - 2) + 2;

	for (int i = 0; i < nFireColumn_count; i++) {
		Sprite* FireColumn = Sprite::create("Images/Monsters/Boss/Fire_column.png");
		FireColumn->setPosition(Vec2(100 * (rand() % 10), -100));

		m_layer->addChild(FireColumn, 2);

		m_Column_vc.pushBack(FireColumn);
	}

	SpriteFrameCache* spritecache = SpriteFrameCache::getInstance();
	spritecache->addSpriteFramesWithFile("Images/Monsters/Boss/Fire_column_medium_.plist");

	Vector<SpriteFrame*> animframes;
	char str[100];
	for (int i = 0; i < 15; i++) {
		if (i >= 3 && i <= 5) {
			for (int j = 0; j < 5; j++) {
				for (int k = 3; k <= 5; k++) {
					sprintf(str, "fire_column_medium_%d.png", k);
					animframes.pushBack(spritecache->getSpriteFrameByName(str));
				}
			}
		}
		else {
			sprintf(str, "fire_column_medium_%d.png", i);
			animframes.pushBack(spritecache->getSpriteFrameByName(str));
		}
	}


	for (Sprite* columns : m_Column_vc) {
		Sprite* Fire_Sprite = Sprite::createWithSpriteFrame(animframes.front());
		Animation* animation = Animation::createWithSpriteFrames(animframes, 0.05f);
		Animate* animate = Animate::create(animation);
		RepeatForever* rep = RepeatForever::create(animate);
		Fire_Sprite->runAction(rep);

		Fire_Sprite->setPosition(Vec2(columns->getPosition().x, 100));

		Fire_Sprite->setVisible(false);
		m_fire_enable = false;				 // 기둥에서 발사되는 불은 생성을 미리 해놓지만 기둥이 적정 Y좌표에 도달하기 전까지는 발사되면 안되기 때문에 생성'만' 해놓고 Sprite invisible 및 disable 처리해준다.
		Fire_Sprite->setScaleY(2.0f);
		m_layer->addChild(Fire_Sprite, 1);
		m_Fire_vc.pushBack(Fire_Sprite);
	}
}

void BossMonster::LavaPunchCrash() {
	this->CreateAnimation("Images/Monsters/Boss/Lava_Punch_Broken.plist", "Lava_Punch_Broken_%d.png", 9, 0.1, m_Lava_Punch_crash, m_Lava_Punch->getPosition(), "LavaPunchCrash", false);

	m_layer->removeChild(m_Lava_Punch, true);
}

Sprite* BossMonster::CreateAnimation(const std::string& _sPath, const char* _cNames, int _nSize, float _fFrameTime, Sprite* _Sprite, Vec2 position, const std::string& _sNodename, bool _repeatForever) {
	SpriteFrameCache* SFC = SpriteFrameCache::getInstance();

	SFC->addSpriteFramesWithFile(_sPath);

	Vector<SpriteFrame*> animFrames;
	char str[100];
	for (int i = 0; i < _nSize; i++) {
		sprintf(str, _cNames, i);
		animFrames.pushBack(SFC->getSpriteFrameByName(str));
	}

	_Sprite = Sprite::createWithSpriteFrame(animFrames.front());
	Animation* animation = Animation::createWithSpriteFrames(animFrames, _fFrameTime);
	Animate* animate = Animate::create(animation);
	if (_repeatForever) {
		RepeatForever* repF = RepeatForever::create(animate);
		_Sprite->runAction(repF);
	}
	else {
		Repeat* rep = Repeat::create(animate, 1);
		_Sprite->runAction(rep);
	}

	_Sprite->setAnchorPoint(Vec2(0.5, 0.5));
	_Sprite->setPosition(position);

	m_layer->addChild(_Sprite, 1, _sNodename);

	return _Sprite;
}