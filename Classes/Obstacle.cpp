#include "Obstacle.h"
#include "EffectSprite.h"
#include "LightEffect.h"

Obstacle::Obstacle() {
	m_fLava_Eruptiontime = 1.5f;
}

Obstacle::Obstacle(b2World* _world, Layer* _layer) {
	m_world = _world;
	m_layer = _layer;

	m_fLava_Eruptiontime = 1.5f;
}

Obstacle::Obstacle(b2World* _world, Layer* _layer, LightEffect* _lighteffect) {
	m_world = _world;
	m_layer = _layer;

	m_fLava_Eruptiontime = 1.5f;
	m_lighteffect = _lighteffect;
}

Obstacle::~Obstacle() {
}

void Obstacle::CreateObstacle(Charactor_Data* _charactor_data) {
	m_Charactor_data = _charactor_data;

	this->CreateLava();

	for (b2Body* b = m_world->GetBodyList(); b; b = b->GetNext()) {
		if (b->GetUserData() != nullptr) {
			b2Body* Obstacle_Body;
			INT16 category = b->GetFixtureList()->GetFilterData().categoryBits;
			Sprite* BladeSprite;
			Sprite* SpriteUserdata;
			b2BodyDef bodydef;
			b2FixtureDef fixture;
			b2Fixture* SensorFixture;
			b2PrismaticJointDef priJointDef;
			b2PrismaticJoint* priJoint;
			b2WeldJointDef weldjointDef;
			Sprite* LopeSprite;
			BladeObstacle* bladeObstacle = new BladeObstacle;
			GunObstacle* gunObstacle = new GunObstacle;


			b2Vec2 vertices[4];
			int vindex = 4;

			b2PolygonShape* rectshape = new b2PolygonShape;

			switch (category) {
			case BLADEBLOCK_CATEGORY:	// 기존의 BaseBody에 BladeBody를 생성해 PrismaticJoint로 연결해준다.
				if (m_Charactor_data->bStage_blind) {
					CreatefallingBlade(b, m_Charactor_data->bStage_blind);
				}
				else {
					CreatefallingBlade(b);
				}
				break;
			case RGUNTRAP_CATEGORY: // Trap의 인식범위를 설정하기 위해 Sprite를 BaseBody Userdata의 자식으로 붙여준다.
				SpriteUserdata = (Sprite*)b->GetUserData();
				
				gunObstacle->RecognizeSprite = Sprite::create("Images/Obstacle/GunTrap_Recognize.png");
				gunObstacle->RecognizeSprite->setAnchorPoint(Vec2(0, 0.5));
				gunObstacle->RecognizeSprite->setScaleX(2.0f);
				gunObstacle->RecognizeSprite->setScaleY(0.5f);
				m_layer->addChild(gunObstacle->RecognizeSprite, 4);

				bodydef.userData = gunObstacle->RecognizeSprite;
				bodydef.position = b->GetPosition() + b2Vec2(0, 0.5f);
				bodydef.type = b2_staticBody;

				Obstacle_Body = m_world->CreateBody(&bodydef);
				Obstacle_Body->SetHp(1000);
				Obstacle_Body->SetIsCh(false);

				rectshape->SetAsBox(0.2, 0.2);
				fixture.shape = rectshape;
				fixture.isSensor = false;
				fixture.filter.categoryBits = 0x0000;
				fixture.filter.maskBits = 0x000E;

				SensorFixture = Obstacle_Body->CreateFixture(&fixture);

				weldjointDef.Initialize(b, Obstacle_Body, b->GetPosition());
				m_world->CreateJoint(&weldjointDef);

				gunObstacle->BaseBody = b;
				gunObstacle->RecognizeBody = Obstacle_Body;

				lb_GunOb.push_back(gunObstacle);
				break;
			case LGUNTRAP_CATEGORY:
				SpriteUserdata = (Sprite*)b->GetUserData();

				gunObstacle->RecognizeSprite = Sprite::create("Images/Obstacle/GunTrap_Recognize.png");
				gunObstacle->RecognizeSprite->setAnchorPoint(Vec2(1, 0.5));
				gunObstacle->RecognizeSprite->setScaleX(2.0f);
				gunObstacle->RecognizeSprite->setScaleY(0.5f);
				m_layer->addChild(gunObstacle->RecognizeSprite, 4);

				bodydef.userData = gunObstacle->RecognizeSprite;
				bodydef.position = b->GetPosition() + b2Vec2(0, 0.5f);
				bodydef.type = b2_staticBody;

				Obstacle_Body = m_world->CreateBody(&bodydef);
				Obstacle_Body->SetHp(1000);
				Obstacle_Body->SetIsCh(false);

				rectshape->SetAsBox(0.2, 0.2);
				fixture.shape = rectshape;
				fixture.isSensor = false;
				fixture.filter.categoryBits = 0x0000;
				fixture.filter.maskBits = 0x000E;

				SensorFixture = Obstacle_Body->CreateFixture(&fixture);

				weldjointDef.Initialize(b, Obstacle_Body, b->GetPosition());
				m_world->CreateJoint(&weldjointDef);

				gunObstacle->BaseBody = b;
				gunObstacle->RecognizeBody = Obstacle_Body;

				lb_GunOb.push_back(gunObstacle);
				break;
			default:
				break;
			}
		}
	}
}

