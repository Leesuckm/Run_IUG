#include "CharactorCreator.h"
#include "Charactor_Hand.h"
#include "EffectSprite.h"
#include "LightEffect.h"

CharactorCreator::CharactorCreator() {
	gNumFootContacts = 0;
	m_sPlayer = new ESorSP;
}

CharactorCreator::CharactorCreator(Layer* _wlayer, Layer* _clayer) {
	m_wlayer = _wlayer;
	m_clayer = _clayer;

	m_sPlayer = new ESorSP;
	gNumFootContacts = 0;
}

CharactorCreator::~CharactorCreator() {
	delete m_Charactor_hand;
	delete m_sPlayer;
	//delete m_oldsprite;
}

void CharactorCreator::CreateCharactor(b2World* _world, Charactor_Data _Charactor_Data, Layer* layer, LightEffect* _effect) {
	m_Charactor_data = _Charactor_Data;

	m_world = _world;
	m_effect = _effect;

	m_Charactor_hand = new Charactor_Hand(_Charactor_Data);

	auto spritecache = SpriteFrameCache::getInstance();
	spritecache->addSpriteFramesWithFile("Images/sprites/lantern_idle/fox_idle_.plist");

	Vector<SpriteFrame*> animFrames;
	char str[100];
	for (int i = 0; i < 4; i++) {
		sprintf(str, "fox_idle_%d.png", i);
		animFrames.pushBack(spritecache->getSpriteFrameByName(str));
	}

	//Sprite* Player_S
	Animation* animation = Animation::createWithSpriteFrames(animFrames, 0.1f);
	Animate* animate = Animate::create(animation);
	RepeatForever* rep = RepeatForever::create(animate);
	// charactor_data에 bool m_blind 넣어서 다시 싹 모든 Shelter에 || m_blind 처리 해줘야한다.
	if (_Charactor_Data.StageNumber == SHELTER || _Charactor_Data.bStage_blind) {
		m_sPlayer->ESpriteData = EffectSprite::createWithSpriteFrame(animFrames.front());
		//_effect->setAmbientLightColor(Color3B(20, 20, 20));
		m_sPlayer->ESpriteData->setColor(_effect->getAmbientLightColor());
		m_sPlayer->ESpriteData->setEffect(_effect, "Images/sprites/lantern_idle/fox_idle_n.png");
		m_sPlayer->ESpriteData->runAction(rep);
		layer->addChild(m_sPlayer->ESpriteData, 3);

		m_MyBodyDef.userData = m_sPlayer->ESpriteData;
	}
	else {
		m_sPlayer->SpriteData = Sprite::createWithSpriteFrame(animFrames.front());
		m_sPlayer->SpriteData->setTag(1818);
		int num = m_sPlayer->SpriteData->getTag();
		m_sPlayer->SpriteData->runAction(rep);
		layer->addChild(m_sPlayer->SpriteData, 3);

		m_MyBodyDef.userData = m_sPlayer->SpriteData;
	}
	

	m_MyBodyDef.type = b2_dynamicBody;
	polygonShape.SetAsBox(0.3f, 0.3f);

	myFixtureDef.shape = &polygonShape; // 여기에 추가된 Shape를 써서 기존 body에 또 fixture를 넣어야만 적용됨.
	myFixtureDef.density = 1.0f;
	myFixtureDef.friction = 0.0f;
	myFixtureDef.restitution = 0.0;
	
	myFixtureDef.filter.categoryBits = 0x0001;
	myFixtureDef.filter.maskBits = 0x0FFF;

	m_MyBodyDef.position.Set(_Charactor_Data.Spawn_Point.x, _Charactor_Data.Spawn_Point.y);
	//m_MyBodyDef.userData = m_sPlayer;

	m_Charactorbody = _world->CreateBody(&m_MyBodyDef);
	m_Charactorbody->CreateFixture(&myFixtureDef);

	m_Charactorbody->SetIsCh(true);

	b2Fixture* SensorFixture = m_Charactorbody->CreateFixture(&myFixtureDef);
	
	SensorFixture->SetUserData((void*)1);

	b2Vec2 footvertices[3];
	footvertices[0] = b2Vec2(0, -0.4f);
	footvertices[1] = b2Vec2(0.2f, -0.35f);
	footvertices[2] = b2Vec2(-0.2f, -0.35f);

	b2Vec2 rightvertices[5];
	rightvertices[0] = b2Vec2(0.35f, -0.35f);
	rightvertices[1] = b2Vec2(0.4f, -0.2f);
	rightvertices[2] = b2Vec2(0.45f, 0);
	rightvertices[3] = b2Vec2(0.4f, -0.2f);
	rightvertices[4] = b2Vec2(0.35f, 0.35f);

	b2Vec2 leftvertices[5];
	leftvertices[0] = b2Vec2(-0.35f, 0.35f);
	leftvertices[1] = b2Vec2(-0.4f, 0.2f);
	leftvertices[2] = b2Vec2(-0.45f, 0);
	leftvertices[3] = b2Vec2(-0.4f, -0.2f);
	leftvertices[4] = b2Vec2(-0.35f, -0.35f);

	// 다른 Stage와는 다르게 2 Stage는 얼음컨셉이기때문에 미끄러지도록 발 부분의 body 마찰력이 다르게 설정된다.
	if (_Charactor_Data.StageNumber >= STAGE_1_1 && _Charactor_Data.StageNumber <= STAGE_1_3) {
		CharactorSensor(footvertices, 3, b2Vec2(0, -0.35f), 3, 5.0f, false); // foot (shape, size ,position, userdata, friction, sensor)
	}
	else if (_Charactor_Data.StageNumber >= STAGE_2_1 && _Charactor_Data.StageNumber <= STAGE_2_3) {
		CharactorSensor(footvertices, 3, b2Vec2(0, -0.35f), 3, 0.05f, false);
	}
	else {
		CharactorSensor(footvertices, 3, b2Vec2(0, -0.35f), 3, 5.0f, false);
	}

	CharactorSensor(leftvertices, 5, b2Vec2(0.5f, 0), 4, 0.0001f, false);
	CharactorSensor(rightvertices, 5, b2Vec2(-0.5f, 0), 4, 0.0001f, false); 

	m_Charactorbody->SetFixedRotation(true);

	CreateCameraSprite(_world);

	m_MoveState = MS_STOP;
	//CreateHeart(_Charactor_Data.n_Hp_Count); // 시작 생명력 Heart_Count개
	//CreateBomb(_Charactor_Data.n_Bomb_Count);
	//CreateLadder(_Charactor_Data.n_Ladder_Count);
}