Charactor_Data* Obstacle::FallingBladeControl(int contact, float dt, float* _delaytime, b2Body* _charactor_body) {
	std::list<BladeObstacle*>::iterator iter;

	// 게임 시작 후 몇프레임 정도 Blade Obstacle intersectRect에 캐릭터 Sprite boundingBox가 true로 반환되는 문제를 해결하기 위해 시작 직후 2초 정도 딜레이를 준다.
	if ((m_fStartDelay -= dt) <= 0) {
		for (iter = lb_BladeOb.begin(); iter != lb_BladeOb.end(); ++iter) {
			BladeObstacle* obstacle = *iter;
			if (contact > 0) {
				b2PrismaticJoint* prismatic = (b2PrismaticJoint*)obstacle->BaseBody->GetJointList()->joint;
				prismatic->SetMotorSpeed(-10);
				m_fObstacleDelayTime = 5;
			}
			else if (contact < 1 && (m_fObstacleDelayTime -= dt) <= 0) {
			b2PrismaticJoint* prismatic = (b2PrismaticJoint*)obstacle->BaseBody->GetJointList()->joint;
			prismatic->SetMotorSpeed(20);
			}

			m_fLopelength = ((obstacle->BaseBody->GetPosition().y * 32) - (obstacle->BladeBody->GetPosition().y * 32));
			log("length = %f", m_fLopelength);
			if (m_fLopelength < 0)
				m_fLopelength = 0;
			else if (m_fLopelength > 120)
				m_fLopelength = 120;

			obstacle->RopeSprite->setTextureRect(Rect(0, 120, 10, -m_fLopelength));

			Sprite* blade_Sprite = (Sprite*)obstacle->BladeBody->GetUserData();
			Sprite* charactor_Sprite = (Sprite*)_charactor_body->GetUserData();
			//log("char pos %f , %f", charactor_Sprite->getPosition().x, charactor_Sprite->getPosition().y);

			Rect rect = blade_Sprite->getBoundingBox();
			if (rect.intersectsRect(charactor_Sprite->getBoundingBox()) && *_delaytime <= 0) {
				*_delaytime = 1.5;
				m_Charactor_data->n_Hp_Count--;
			}
		}
		*_delaytime -= dt;
	}

	if (m_fStartDelay < -10) {
		m_fStartDelay = -10;
	}
	return m_Charactor_data;
}

void Obstacle::GunTrapControl(int contect, float dt, float* _delaytime, Charactor_Data* _charactor_data, b2Body* _charactor_body, int* _contactBullet) {
	std::list<GunObstacle*>::iterator iter;
	Sprite* charactor_sprite = (Sprite*)_charactor_body->GetUserData();
	if ((m_fBulletFireDelay -= dt) <= 0) {
		// GunTrap의 인식 범위에 캐릭터가 인식되었을때 총알 생성 및 총알 list에 저장
		for (iter = lb_GunOb.begin(); iter != lb_GunOb.end(); ++iter) {
			b2BodyDef bodydef;
			b2Body* Trapbullet_Body;
			GunObstacle* gunObstacle = *iter;

			Rect rect = gunObstacle->RecognizeSprite->getBoundingBox();
			//	log("char pos %f , %f", charactor_sprite->getPosition().x, charactor_sprite->getPosition().y);

			if (charactor_sprite->getPosition().x == 0) { // 1 프레임에 중복 호출되어 Sprite의 교체가 이루어지면 중간에 position값을 읽어오지 못하는 경우가 발생한다. Userdata의 position 값을 잃는 경우를 무시하기 위해.
				return;
			}

			b2FixtureDef fixture;
			b2Fixture* SensorFixture;

			b2CircleShape* rectshape = new b2CircleShape;

			// GunTrap 인식 범위 Sprite에 Charactor Sprite가 겹치면 bullet body 생성
			if (rect.intersectsRect(charactor_sprite->getBoundingBox())) {
				if (m_Charactor_data->bStage_blind) {
					GunTrapFireAnimation(gunObstacle, "n_Images/obstacle/GunTrap_Fire_.plist", "n_Images/obstacle/GunTrap_Fire_n.png", "GunTrap_Fire_%d.png" , m_lighteffect);
				}
				else {
					log("%d", m_Charactor_data->StageNumber);
					if (m_Charactor_data->StageNumber >= STAGE_1_1 && m_Charactor_data->StageNumber <= STAGE_1_3) {
						GunTrapFireAnimation(gunObstacle, "Images/obstacle/GunTrap_Fire_.plist", "GunTrap_Fire_%d.png");
					}
					else if (m_Charactor_data->StageNumber >= STAGE_2_1 && m_Charactor_data->StageNumber <= STAGE_2_3) {
						GunTrapFireAnimation(gunObstacle, "Images/obstacle/GunTrap_Fire_ice_.plist", "GunTrap_Fire_ice_%d.png");
					}
					else if (m_Charactor_data->StageNumber >= STAGE_3_1 && m_Charactor_data->StageNumber <= STAGE_3_3) {
						GunTrapFireAnimation(gunObstacle, "Images/obstacle/GunTrap_Fire_Rock_.plist", "GunTrap_Fire_Rock_%d.png");
					}
				}
				Sprite* TrapBullet_Sprite = Sprite::create("Images/obstacle/TrapBullet.png");
				m_layer->addChild(TrapBullet_Sprite, 3);

				bodydef.userData = TrapBullet_Sprite;
				if (gunObstacle->BaseBody->GetFixtureList()->GetFilterData().categoryBits == LGUNTRAP_CATEGORY) {
					bodydef.position = gunObstacle->BaseBody->GetPosition() + b2Vec2(-1.3, 0.5);
				}
				else {
					bodydef.position = gunObstacle->BaseBody->GetPosition() + b2Vec2(1.3, 0.5);
				}
				bodydef.type = b2_dynamicBody;

				Trapbullet_Body = m_world->CreateBody(&bodydef);
				Trapbullet_Body->SetHp(1000);
				Trapbullet_Body->SetIsCh(false);

				rectshape->m_radius = 0.1f;
				fixture.shape = rectshape;
				fixture.isSensor = false;
				fixture.filter.categoryBits = TRAPBULLET_CATEGORY;
				fixture.filter.maskBits = 0x000E;
				SensorFixture = Trapbullet_Body->CreateFixture(&fixture);
				SensorFixture->SetUserData((void*)TRAPBULLET_CONTACT);

				//Trapbullet_Body->ApplyLinearImpulse(b2Vec2(30, 0), b2Vec2(Trapbullet_Body->GetWorldCenter()), true);
				Trapbullet_Body->SetGravityScale(0.0f);
				if (gunObstacle->BaseBody->GetFixtureList()->GetFilterData().categoryBits == LGUNTRAP_CATEGORY) {
					Trapbullet_Body->SetLinearVelocity(b2Vec2(-10, 0));
				}
				else {
					Trapbullet_Body->SetLinearVelocity(b2Vec2(10, 0));
				}

				lb_TrapBullet.push_back(Trapbullet_Body);
				m_fBulletFireDelay = 2;
			}
		}
	}
	if (m_fBulletFireDelay < -10) {
		m_fBulletFireDelay = 2;
	}

	// 총알 충돌 처리
	if (GunBulletCollision(charactor_sprite, _contactBullet) && *_delaytime <= 0) {
		*_delaytime = 1.5;
		_charactor_data->n_Hp_Count--;
	}

	if(m_EffectAnimation.getGunTrapEffects()->size() > 0){
		std::list<Effecter>::iterator iter;
		for (iter = m_EffectAnimation.getGunTrapEffects()->begin(); iter != m_EffectAnimation.getGunTrapEffects()->end();) {
			if ((iter->animation_time -= dt) <= 0) {
				m_layer->removeChild(iter->EffectSprite);
				iter = m_EffectAnimation.getGunTrapEffects()->erase(iter);
			}
			else
				++iter;
		}
	}

}