void CharactorCreator::CharactorSensor(b2Vec2* vertices, int size, b2Vec2 position, int part, float fric, bool sensor) {
	
	polygonShape.Set(vertices, size);

	if (sensor) {
		polygonShape.SetAsBox(1, 1, position, 0);
	}
	myFixtureDef.isSensor = sensor;
	myFixtureDef.friction = fric;
	//myFixtureDef.filter.categoryBits = 0x0000;
	//myFixtureDef.filter.categoryBits = 0xFFFF;
	b2Fixture* footSensorFixture = m_Charactorbody->CreateFixture(&myFixtureDef);
	footSensorFixture->SetUserData((void*)part);
}

void CharactorCreator::CreateCameraSprite(b2World* _world) {

	EffectSprite* CameraSp = EffectSprite::create("Images/Camera.png");
	CameraSp->setScale(0.3f);
	CameraSp->setVisible(true);
	m_MyCameraDef.type = b2_dynamicBody;

	m_MyCameraDef.position.Set(m_MyBodyDef.position.x, m_MyBodyDef.position.y);
	m_MyCameraDef.userData = CameraSp;

	b2CircleShape circle;
	circle.m_radius = 0.3f;

	b2FixtureDef CameraFixture;
	CameraFixture.filter.categoryBits = 0x0006;
	CameraFixture.filter.maskBits = 0x000E;
	CameraFixture.isSensor = true;
	CameraFixture.shape = &circle;
	CameraFixture.density = 0.1f;
	CameraFixture.friction = 0.0f;
	CameraFixture.restitution = 0;

	m_Camerabody = _world->CreateBody(&m_MyCameraDef);
	m_Camerabody->CreateFixture(&CameraFixture);
	m_Camerabody->SetHp(30000);
}