void Obstacle::GunTrapFireAnimation(GunObstacle* _gunObstacle, const std::string& _sPlist_Path, const char* _sFilename_Path) {
	int nSheetmx = 6;

	SpriteFrameCache* spritecache = SpriteFrameCache::getInstance();
	spritecache->addSpriteFramesWithFile(_sPlist_Path);

	Vector<SpriteFrame*> animFrames;
	char str[100];
	for (int i = 0; i < nSheetmx; i++) {
		sprintf(str, _sFilename_Path, i);
		animFrames.pushBack(spritecache->getSpriteFrameByName(str));
	}

	Animation* animation = Animation::createWithSpriteFrames(animFrames, 0.5f / 4);
	Animate* animate = Animate::create(animation);
	Repeat* rep = Repeat::create(animate, 1);

	Sprite* SpData = (Sprite*)_gunObstacle->BaseBody->GetUserData();
	if (_gunObstacle->BaseBody->GetFixtureList()->GetFilterData().categoryBits == LGUNTRAP_CATEGORY) {
		SpData->setFlippedX(true);
	}
	SpData->runAction(rep);

	return;
}

void Obstacle::GunTrapFireAnimation(GunObstacle* _gunObstacle, const std::string& _sPlist_Path, const std::string& _sSheet_Path, const char* _cFilename_Path, LightEffect* _lighteffect) {
	int nSheetmx = 6;
	std::string sPath;
	std::string sPath_f;
	sPath = "n_Images/obstacle/GunTrap_Fire_.plist";
	sPath_f = "n_Images/obstacle/GunTrap_Fire_n.png";
	SpriteFrameCache* spritecache = SpriteFrameCache::getInstance();
	spritecache->addSpriteFramesWithFile(_sPlist_Path);

	Vector<SpriteFrame*> animFrames;
	char str[100];
	for (int i = 0; i < nSheetmx; i++) {
		sprintf(str, _cFilename_Path, i);
		animFrames.pushBack(spritecache->getSpriteFrameByName(str));
	}

	Animation* animation = Animation::createWithSpriteFrames(animFrames, 0.5f / 4);
	Animate* animate = Animate::create(animation);
	Repeat* rep = Repeat::create(animate, 1);

	EffectSprite* Efspdata = (EffectSprite*)_gunObstacle->BaseBody->GetUserData();
	m_layer->removeChild(Efspdata, true);

	EffectSprite* EfnewSpData = EffectSprite::createWithSpriteFrame(animFrames.front());
	if (_gunObstacle->BaseBody->GetFixtureList()->GetFilterData().categoryBits == LGUNTRAP_CATEGORY) {
		EfnewSpData->setAnchorPoint(Vec2(0.5, 0));
	}
	else {
		EfnewSpData->setAnchorPoint(Vec2(0, 0));
	}
	EfnewSpData->setColor(_lighteffect->getAmbientLightColor());
	EfnewSpData->setEffect(_lighteffect, _sSheet_Path);
	m_layer->addChild(EfnewSpData);
	_gunObstacle->BaseBody->SetUserData(EfnewSpData);
	if (_gunObstacle->BaseBody->GetFixtureList()->GetFilterData().categoryBits == LGUNTRAP_CATEGORY) {
		EfnewSpData->setFlippedX(true);
	}
	EfnewSpData->runAction(rep);

	return;
}

bool Obstacle::GunBulletCollision(Sprite* charactor_sprite, int* _contactBullet) {
	std::list<b2Body*>::iterator iter;

	Rect Charactor_area = charactor_sprite->getBoundingBox();

	for (iter = lb_TrapBullet.begin(); iter != lb_TrapBullet.end(); ++iter) {
		b2Body* bullet_Body = *iter;
		Sprite* bullet_Sprite = static_cast<Sprite*>(bullet_Body->GetUserData());
		Rect bullet_area = bullet_Sprite->getBoundingBox();
		
		if (Charactor_area.intersectsRect(bullet_area)) {
			log("bullet strikes a Charactor");
			return true;
		}
		else if (*_contactBullet > 0) {
			*_contactBullet = 0;
			Sprite* bulletSprite = static_cast<Sprite*>(bullet_Body->GetUserData());

			std::string sPath = "Images/effects/guntrap_crash_effect.png";
			
			if (bullet_Body->GetLinearVelocity().x < 0) {
				m_EffectAnimation.CreateEffect(sPath, 5, Size(20, 15), bulletSprite->getPosition(), 0.5f, m_layer);
			}
			else
				m_EffectAnimation.CreateEffect(sPath, 5, Size(20, 14), bulletSprite->getPosition(), 0.5f, m_layer);
			
			m_layer->removeChild(bulletSprite, true);
			m_world->DestroyBody(bullet_Body);

			lb_TrapBullet.erase(iter);	
			break;
		}
		else {
			return false;
		}
	}
}