void CharactorCreator::CreateHeart(int nHeart) {
	m_nHeartCount = nHeart;
	m_Charactorbody->SetHp(m_nHeartCount);

	auto texture = Director::getInstance()->getTextureCache()->addImage("Images/Heart.png");
	m_sHeart = Sprite::createWithTexture(texture, Rect(0, 0, 32, 32));
	m_sHeart->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	m_sHeart->setPosition(Vec2(0, 310));
	m_clayer->addChild(m_sHeart, 5);
}

void CharactorCreator::CreateBomb(int nBomb) {
	m_nBombCount = nBomb;
	auto texture = Director::getInstance()->getTextureCache()->addImage("Images/Bomb.png");
	m_sBomb = Sprite::createWithTexture(texture, Rect(0, 0, 34, 32));
	m_sBomb->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	m_sBomb->setPosition(Vec2(40, 310));
}

void CharactorCreator::CreateLadder(int nLadder) {
	m_nLadderCount = nLadder;
	auto texture = Director::getInstance()->getTextureCache()->addImage("Images/Ladder.png");
	m_sLadder = Sprite::createWithTexture(texture, Rect(0, 0, 32, 32));
	m_sLadder->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	m_sLadder->setPosition(Vec2(80, 310));
}

void CharactorCreator::Heartmanager(Layer* layer, int heartcount) {
	m_nHeartCount = heartcount;
	m_Charactorbody->SetHp(heartcount);
	std::string Heart = std::to_string(m_Charactorbody->GetHp());
	layer->removeChild(m_HeartLabel, true);
	m_HeartLabel = Label::createWithSystemFont(Heart, "", 23);
	m_HeartLabel->setPosition(m_sHeart->getPosition() + Vec2(15,-15));
	m_HeartLabel->setColor(Color3B(255, 255, 255));
	layer->addChild(m_HeartLabel, 5);
}

void CharactorCreator::Bombmanager(Layer* layer, int bombcount) {
	std::string Bomb = std::to_string(bombcount);
	layer->removeChild(m_BombLabel, true);
	m_BombLabel = Label::createWithSystemFont(Bomb, "", 23);
	m_BombLabel->setPosition(m_sBomb->getPosition() + Vec2(15, -15));
	m_BombLabel->setColor(Color3B(255, 255, 255));
	layer->addChild(m_BombLabel, 5);
}

void CharactorCreator::Laddermanager(Layer* layer, int laddercount) {
	std::string Ladder = std::to_string(laddercount);
	layer->removeChild(m_LadderLabel, true);
	m_LadderLabel = Label::createWithSystemFont(Ladder, "", 23);
	m_LadderLabel->setPosition(m_sLadder->getPosition() + Vec2(15, -15));
	m_LadderLabel->setColor(Color3B(255, 255, 255));
	layer->addChild(m_LadderLabel, 5);
}


ESorSP* CharactorCreator::AnimationState(_moveState state, Layer* layer) {
	int nSheetmx;
	std::string sPath;
	std::string sPath_n;
	switch (state) {
	case MS_LEFT:
		nSheetmx = 6;
		sPath = "Images/sprites/lantern_run/fox_run_.plist";
		sPath_n = "Images/sprites/lantern_run/fox_run_n.png";
		break;
	case MS_RIGHT:
		nSheetmx = 6;
		sPath = "Images/sprites/lantern_run/fox_run_.plist";
		sPath_n = "Images/sprites/lantern_run/fox_run_n.png";
		break;
	case MS_STOP:
		nSheetmx = 4;
		sPath = "Images/sprites/lantern_idle/fox_idle_.plist";
		sPath_n = "Images/sprites/lantern_idle/fox_idle_n.png";
		break;
	case MS_UPDOWN:
		nSheetmx = 2;
		sPath = "Images/sprites/lantern_climb/fox_climb_.plist";
		sPath_n = "Images/sprites/lantern_climb/fox_climb_n.png";
		break;
	case MS_JUMP:
		nSheetmx = 1;
		sPath = "Images/sprites/lantern_jump/fox_jump1_.plist";
		sPath_n = "Images/fox_jump1_n.png";
		break;
	case MS_JUMPDOWN:
		nSheetmx = 1;
		sPath = "Images/sprites/lantern_jump/fox_jump2_.plist";
		sPath_n = "Images/fox_jump2_n.png";
		break;
	case MS_PAIN:
		nSheetmx = 1;
		sPath = "Images/sprites/lantern_pain/fox_pain_.plist";
		sPath_n = "Images/sprites/lantern_pain/fox_pain_n.png";
		break;
	default:
		nSheetmx = 6;
		sPath = "Images/sprites/lantern_run/fox_run_.plist";
		sPath_n = "Images/sprites/lantern_run/fox_run_n.png";
		break;
		log("state error");
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
			sprintf(str, "fox_run_%d.png", i);
			animFrames.pushBack(spritecache->getSpriteFrameByName(str));
			break;
		case MS_STOP:
			sprintf(str, "fox_idle_%d.png", i);
			animFrames.pushBack(spritecache->getSpriteFrameByName(str));
			break;
		case MS_UPDOWN:
			sprintf(str, "fox_climb_%d.png", i);
			animFrames.pushBack(spritecache->getSpriteFrameByName(str));
			break;
		case MS_JUMP:
			sprintf(str, "fox_jump1.png");
			animFrames.pushBack(spritecache->getSpriteFrameByName(str));
			break;
		case MS_JUMPDOWN:
			sprintf(str, "fox_jump2.png");
			animFrames.pushBack(spritecache->getSpriteFrameByName(str));
			break;
		case MS_PAIN:
			sprintf(str, "fox_pain_%d.png", i);
			animFrames.pushBack(spritecache->getSpriteFrameByName(str));
			break;
		default:
			sprintf(str, "fox_run_%d.png", i);
			animFrames.pushBack(spritecache->getSpriteFrameByName(str));
			break;
			log("state error");
			break;
		}
	}

	Animation* animation = Animation::createWithSpriteFrames(animFrames, 0.5f / 4);
	Animate* animate = Animate::create(animation);
	RepeatForever* rep = RepeatForever::create(animate);

	if (m_Charactor_data.StageNumber == SHELTER || m_Charactor_data.bStage_blind) {
		m_sPlayer->ESpriteData = EffectSprite::createWithSpriteFrame(animFrames.front());

		m_sPlayer->ESpriteData->setColor(m_effect->getAmbientLightColor());
		m_sPlayer->ESpriteData->setEffect(m_effect, "Images/sprites/lantern_idle/fox_idle_n.png"); // 처음 초기화할때는 Sprite Data로 이루어져 있지만 setEffect를 통해 기존 SpriteData를 NomalMap Sprite Data로 교체해서 LightEffect를 얻는다.
		m_sPlayer->ESpriteData->runAction(rep);
		layer->addChild(m_sPlayer->ESpriteData, 3);

		m_Charactorbody->SetUserData(m_sPlayer->ESpriteData);
	}
	else {
		m_sPlayer->SpriteData = EffectSprite::createWithSpriteFrame(animFrames.front());
		int num = m_sPlayer->SpriteData->getTag();
		m_sPlayer->SpriteData->runAction(rep);
		layer->addChild(m_sPlayer->SpriteData, 3);

		m_Charactorbody->SetUserData(m_sPlayer->SpriteData);
	}


	return m_sPlayer;
}

void CharactorCreator::SpriteChange(_moveState state, Layer* layer) {
	m_oldsprite = new ESorSP;
	m_newSprite = CharactorCreator::AnimationState(state, layer);

	if (m_Charactor_data.StageNumber == SHELTER || m_Charactor_data.bStage_blind) {

		m_oldsprite->ESpriteData = static_cast<EffectSprite*>(m_Charactorbody->GetUserData());
		m_Charactorbody->SetUserData(nullptr);
		layer->removeChild(m_oldsprite->ESpriteData, true);

		m_Charactorbody->SetUserData(m_newSprite->ESpriteData);

		if (state == MS_LEFT) {
			m_newSprite->ESpriteData->setFlippedX(true);
		}

		else if (state == MS_RIGHT) {
			m_newSprite->ESpriteData->setFlippedX(false);
		}
	}
	else {
		m_oldsprite->SpriteData = static_cast<Sprite*>(m_Charactorbody->GetUserData());
		m_Charactorbody->SetUserData(nullptr);
		layer->removeChild(m_oldsprite->SpriteData, true);
		layer->removeChild(m_sPlayer->SpriteData, true);

		m_Charactorbody->SetUserData(m_newSprite->SpriteData);

		if (state == MS_LEFT) {
			m_newSprite->SpriteData->setFlippedX(true);
		}

		else if (state == MS_RIGHT) {
			m_newSprite->SpriteData->setFlippedX(false);
		}
	}
}