Sprite* Obstacle::CreateLava(){
	Texture2D* texture = Director::getInstance()->getTextureCache()->addImage("Images/obstacle/Lava.png");

	Animation* animation = Animation::create();
	animation->setDelayPerUnit(0.5f);
	
	Animation* animationb = Animation::create();
	animationb->setDelayPerUnit(0.5f);

	for (int i = 0; i < 10; i++) {
		animation->addSpriteFrameWithTexture(texture, Rect(i * 960, 0, 960, 1024));
		animationb->addSpriteFrameWithTexture(texture, Rect(i * 960, 0, 960, 1024));
	}

	m_Lava_Sprite = Sprite::createWithTexture(texture, Rect(0, 0, 960, 1024));
	m_Lava_Sprite->setAnchorPoint(Vec2(0, 1));
	m_Lava_Sprite->setPosition(Vec2(0, -10));
	m_layer->addChild(m_Lava_Sprite, 4);

	m_Lava_Spriteb = Sprite::createWithTexture(texture, Rect(0, 0, 960, 1024));
	m_Lava_Spriteb->setAnchorPoint(Vec2(0, 1));
	m_Lava_Spriteb->setPosition(Vec2(960, -10));
	m_layer->addChild(m_Lava_Spriteb, 4);

	Animate* animate = Animate::create(animation);
	RepeatForever* rep = RepeatForever::create(animate);
	m_Lava_Sprite->runAction(rep);

	Animate* animateb = Animate::create(animationb);
	RepeatForever* repb = RepeatForever::create(animateb);
	m_Lava_Spriteb->runAction(repb);

	return m_Lava_Sprite;
}

void Obstacle::LavaEruption(float dt) {
	if ((m_fLava_Eruptiontime -= dt) <= 0) {
		m_Lava_Sprite->setPosition(m_oldLava_pos + Vec2(0, 5));
		m_Lava_Spriteb->setPosition(m_oldLava_pos + Vec2(960, 5));
		m_oldLava_pos = m_Lava_Sprite->getPosition();

		m_fLava_Eruptiontime = 1.0f;
	}
}

void Obstacle::CreateTraps() {

}

void Obstacle::CreatefallingBlade(b2Body* b) {
	b2Body* Obstacle_Body;
	Sprite* BladeSprite;
	Sprite* SpriteUserdata;
	b2BodyDef bodydef;
	b2FixtureDef fixture;
	b2Fixture* SensorFixture;
	b2PrismaticJointDef priJointDef;
	b2PrismaticJoint* priJoint;
	Sprite* LopeSprite;
	BladeObstacle* bladeObstacle = new BladeObstacle;

	b2Vec2 vertices[4];
	int vindex = 4;

	b2PolygonShape* rectshape = new b2PolygonShape;

	BladeSprite = Sprite::create("Images/obstacle/FallingBlade.png");
	BladeSprite->setScale(0.7f);
	m_layer->addChild(BladeSprite, 3);

	bodydef.userData = BladeSprite;
	bodydef.position = b->GetPosition() - b2Vec2(-0.5, 2);
	bodydef.type = b2_dynamicBody;

	Obstacle_Body = m_world->CreateBody(&bodydef);
	Obstacle_Body->SetHp(1000);
	Obstacle_Body->SetIsCh(false);

	rectshape->SetAsBox(1, 0.7);
	fixture.shape = rectshape;
	fixture.isSensor = false;
	fixture.filter.categoryBits = FALLBLADE_CATEGORY;
	fixture.filter.maskBits = 0x000E;
	SensorFixture = Obstacle_Body->CreateFixture(&fixture);
	SensorFixture->SetUserData((void*)FALLINGBLADE_CONTACT);

	priJointDef.Initialize(b, Obstacle_Body, b->GetPosition() + b2Vec2(0.5, 0.5), b2Vec2(0.0f, -0.1f));
	priJointDef.lowerTranslation = -0.9;
	priJointDef.upperTranslation = 6;
	priJointDef.enableLimit = true;

	priJointDef.enableMotor = true;
	priJointDef.motorSpeed = 10;
	priJointDef.maxMotorForce = 100;

	Obstacle_Body->SetGravityScale(3.0f);

	priJoint = (b2PrismaticJoint*)m_world->CreateJoint(&priJointDef);

	SpriteUserdata = (Sprite*)b->GetUserData();
	LopeSprite = Sprite::create("Images/obstacle/Lope.png");
	LopeSprite->setAnchorPoint(Vec2(0.0f, 0.0f));
	LopeSprite->setPosition(Vec2(11, 0));
	SpriteUserdata->addChild(LopeSprite, 5);

	bladeObstacle->BaseBody = b;
	bladeObstacle->BladeBody = Obstacle_Body;
	bladeObstacle->RopeSprite = LopeSprite;

	lb_BladeOb.push_back(bladeObstacle);
}