ESorSP* CharactorCreator::Pickup(std::string* Item, _moveState state, Layer* layer) {
	std::string sPath;
	ESorSP* SpriteData;
	if (*Item == "Pickaxe") {
		sPath = "Images/sprites/pickaxe_idle/fox_pickaxe_idle_.plist";
		layer->removeChild((Sprite*)m_Charactorbody->GetUserData(), true);
		m_Charactor_hand->Holeinone_LeftHand(sPath, *Item, m_Charactorbody, state,layer, m_world, m_effect);
		SpriteData = m_Charactor_hand->AnimationState("Pickaxe", Hand_On, layer, g_bViewing);
		m_Charactor_hand->setDurability(30); // Pickaxe의 내구도
	}
	else if (*Item == "Bomb") {
		sPath = "Images/sprites/bomb_throw/fox_bomb_throw_.plist";
		m_Charactor_hand->Holeinone_LeftHand(sPath, *Item, m_Charactorbody, state, layer, m_world, m_effect);
		SpriteData = m_Charactor_hand->AnimationState("Pickaxe", THROW_BOMB, layer, g_bViewing);
	}
	else if (*Item == "Sword") {
		sPath = "Images/sprites/sword_idle/fox_sword_idle_.plist";
		layer->removeChild((Sprite*)m_Charactorbody->GetUserData(), true);
		m_Charactor_hand->Holeinone_LeftHand(sPath, *Item, m_Charactorbody, state, layer, m_world, m_effect);
		SpriteData = m_Charactor_hand->AnimationState("Sword",Hand_On, layer, g_bViewing);
	}
	else if (*Item == "Gun") {
		sPath = "Images/sprites/gun_idle/fox_gun_idle_.plist";
		layer->removeChild((Sprite*)m_Charactorbody->GetUserData(), true);
		m_Charactor_hand->Holeinone_LeftHand(sPath, *Item, m_Charactorbody, state, layer, m_world, m_effect);
		SpriteData = m_Charactor_hand->AnimationState("Gun", Hand_On, layer, g_bViewing);
	}

	return SpriteData;
}

ESorSP* CharactorCreator::Pickup(std::string* Item, _moveState state, Layer* layer, bool _blind) {
	std::string sPath;
	ESorSP* SpriteData;
	if (*Item == "Pickaxe") {
		sPath = "Images/sprites/pickaxe_idle/fox_pickaxe_idle_.plist";
		layer->removeChild((Sprite*)m_Charactorbody->GetUserData(), true);
		m_Charactor_hand->Holeinone_LeftHand(sPath, *Item, m_Charactorbody, state, layer, m_world, m_effect);
		SpriteData = m_Charactor_hand->AnimationState("Pickaxe", Hand_On, layer, g_bViewing);
		m_Charactor_hand->setDurability(30); // Pickaxe의 내구도
	}
	else if (*Item == "Bomb") {
		sPath = "Images/sprites/bomb_throw/fox_bomb_throw_.plist";
		m_Charactor_hand->Holeinone_LeftHand(sPath, *Item, m_Charactorbody, state, layer, m_world, m_effect);
		SpriteData = m_Charactor_hand->AnimationState("Pickaxe", THROW_BOMB, layer, g_bViewing);
	}
	else if (*Item == "Sword") {
		sPath = "Images/sprites/sword_idle/fox_sword_idle_.plist";
		layer->removeChild((Sprite*)m_Charactorbody->GetUserData(), true);
		m_Charactor_hand->Holeinone_LeftHand(sPath, *Item, m_Charactorbody, state, layer, m_world, m_effect);
		SpriteData = m_Charactor_hand->AnimationState("Sword", Hand_On, layer, g_bViewing);
	}
	else if (*Item == "Gun") {
		sPath = "Images/sprites/gun_idle/fox_gun_idle_.plist";
		layer->removeChild((Sprite*)m_Charactorbody->GetUserData(), true);
		m_Charactor_hand->Holeinone_LeftHand(sPath, *Item, m_Charactorbody, state, layer, m_world, m_effect);
		SpriteData = m_Charactor_hand->AnimationState("Gun", Hand_On, layer, g_bViewing);
	}

	return SpriteData;
}