void Obstacle::CreatefallingBlade(b2Body* b, bool _blind) {
	b2Body* Obstacle_Body;
	EffectSprite* BladeSprite;
	EffectSprite* SpriteUserdata;
	b2BodyDef bodydef;
	b2FixtureDef fixture;
	b2Fixture* SensorFixture;
	b2PrismaticJointDef priJointDef;
	b2PrismaticJoint* priJoint;
	EffectSprite* LopeSprite;
	BladeObstacle* bladeObstacle = new BladeObstacle;

	b2Vec2 vertices[4];
	int vindex = 4;

	b2PolygonShape* rectshape = new b2PolygonShape;

	BladeSprite = EffectSprite::create("Images/obstacle/FallingBlade.png");
	BladeSprite->setEffect(m_lighteffect, "n_Images/obstacle/FallingBlade_n.png");
	BladeSprite->setColor(m_lighteffect->getAmbientLightColor());
	BladeSprite->setScale(0.7f);
	m_layer->addChild(BladeSprite, 3);

	bodydef.userData = BladeSprite;
	bodydef.position = b->GetPosition() - b2Vec2(-0.5, 2);
	bodydef.type = b2_dynamicBody;

	Obstacle_Body = m_world->CreateBody(&bodydef);
	Obstacle_Body->SetHp(1000);
	Obstacle_Body->SetIsCh(false);

	rectshape->SetAsBox(1, 0.7);
	fixture.shape = rectshape;
	fixture.isSensor = false;
	fixture.filter.categoryBits = FALLBLADE_CATEGORY;
	fixture.filter.maskBits = 0x000E;
	SensorFixture = Obstacle_Body->CreateFixture(&fixture);
	SensorFixture->SetUserData((void*)FALLINGBLADE_CONTACT);

	priJointDef.Initialize(b, Obstacle_Body, b->GetPosition() + b2Vec2(0.5, 0.5), b2Vec2(0.0f, -0.1f));
	priJointDef.lowerTranslation = -0.9;
	priJointDef.upperTranslation = 6;
	priJointDef.enableLimit = true;

	priJointDef.enableMotor = true;
	priJointDef.motorSpeed = 10;
	priJointDef.maxMotorForce = 100;

	Obstacle_Body->SetGravityScale(3.0f);

	priJoint = (b2PrismaticJoint*)m_world->CreateJoint(&priJointDef);

	SpriteUserdata = (EffectSprite*)b->GetUserData();
	LopeSprite = EffectSprite::create("Images/obstacle/Lope.png");
	LopeSprite->setEffect(m_lighteffect, "n_Images/obstacle/Lope_n.png");
	LopeSprite->setColor(m_lighteffect->getAmbientLightColor());
	LopeSprite->setAnchorPoint(Vec2(0.0f, 0.0f));
	LopeSprite->setPosition(Vec2(11, 0));
	SpriteUserdata->addChild(LopeSprite, 5);

	bladeObstacle->BaseBody = b;
	bladeObstacle->BladeBody = Obstacle_Body;
	bladeObstacle->RopeSprite = LopeSprite;

	lb_BladeOb.push_back(bladeObstacle);
}