#include "HelloWorldScene.h"
#include "JoyStick.h"
#include "Charactor_Hand.h"
#include "CharactorCreator.h"
#include "MonsterCreator.h"
#include "Opossum.h"
#include "Frog.h"
#include "Bat.h"
#include "BossMonster.h"
#include "B2DebugDrawLayer.h"
#include "Explosion.h"
#include "EffectAnimation.h"
#include "EffectSprite.h"
#include "LightEffect.h"
#include "Obstacle.h"
#include "GameUI.h"
#include "ItemShop.h"
#include "soundManager.h"
#include "Title.h"
#include "Option.h"
#include "ActionShake.h"
#include "LoadingScene.h"
#include "BuffManager.h"


USING_NS_CC;

const std::string BGM_Background_1_PATH = "Sounds/Background_Sound.mp3";
const std::string BGM_Charactor_Step_PATH = "Sounds/Charactor_step.mp3";
const std::string SEF_Charactor_Jump_PATH = "Sounds/Charactor_Jump.mp3";
const std::string SEF_Charactor_Pain_PATH = "Sounds/Charactor_Pain.mp3";
const std::string SEF_Potion_Drink_PATH = "Sounds/PotionDrink.mp3";
const std::string SEF_Item_Pickup_PATH = "Sounds/Pickup_Item.mp3";
const std::string SEF_Gun_Pickup_PATH = "Sounds/Pickup_Gun.mp3";
const std::string SEF_Gold_Pickup_PATH = "Sounds/Pickup_Gold.mp3";

const std::string SEF_Item_Swing_PATH = "Sounds/Swing_Item.mp3";
const std::string SEF_Block_Attack_PATH = "Sounds/Attack_Block.mp3";
const std::string SEF_Gun_Attack_PATH = "Sounds/Attack_Gun.mp3";
const std::string SEF_Sword_Slice_PATH = "Sounds/Slice_Sword.mp3";

const std::string SEF_Item_Throw_PATH = "Sounds/Throw_Item.mp3";
const std::string SEF_BOMB_EXPROTION_PATH = "Sounds/Explosion_Bomb.mp3";

const std::string SEF_MONSTER_HIT_PATH = "Sounds/Monster_Hit.mp3";
const std::string SEF_MONSTER_HIT_BY_FEET_PATH = "Sounds/Monster_attack_by_feet.mp3";
const std::string BGM_CAUTION_SIREN_PATH = "Sounds/Caution_Siren.mp3";

const std::string SEF_BUFFEFFECT_PATH = "Sounds/Buff.mp3";
const std::string SEF_DEBUFFEFFECT_PATH = "Sounds/Debuff.mp3";
const std::string SEF_HIT_FAILED_PATH = "Sounds/Hit_Failed_EF.mp3";

#define PTMRATIO 32
#define DIRECTPOW 30
#define JUMPPOW 10
#define ATTACKDELAY 5
#define PICKAXEDAMAGE 9;
#define BOMBDAMAGE 5
#define SWORDDAMAGE 5
#define GUNDAMAGE 2
#define DEGTORAD 3.14 / 180
#define RADTODEG 180 / 3.14

CMyContactListener gContactInstance;
float currentRayAngle = 0;
Charactor_Data g_CharactorData;

Scene* HelloWorld::createScene(Charactor_Data _init_Charactor)
{
	g_CharactorData = _init_Charactor;
    return HelloWorld::create();
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }
	winSize = Director::getInstance()->getWinSize();

	wlayer = DLayer::create();
	wlayer->setContentSize(Size(1920, 1024));
	this->addChild(wlayer, 2);

	m_clayer = LayerColor::create(Color4B(255, 255, 255, 0));
	wlayer->addChild(m_clayer, 5);

	m_joystick = JoyStick::create();
	m_joystick->setLimitScreen(true);
	m_joystick->setTouchShow(true);
	m_clayer->addChild(m_joystick);

	m_fDelayTime = 0;
	m_ftime = 0;

	m_bChangeStage = false;

	shader = new GLProgram();
	shader->initWithFilenames("shader/white.vsh", "shader/white.fsh");

	shader->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_COLOR, GLProgram::VERTEX_ATTRIB_COLOR);
	shader->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_POSITION, GLProgram::VERTEX_ATTRIB_POSITION);
	shader->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_TEX_COORD, GLProgram::VERTEX_ATTRIB_TEX_COORD);

	shader->link();
	shader->updateUniforms();

	m_Contact_clear.push_back(&gNumFootContacts);
	m_Contact_clear.push_back(&gNumRightContacts);
	m_Contact_clear.push_back(&gNumObstacleContacts);
	m_Contact_clear.push_back(&gNumItemContacts);
	m_Contact_clear.push_back(&gNumpickaxeContacts);
	m_Contact_clear.push_back(&gNumMonsterContacts);
	m_Contact_clear.push_back(&gNumMonsterFootContacts);
	m_Contact_clear.push_back(&gNumFrogFootContacts);
	m_Contact_clear.push_back(&gNumFootAttackContacts);
	m_Contact_clear.push_back(&gNumBombContacts);
	m_Contact_clear.push_back(&gNumSwordContacts);
	m_Contact_clear.push_back(&gNumGunContacts);
	m_Contact_clear.push_back(&gNumLadderContacts);
	m_Contact_clear.push_back(&gNumLongLadderContacts);
	m_Contact_clear.push_back(&gNumLadderUpDown);
	m_Contact_clear.push_back(&gNumGoldContacts);
	m_Contact_clear.push_back(&gNumShopManContacts);
	m_Contact_clear.push_back(&gNumChipToCharactor);
	m_Contact_clear.push_back(&gNumChipToItem);
	m_Contact_clear.push_back(&gNumChipToPickaxe);
	m_Contact_clear.push_back(&gNumChipToMonster);
	m_Contact_clear.push_back(&gNumChipToBomb);
	m_Contact_clear.push_back(&gNumFallingBladeContacts);
	m_Contact_clear.push_back(&gNumTrapBulletContacts);


	srand(static_cast<unsigned int>(time(NULL)));
	
	if (this->createBox2dWorld(true)) {
		this->schedule(schedule_selector(HelloWorld::tick));
	}
	
    return true;
}

HelloWorld::~HelloWorld() {
	for (int i = 0; i < m_Contact_clear.size(); i++) {
		*m_Contact_clear.at(i) = 0;
	}
	g_isLeft = false;
	g_isRight = false;
	g_isUp = false;
	g_isDown = false;
	g_isLadderJump = false;
	g_isLadderStop = false;
	g_isJump = false;
	g_isThrow = false;


	delete _world;
	_world = nullptr;

	delete m_debugDraw;
	m_debugDraw = nullptr;
	delete m_ItemShop;
	m_ItemShop = nullptr;
	delete m_Explosion;
	m_Explosion = nullptr;
	delete m_GameUi;
	m_GameUi = nullptr;
	delete m_HeartLabel;
	m_HeartLabel = nullptr;
	delete Player;
	Player = nullptr;
	delete GunEffect;
	GunEffect = nullptr;
	delete m_Monster_Component;
	m_Monster_Component = nullptr;
//	delete m_Monster;
	m_Monster = nullptr;

	delete m_Obstacle;
	m_Obstacle = nullptr;

	delete m_BuffManager;
	m_BuffManager = nullptr;
}

void HelloWorld::onEnter() {
	Scene::onEnter();

	// 싱글터치모드로 터치리스너 등록
	auto listener = EventListenerTouchAllAtOnce::create();
	
	listener->onTouchesBegan = CC_CALLBACK_2(HelloWorld::onTouchesBegan, this);
	listener->onTouchesMoved = CC_CALLBACK_2(HelloWorld::onTouchesMoved, this);
	listener->onTouchesEnded = CC_CALLBACK_2(HelloWorld::onTouchesEnded, this);

	m_kListener = EventListenerKeyboard::create();
	m_kListener->onKeyPressed = CC_CALLBACK_2(HelloWorld::onKeyPressed, this);
	m_kListener->onKeyReleased = CC_CALLBACK_2(HelloWorld::onKeyReleased, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(m_kListener, this);

	//Director::sharedDirector()->getEventDispatcher()->removeEventListenersForTarget(m_CameraSprite);
}

void HelloWorld::onExit() {
	Scene::onExit();
}
/*
void DLayer::onEnter() {
	Layer::onEnter();

	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(DLayer::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(DLayer::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(DLayer::onTouchEnded, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}
void DLayer::onExit() {
	Layer::onExit();
}

bool DLayer::onTouchBegan(Touch* touch, Event* event) {

	Vec2 touchPoint = touch->getLocation();
	Vec2 touchPoint2 = Node::convertToNodeSpace(touchPoint);
	log("nodeSpace.. %f, %f", touchPoint2.x, touchPoint2.y);
	this->schedule(schedule_selector(HelloWorld::TouchHold), 0.5f);

	return true;
}
void DLayer::onTouchMoved(Touch* touch, Event* event) {

}
void DLayer::onTouchEnded(Touch* touch, Event* event) {
	
}*/


int n = 0;
int nb = 0;
int nc = 0;
void HelloWorld::tick(float dt) {
	if (m_bChangeStage) {
		m_bChangeStage = false;
		return;
	}
	int velocityIterations = 8;
	int positionIterations = 3;

	this->TouchUpdate(dt);
	if (g_CharactorData.StageNumber != SHELTER) {
		//log("this not shelter this STAGE %d", g_CharactorData.StageNumber);
	}
	else {
		//log("this shelter");
		log("Charactor pos %f, %f", Player->getCharactorBody()->GetPosition().x, Player->getCharactorBody()->GetPosition().y);
	}
	// 맵, 캐릭터, 아이템 든 캐릭터, 몬스터 Light 해제하고 일정 맵에서만 Light 적용하도록 할것. 

	//Player_posasdas = Vec2(Player->getCharactorBody()->GetPosition().x * 32, Player->getCharactorBody()->GetPosition().y * 32);

	if ((m_GameUi->m_StageMessageTime -= dt) < 0 && m_GameUi->getStageMessageTurnOff()) {
		m_GameUi->setStageMessageTurnOff(false);
		m_GameUi->MessageDelete(m_GameUi->getStageMessage());
		m_GameUi->m_StageMessageTime = 0;
	}

	if (Player->getCharactorBody()->GetPosition().y > 29) { // 캐릭터가 특정 Y좌표로 이동했을때 Stage Clear 이기 때문에 다음 Stage로 넘어갈 수 있도록 기존 Stage를 지워주어 다음 Stage를 만들 준비를 한다.
		this->clearFloor();
		m_bChangeStage = true;
		if (m_bChangeStage) {
			m_bChangeStage = false;
			return;
		}
	}

	if (m_ItemShop->getThrowItemQSize() > 0) {
		m_ItemShop->ThrowItem(dt);
		g_CharactorData.n_Gold_Count = m_ItemShop->small_change();
		m_GameUi->GoldManager(m_clayer, g_CharactorData.n_Gold_Count);
	}
	if (m_GameUi->getMessage()) {
		m_GameUi->setMessage(m_GameUi->getMessage());
		m_fTemporary_messageTime = 2;
	}

	if (m_GameUi->TemporaryMessageManager(&m_fTemporary_messageTime, dt)) {
		m_GameUi->MessageDelete(m_GameUi->getItemMessage());
	}

	if ((m_fLavatime -= dt) <= 0) {
		m_fLavatime = 0;
		m_Obstacle->LavaEruption(dt);

		if (m_GameUi->getCautionOnOff()) {
			CSoundManager::getInstance()->playBgm(BGM_CAUTION_SIREN_PATH, true, false);
			m_GameUi->CreateCautionMessage();
			m_GameUi->setCautionOnOff(false);
		}
		if ((m_fcautiontime -= dt) <= 0 && m_GameUi->getCautionTurnoff()) {
			CSoundManager::getInstance()->stopBgm(BGM_CAUTION_SIREN_PATH);
			m_GameUi->setCautionTurnoff(false);
			m_clayer->removeChild(m_GameUi->getCautionSprite(), true);
		}
	}
	int nbefore_hp = g_CharactorData.n_Hp_Count;
	if (g_CharactorData.StageNumber != SHELTER) {
		m_Obstacle->FallingBladeControl(gNumFallingBladeContacts, dt, &m_fCharactorPainDelay, Player->getCharactorBody());
		m_Obstacle->GunTrapControl(gNumTrapBulletContacts, dt, &m_fCharactorPainDelay, &g_CharactorData, Player->getCharactorBody(), &gNumTrapBulletContacts);
		if (Map_Body.getLavalist().size() > 0) {
			if (Map_Body.LavaCollision(Player->getCameraBody(), dt, &m_fCharactorPainDelay, wlayer)) {
				g_CharactorData.n_Hp_Count = g_CharactorData.n_Hp_Count - 2;
			}
		}
	}
	//log("gNum %d", gNumTrapBulletContacts);
	if (nbefore_hp != g_CharactorData.n_Hp_Count) {
		this->CharactorPainAction();
	}

	// 맵이 SHELTER이거나 Blind 맵일 경우에 조명이 맵을 벗어나지 않게하고 캐릭터를 따라 움직일 수 있도록.
	if (g_CharactorData.StageNumber == SHELTER || g_CharactorData.bStage_blind) {
		Vec2 LightPos;
		float x = m_oldpos.x - Player->getCharactorBody()->GetPosition().x;
		m_oldpos.x = Player->getCharactorBody()->GetPosition().x;
		//log("%f", x);

		LightPos.x = Player->getCharactorBody()->GetPosition().x * PTMRATIO + x;
		//	log("LightPos.y = %f", LightPos.y);
		if (LightPos.x > 240 && LightPos.x < 1680) LightPos.x = 240;
		if (LightPos.x > 240 && LightPos.x > 1680) LightPos.x = LightPos.x - 1440;

		float y = m_oldpos.y - Player->getCharactorBody()->GetPosition().y;
		m_oldpos.y = Player->getCharactorBody()->GetPosition().y;
		LightPos.y = Player->getCharactorBody()->GetPosition().y * PTMRATIO + y;
		if (LightPos.y > 160) LightPos.y = 160;

		//log("ply %f - old %f = %f", Player->getCharactorBody()->GetPosition().x * 32, m_oldpos.y * 32, x);

		Map_Body.getLightEffect()->setLightPos(Vec3(LightPos.x - 10, LightPos.y - 10, 10));
		Map_Body.getLightEffect();
	}

	b2Vec2 vel = Player->getCharactorBody()->GetLinearVelocity();
	float desiredVel = 0;

	if (g_isLeft == true) desiredVel = -5 - g_CharactorData.status.f_Speed;
	if (g_isStop == true) desiredVel = 0;
	if (g_isUp == true) desiredVel = 5 + g_CharactorData.status.f_JumpPower;
	if (g_isDown == true) desiredVel = -5 - g_CharactorData.status.f_JumpPower;
	if (g_isRight == true) desiredVel = 5 + g_CharactorData.status.f_Speed;
	if (g_isLeft && g_isRight) desiredVel = 0;
	if (g_isLeft && g_isStop) desiredVel = 0;
	if (g_isRight && g_isStop) desiredVel = 0;
	//if (g_isThrow && g_isLeft || g_isRight) desiredVel = 0;

	m_fLadderCooltime -= dt;
	if (m_fLadderCooltime < -10) m_fLadderCooltime = 0;

	// 공격 쿨타임 1.5초
	if ((m_fAttackCooltime -= dt) <= 0) {
		//m_fAttackCooltime = 1;
		g_isAttackOn = true;
		g_ATTACK = false;
	}

	if ((m_fBombCooltime -= dt) <= 0) {
		g_isThrowOn = true;
	}

	// 공격 animation 1회 출력후 멈춘뒤 다음 Animation 출력
	if (!g_isAttackOn && (m_fAttackDelaytime -= dt) <= 0) {
		//	m_fAttackDelaytime = 0.5;
		if (m_bTakePickaxe)
			HelloWorld::KeyAction(MS_STOP, Hand_On, "Pickaxe");
		else if (m_bTakeSword)
			HelloWorld::KeyAction(MS_STOP, Hand_On, "Sword");
		else if (m_bTakeGun)
			HelloWorld::KeyAction(MS_STOP, Hand_On, "Gun");
		else
			HelloWorld::KeyAction(MS_STOP, Hand_On, "");
	}
	else if (m_fAttackDelaytime <= 0) {
		m_bMonster_swordshot = false;
	}
	else if (m_bTakeSword && m_fAttackDelaytime > 0) {

	}


	if (g_ATTACK) {
		HelloWorld::AttackMonster(dt);
	}

	power_enegy(dt);
	m_fcleantime += dt;

	// 총알이 월드에 추가됬을 때
	if (Bullets.size() > 0) {
		std::list<Bullet>::iterator biter;
		//std::list<Monster>::iterator miter;
		for (biter = Bullets.begin(); biter != Bullets.end();) {
			b2Vec2 pos = biter->body->GetPosition();
			MyQueryCallback bulletCallback;
			biter->aabb.lowerBound = pos - b2Vec2(0.1, 0.1);
			biter->aabb.upperBound = pos + b2Vec2(0.1, 0.1);
			m_aabbb = biter->aabb;
			_world->QueryAABB(&bulletCallback, biter->aabb);

			std::sort(bulletCallback.foundBodies.begin(), bulletCallback.foundBodies.end());
			bulletCallback.foundBodies.erase(std::unique(bulletCallback.foundBodies.begin(), bulletCallback.foundBodies.end()), bulletCallback.foundBodies.end());
			// 총알의 AABB경계에 다른 object가 접촉되었을 시 1개 이상인 이유는 총알의 body가 항상 aabb영역 안에 포함되기 때문
			if (bulletCallback.foundBodies.size() > 1) {

				for (int i = 0; i < bulletCallback.foundBodies.size(); i++) {
					b2Body* bulletBody = bulletCallback.foundBodies[i];
					b2BodyType bodytype = bulletBody->GetType();
					if (bodytype != b2_staticBody && bodytype != b2_dynamicBody && bodytype != b2_kinematicBody) {
						log("실체가없는 body");
						continue;
					}
					INT16 category = bulletBody->GetFixtureList()->GetFilterData().categoryBits;

					if (category == 0x0002) {
						//m_DestroyQ.push(bulletBody);
						m_bBulletcrash = true;
						log("block");
						//	break;
					}
					else if (category == 0x0010 && nb % 2 == 0) {
						m_bBulletcrash = true;
						int num = bulletBody->GetHp();
						num -= GUNDAMAGE;
						bulletBody->SetHp(num);
						log("monster hp = %d", num);
					}
				}
				nb++;
				if (nb > 10)
					nb = 0;
			}

			if (m_bBulletcrash) {
				m_bBulletcrash = false;
				Sprite* bulletsp = static_cast<Sprite*>(biter->body->GetUserData());
				std::string sPath = "Images/effects/gun_crash_effect.png";
				bulletsp = GunEffect->CreateEffect(sPath, 5, Size(20, 14), bulletsp->getPosition(), 0.5f, wlayer);
				//EffectAnimation::getInstance()->CreateEffect(sPath, 5, Size(20, 14), bulletsp->getPosition(), 0.5, wlayer);
				if (!g_bViewing) bulletsp->setFlippedX(true);
				m_DestroyQ.push(biter->body);
				biter = Bullets.erase(biter);
			}
			else
				++biter;
		}
	}

	if (GunEffect->getGunEffects()->size() > 0) {
		std::list<Effecter>::iterator iter;
		for (iter = GunEffect->getGunEffects()->begin(); iter != GunEffect->getGunEffects()->end();) {
			if ((iter->animation_time -= dt) <= 0) {
				wlayer->removeChild(iter->EffectSprite);
				iter = GunEffect->getGunEffects()->erase(iter);
			}
			else ++iter;
		}
	}

	if (GunEffect->getSwordEffects()->size() > 0) {
		std::list<Effecter>::iterator iter;
		for (iter = GunEffect->getSwordEffects()->begin(); iter != GunEffect->getSwordEffects()->end();) {
			if ((iter->animation_time -= dt) <= 0) {
				wlayer->removeChild(iter->EffectSprite);
				iter = GunEffect->getSwordEffects()->erase(iter);
			}
			else ++iter;
		}
	}

	if (GunEffect->getBombEffects()->size() > 0) {
		std::list<Effecter>::iterator iter;
		for (iter = GunEffect->getBombEffects()->begin(); iter != GunEffect->getBombEffects()->end();) {
			if ((iter->animation_time -= dt) <= 0) {
				wlayer->removeChild(iter->EffectSprite);
				iter = GunEffect->getBombEffects()->erase(iter);
			}
			else ++iter;
		}
	}

	// 폭탄이 월드에 추가됬을 때
	if (m_Explosion->getBombs_size() > 0) {
		Bombs = m_Explosion->getBombs();
		std::list<Bomb>::iterator iter;
		b2Vec2 pos;
		for (iter = Bombs->begin(); iter != Bombs->end(); ++iter) {
			pos = iter->body->GetPosition();
			iter->aabb.lowerBound = pos - b2Vec2(1, 1);
			iter->aabb.upperBound = pos + b2Vec2(2, 2);
			m_aabb = iter->aabb;
			_world->QueryAABB(&queryCallback, iter->aabb);
		}

		// 폭발
		// 바로 터지지 않고 body의 설정시간 후 터진다.
		if (m_Explosion->burst(_world, dt, wlayer)) {
			CSoundManager::getInstance()->playEffect(SEF_BOMB_EXPROTION_PATH);

			float interval = 1 / 60;
			float duration = 0.8f;
			float speed = 6.0f;
			float magnitude = 4.f;

			this->runAction(ActionShake::create(duration, speed, magnitude));

			std::string sPath = "Images/effects/explosion.png";
			Vec2 Bombpos = Vec2(pos.x * 32, pos.y * 32);
			GunEffect->CreateEffect(sPath, 4, Size(40, 41), Bombpos, 0.5f, wlayer);

			// 폭탄이 터지면서 사라질때 한 프레임에서의 쿼리영역만 처리하기 위해 중복body삭제
			std::sort(queryCallback.foundBodies.begin(), queryCallback.foundBodies.end());
			queryCallback.foundBodies.erase(std::unique(queryCallback.foundBodies.begin(), queryCallback.foundBodies.end()), queryCallback.foundBodies.end());
			for (int i = 0; i < queryCallback.foundBodies.size(); i++) {
				b2Vec2 pos = queryCallback.foundBodies[i]->GetPosition();
				b2Body* body = queryCallback.foundBodies[i];
				//b2Fixture* fix = body->GetFixtureList();
				//UINT16 categori = fix->GetFilterData().categoryBits;



				int obj_hp = body->GetHp();
				if (obj_hp > 0) {
					obj_hp = obj_hp - BOMBDAMAGE * 6;
					body->SetHp(obj_hp);
				}

				//				log("pos.x %f, pos.y %f %d", pos.x, pos.y, i);
			}
		}

	}

	if (m_fcleantime > 0.8) {
		m_fcleantime = 0;
		if (m_Explosion->getChips_size() != 0) {
			m_Explosion->cleanChips(_world);
		}
	}

	/*몬스터 및 장애물 충돌 시 Charactor Move State 가 MS_PAIN 상태로 바뀌며
	m_fCharactorPaintime(캐릭터 점멸 시간)만큼 Charactor Sprite visible 을
	조절하여 점멸되도록 해 충돌 이펙트를 연출해준다.*/
	if (Player->getMoveState() == MS_PAIN && m_fCharactorPaintime > 0) {
		Player->getCharactorBody()->SetGravityScale(1.0f);
		m_before_state = MS_PAIN;
		Player->setMoveState(MS_PAIN);
		float swap_value = (m_fCharactorPaintime -= dt);
		EffectSprite* CharactorSp = static_cast<EffectSprite*>(Player->getCharactorBody()->GetUserData());

		if (swap_value > 0.1) {
			//CharactorSp->setGLProgram(shader);
			CharactorSp->setVisible(false);
		}
		else if (swap_value < 0.1) {
			//CharactorSp->setGLProgram(GLProgramCache::getInstance()->getGLProgram(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR_NO_MVP));
			CharactorSp->setVisible(true);
			if (swap_value < 0)
				m_fCharactorPaintime = 0.2f;
		}
	}

	if (Player->getMoveState() == MS_PAIN && (m_fCharactorPainDelay -= dt) <= 0) {
		EffectSprite* CharactorSp = static_cast<EffectSprite*>(Player->getCharactorBody()->GetUserData());

		CharactorSp->setVisible(true);
		Player->setMoveState(MS_STOP);
	}
	if (m_bChangeStage) {
		return;
	}
	if (gNumFootContacts < 1 && !g_isUp && !g_isDown && !g_isLadderRight && !g_isLadderLeft && m_fAttackDelaytime <= 0 && Player->getMoveState() != MS_PAIN && m_before_state != MS_RIGHTUP && m_before_state != MS_LEFTUP) {
		//	Player->setMoveState(MS_STOP);
			//log("Jump %f", gNumFootContacts);
			// 캐릭터의 현재 y축을 이전 프레임의 y축과 비교하여 Sprite를 변경해준다.

		if (!m_bCan_UpDown) {
			float distance = Player->getCharactorBody()->GetPosition().y - m_Player_before_position.y;
			if (m_Player_before_position.y < Player->getCharactorBody()->GetPosition().y) {
				m_Player_before_position = Player->getCharactorBody()->GetPosition();
				if (m_bTakePickaxe)
					HelloWorld::KeyAction(MS_JUMP, JumpHand_On, "Pickaxe");
				else if (m_bTakeSword)
					HelloWorld::KeyAction(MS_JUMP, JumpHand_On, "Sword");
				else if (m_bTakeGun)
					HelloWorld::KeyAction(MS_JUMP, JumpHand_On, "Gun");
				else
					HelloWorld::KeyAction(MS_JUMP, JumpHand_On, "");
			}
			else if (m_Player_before_position.y > Player->getCharactorBody()->GetPosition().y) {
				m_Player_before_position = Player->getCharactorBody()->GetPosition();
				if (m_bTakePickaxe)
					HelloWorld::KeyAction(MS_JUMPDOWN, JumpHand_Down, "Pickaxe");
				else if (m_bTakeSword)
					HelloWorld::KeyAction(MS_JUMPDOWN, JumpHand_Down, "Sword");
				else if (m_bTakeGun)
					HelloWorld::KeyAction(MS_JUMPDOWN, JumpHand_Down, "Gun");
				else
					HelloWorld::KeyAction(MS_JUMPDOWN, JumpHand_Down, "");
			}
			m_bOnLadder = false;
			g_isLadderJump = false;
		}
		else if (m_bCan_UpDown) {
			if (g_isUp || g_isDown || m_before_state == MS_UPDOWN) {
				/* 일부러 MS_UPDOWN과 MS_ONLADDER를 나누어 둠.
				 이유는 Joystick은 매 Frame(0.016)마다 입력되어 Joystick check를 할때마다 Animation을 바뀌어야하고 따라서,
				 Charactor before state 와 현재 Charactor current state를 비교해서 다를때만 Sprite Animation을 바꾸어 body에 적용시키도록 했다.
				 따라서, 사다리에서 오르고 내려올때 Animation이 중복되서 body에 적용되면 바뀔 Sprite가 없는데도 기존 Sprite의 지우고 반복을 프레임마다 반복하기 때문에
				 body는 움직이지만 Sprite는 마치 멈춰있는 듯한 버그가 생기므로
				 사다리에 관련된 캐릭터의 상태는 MS_UPDOWN이 관리하고 그것과 비교하기 위한 용도로 MS_ONLADDER를 두어 비교용도로 사용한다.*/
				Player->setMoveState(MS_ONLADDER);
				m_bOnLadder = true;
				m_bCan_Move = false;
			}
		}

		m_bDo_Jump = true;
	}

	else if (gNumFootContacts < 1 && !g_isUp && !g_isDown && m_fAttackDelaytime <= 0 && Player->getMoveState() != MS_PAIN) {
		if (g_isLadderRight && g_isJump && m_before_state == MS_UPDOWN || m_before_state == MS_ONLADDER) {
			g_isLadderRight = false;
			m_before_state = MS_RIGHTJUMP;
			Player->setMoveState(m_before_state);
			m_bCan_UpDown = false;
			m_bOnLadder = false;
			g_bViewing = true;
			Player->getCharactorBody()->SetGravityScale(1.0f);
			Player->getCharactorBody()->ApplyLinearImpulse(b2Vec2(m_fImpulse, JUMPPOW + g_CharactorData.status.f_JumpPower), Player->getCharactorBody()->GetWorldCenter(), true);
			log("%f RightJump", 8 + g_CharactorData.status.f_JumpPower);
			CSoundManager::getInstance()->playEffect(SEF_Charactor_Jump_PATH);
		}
		else if (g_isLadderLeft && g_isJump && m_before_state == MS_UPDOWN || m_before_state == MS_ONLADDER) {
			g_isLadderLeft = false;
			m_before_state = MS_LEFTJUMP;
			Player->setMoveState(m_before_state);
			m_bCan_UpDown = false;
			m_bOnLadder = false;
			g_bViewing = false;
			Player->getCharactorBody()->SetGravityScale(1.0f);
			Player->getCharactorBody()->ApplyLinearImpulse(b2Vec2(-m_fImpulse, JUMPPOW + g_CharactorData.status.f_JumpPower), Player->getCharactorBody()->GetWorldCenter(), true);
			log("%f LeftJump", 8 + g_CharactorData.status.f_JumpPower);
			CSoundManager::getInstance()->playEffect(SEF_Charactor_Jump_PATH);
		}
		log("%d %d %f", g_isLadderRight, g_isJump, m_fJumpTimeout);
	}

	else if (gNumFootContacts > 0 && (m_fJumpTimeout -= dt) < 0) {
		//log("Jumpin %d", gNumFootContacts);
		m_fJumpTimeout = 0.07f;
		m_bCan_UpDown = false;
		m_bOnLadder = false;
		m_bCan_Move = true;
		if (m_bDo_Jump) {
			m_bDo_Jump = false;
			if (!g_isLeft && !g_isRight) {
				if (m_before_state == MS_UPDOWN) {
					m_before_state = MS_UPDOWN;
				}
				else {
					m_before_state = MS_STOP;
				}
				m_before_handstate = Hand_On;
			}
			if (m_bTakePickaxe || m_bTakeSword || m_bTakeGun) {
				if (g_isLeft) {
					m_before_handstate = LeftHand_On;
				}
				else if (g_isRight) {
					m_before_handstate = RightHand_On;
				}
				else {
					m_before_handstate = Hand_On;
				}
			}
			if (m_bTakePickaxe)
				HelloWorld::KeyAction(m_before_state, m_before_handstate, "Pickaxe");
			else if (m_bTakeSword)
				HelloWorld::KeyAction(m_before_state, m_before_handstate, "Sword");
			else if (m_bTakeGun)
				HelloWorld::KeyAction(m_before_state, m_before_handstate, "Gun");
			else
				HelloWorld::KeyAction(m_before_state, m_before_handstate, "");
		}
		if (g_isLeft && g_isJump) Player->setMoveState(MS_LEFTJUMP);
		if (g_isRight && g_isJump) Player->setMoveState(MS_RIGHTJUMP);
		if (g_isJump && !g_isLeft && !g_isRight) Player->setMoveState(MS_JUMP);

		b2Vec2 force = Player->getCharactorBody()->GetLinearVelocity();
		if (force.y < 3) {
			switch (Player->getMoveState()) {
			case MS_RIGHTJUMP:
				//if (nJumpStep > 0) {
				Player->getCharactorBody()->ApplyLinearImpulse(b2Vec2(m_fImpulse * 2, JUMPPOW + g_CharactorData.status.f_JumpPower), Player->getCharactorBody()->GetWorldCenter(), true);
				log("%f RightJump", 8 + g_CharactorData.status.f_JumpPower);
				CSoundManager::getInstance()->playEffect(SEF_Charactor_Jump_PATH);
				//	m_nJumpStep--;
				log("RJ");
				nc++;
				//}
				break;
			case MS_LEFTJUMP:
				//if (nJumpStep > 0) {
				Player->getCharactorBody()->ApplyLinearImpulse(b2Vec2(-m_fImpulse * 2, JUMPPOW + g_CharactorData.status.f_JumpPower), Player->getCharactorBody()->GetWorldCenter(), true);
				log("%f LeftJump", 8 + g_CharactorData.status.f_JumpPower);
				CSoundManager::getInstance()->playEffect(SEF_Charactor_Jump_PATH);
				//	m_nJumpStep--;
				log("LJ");
				nc++;
				//	}
				break;
			case MS_JUMP:
				//if (nJumpStep > 0) {
				Player->getCharactorBody()->ApplyLinearImpulse(b2Vec2(0, JUMPPOW + g_CharactorData.status.f_JumpPower), Player->getCharactorBody()->GetWorldCenter(), true);
				log("%f impulse", m_fImpulse * 2 + g_CharactorData.status.f_JumpPower);
				CSoundManager::getInstance()->playEffect(SEF_Charactor_Jump_PATH);
				//Player->getCharactorBody()->ApplyForce(b2Vec2(0, JUMPPOW), Player->getCharactorBody()->GetWorldCenter(), true);
				//body->ApplyForceToCenter(b2Vec2(500,500), true);
				//body->ApplyTorque(45.0f, true);
			//	m_nJumpStep--;
				log("IJ");
				//}
				break;
			}
		}
	}

	if (m_EnableLadder.size() > 0) {
		std::list<b2Body*>::iterator iter;
		for (iter = m_EnableLadder.begin(); iter != m_EnableLadder.end();) {
			b2Body* Ladderbody = *iter;
			Sprite* LadderSp = static_cast<Sprite*>(Ladderbody->GetUserData());
			EffectSprite* CharactorSp = static_cast<EffectSprite*>(Player->getCharactorBody()->GetUserData());
			Rect Ladder_rect = CharactorSp->getBoundingBox();

			if (Ladder_rect.intersectsRect(LadderSp->getBoundingBox())) {
				m_bCan_UpDown = true;
				//Player->getCharactorBody()->SetLinearVelocity(b2Vec2(0, 0));
				b2Vec2 velocity = Player->getCharactorBody()->GetLinearVelocity();
				//log("velocity %f %f on!", velocity.x, velocity.y);
				++iter;
				break;
			}
			else {
				m_bCan_UpDown = false;
				++iter;
			}
		}
	}


	if (gNumObstacleContacts < 1) {
		//log("Obstacle Contacts is No");
	}
	else if (gNumObstacleContacts > 0 && m_fdt > 1.5) {
		m_fdt = 0;
		if (gNumRightContacts > 0) { // Can_Broken는 어디까지나 부술 수 있는 영역을 다루기 때문에 다른 오브젝트와 접촉시 무시되야하므로 장애물과 캐릭터의 접촉시에만 동작하도록
			log("Obstacle Contacts is Yes");

			CSoundManager::getInstance()->playEffect(SEF_Charactor_Pain_PATH);

			m_fDelayTime = 1.5f; // 장애물 충돌 시 딜레이 1초동안 동작을 할 수 없다.
			m_fCharactorPainDelay = 1.5f;
			Player->setMoveState(MS_PAIN);
			// 장애물 충돌시 튕겨지는 힘.
			if (m_bTakePickaxe) {
				KeyAction(Player->getMoveState(), PAIN, "Pickaxe");
			}
			else if (m_bTakeSword) {
				KeyAction(Player->getMoveState(), PAIN, "Sword");
			}
			else if (m_bTakeGun) {
				KeyAction(Player->getMoveState(), PAIN, "Gun");
			}
			else {
				KeyAction(Player->getMoveState(), PAIN, "");
			}
			if (g_bViewing)	Player->getCharactorBody()->ApplyLinearImpulse(b2Vec2(-m_fImpulse * 2 / 3, m_fImpulse * 2 / 3), Player->getCharactorBody()->GetWorldCenter(), true);
			else if (!g_bViewing) Player->getCharactorBody()->ApplyLinearImpulse(b2Vec2(m_fImpulse * 2 / 3, m_fImpulse * 2 / 3), Player->getCharactorBody()->GetWorldCenter(), true);


			g_CharactorData.n_Hp_Count--;
			if (g_CharactorData.n_Hp_Count <= 0) GameOver(this);
			//m_clayer->removeChild(m_HeartLabel, true);

			m_GameUi->Heartmanager(&g_CharactorData);
		}
	}
	// 충돌 후 딜레이
	if (m_fDelayTime > (m_ftime += dt)) {
		g_isLeft = false;
		g_isRight = false;
		g_isStop = false;
		m_fDelayTime = 0;
		m_ftime = 0;
	}
	if (m_ftime > 1) {
		m_ftime = 0;
	}

	if (gNumChipToCharactor > 0) {
		b2Vec2 player_pos = Player->getCharactorBody()->GetPosition();
		std::vector<BombChip>* chips = m_Explosion->getChips();
		std::vector<BombChip>::iterator iter;
		int i = 0;
		for (iter = chips->begin(); iter != chips->end();) {
			b2Body* chip_body = iter->body;
			b2Vec2 chip_pos = chip_body->GetPosition();
			b2Vec2 distance = player_pos - chip_pos;
			float x = fabs(distance.x);
			float y = fabs(distance.y);

			b2Vec2 velocity = chip_body->GetLinearVelocity();

			if (x < 0.5 && y < 0.5 && iter->power == true) {
				iter->power = false;
				m_fCharactorPainDelay = 1.5f;
				Player->setMoveState(MS_PAIN);

				if (m_bTakePickaxe) {
					KeyAction(Player->getMoveState(), PAIN, "Pickaxe");
				}
				else if (m_bTakeSword) {
					KeyAction(Player->getMoveState(), PAIN, "Sword");
				}
				else if (m_bTakeGun) {
					KeyAction(Player->getMoveState(), PAIN, "Gun");
				}
				else {
					KeyAction(Player->getMoveState(), PAIN, "");
				}

				m_fDelayTime = 2;

				if (g_bViewing)
					Player->getCharactorBody()->ApplyLinearImpulse(b2Vec2(-2, 3), Player->getCharactorBody()->GetWorldCenter(), true);
				else if (!g_bViewing)
					Player->getCharactorBody()->ApplyLinearImpulse(b2Vec2(2, 3), Player->getCharactorBody()->GetWorldCenter(), true);

				m_fdt = 0;

				g_CharactorData.n_Hp_Count -= BOMBDAMAGE;
				//_world->DestroyBody(chip_body);
				//m_Explosion->getChips().erase(iter++);
				m_GameUi->Heartmanager(&g_CharactorData);
				if (Player->getCharactorBody()->GetHp() <= 0) {
					//this->clearFloor();
					this->GameOver(this);
				}
			}
			else ++iter;
			i++;
		}
	}

	if (m_bTakePickaxe) {
		MyQueryCallback BrokenCallback;
		m_BrokenAABB.lowerBound = Player->getCharactorBody()->GetPosition() - b2Vec2(1, 1);
		m_BrokenAABB.upperBound = Player->getCharactorBody()->GetPosition() + b2Vec2(1, 1);
		_world->QueryAABB(&BrokenCallback, m_BrokenAABB);

		std::sort(BrokenCallback.foundBodies.begin(), BrokenCallback.foundBodies.end());
		BrokenCallback.foundBodies.erase(std::unique(BrokenCallback.foundBodies.begin(), BrokenCallback.foundBodies.end()), BrokenCallback.foundBodies.end());

		for (int i = 0; i < BrokenCallback.foundBodies.size(); i++) {
			if (g_CharactorData.bStage_blind) {
				log("Pickaxe is not available in Blind area.");
				break;
			}
			b2Body* foundBody = BrokenCallback.foundBodies[i];
			UINT16 category = foundBody->GetFixtureList()->GetFilterData().categoryBits;

			if (category == BLOCK_CATEGORY || category == GBLOCK_CATEGORY) {
				b2Vec2 Charactor_pos = Player->getCharactorBody()->GetPosition();
				b2Vec2 Block_pos = foundBody->GetPosition();
				EffectSprite* sp = static_cast<EffectSprite*>(foundBody->GetUserData());
				if (g_bViewing && !g_isUpBroken && !g_isDownBroken && !g_isRightUpBroken && !g_isLeftUpBroken) {
					if ((Charactor_pos.x * 32 < Block_pos.x * 32 && Charactor_pos.x * 32 + 30 > Block_pos.x * 32) &&
						(Charactor_pos.y * 32 - 15 < Block_pos.y * 32 && Charactor_pos.y * 32 + 15 > Block_pos.y * 32)) {
						sp->setColor(Color3B(125, 125, 125));
						m_BrokenBlockBody = foundBody;
						g_isRightBroken = true;
						break;
					}
					else {
						g_isRightBroken = false;
						sp->setColor(Color3B(255, 255, 255));
					}
				}
				else if (!g_bViewing && !g_isUpBroken && !g_isDownBroken && !g_isRightUpBroken && !g_isLeftUpBroken) {
					if ((Charactor_pos.x * 32 - 40 > Block_pos.x * 32 && Charactor_pos.x * 32 > Block_pos.x * 32) &&
						(Charactor_pos.y * 32 - 15 < Block_pos.y * 32 && Charactor_pos.y * 32 + 15 > Block_pos.y * 32)) {
						sp->setColor(Color3B(125, 125, 125));
						m_BrokenBlockBody = foundBody;
						g_isLeftBroken = true;
						break;
					}
					else {
						g_isLeftBroken = false;
						sp->setColor(Color3B(255, 255, 255));
					}
				}
				if (g_isUpBroken) {
					if ((Charactor_pos.x * 32 - 20 < Block_pos.x * 32 && Charactor_pos.x * 32 + 10 > Block_pos.x * 32) &&
						(Charactor_pos.y * 32 < Block_pos.y * 32 && Charactor_pos.y * 32 + 30 > Block_pos.y * 32)) {
						//Sprite* sp = static_cast<Sprite*>(foundBody->GetUserData());
						sp->setColor(Color3B(125, 125, 125));
						g_isLeftBroken = false;
						g_isRightBroken = false;
						g_isCanUpBroken = true;
						m_BrokenBlockBody = foundBody;
						break;
					}
					else {
						g_isCanUpBroken = false;
						sp->setColor(Color3B(255, 255, 255));
					}
				}
				if (g_isDownBroken) {
					if ((Charactor_pos.x * 32 - 20 < Block_pos.x * 32 && Charactor_pos.x * 32 + 10 > Block_pos.x * 32) &&
						(Charactor_pos.y * 32 - 50 < Block_pos.y * 32 && Charactor_pos.y * 32 > Block_pos.y * 32)) {
						//Sprite* sp = static_cast<Sprite*>(foundBody->GetUserData());
						sp->setColor(Color3B(125, 125, 125));
						g_isLeftBroken = false;
						g_isRightBroken = false;
						g_isCanDownBroken = true;
						m_BrokenBlockBody = foundBody;
						break;
					}
					else {
						g_isCanDownBroken = false;
						sp->setColor(Color3B(255, 255, 255));
					}
				}
				if (g_isRightUpBroken) {
					if ((Charactor_pos.x * 32 < Block_pos.x * 32 && Charactor_pos.x * 32 + 30 > Block_pos.x * 32) &&
						(Charactor_pos.y * 32 < Block_pos.y * 32 && Charactor_pos.y * 32 + 30 > Block_pos.y * 32)) {
						//Sprite* sp = static_cast<Sprite*>(foundBody->GetUserData());
						sp->setColor(Color3B(125, 125, 125));
						g_isLeftBroken = false;
						g_isRightBroken = false;
						g_isCanUpBroken = false;
						g_isCanDownBroken = false;
						g_isCanRightUpBroken = true;
						m_BrokenBlockBody = foundBody;
						break;
					}
					else {
						g_isCanRightUpBroken = false;
						sp->setColor(Color3B(255, 255, 255));
					}
				}
				if (g_isLeftUpBroken) {
					if ((Charactor_pos.x * 32 - 40 > Block_pos.x * 32 && Charactor_pos.x * 32 > Block_pos.x * 32) &&
						(Charactor_pos.y * 32 < Block_pos.y * 32 && Charactor_pos.y * 32 + 30 > Block_pos.y * 32)) {
						//Sprite* sp = static_cast<Sprite*>(foundBody->GetUserData());
						sp->setColor(Color3B(125, 125, 125));
						g_isLeftBroken = false;
						g_isRightBroken = false;
						g_isCanUpBroken = false;
						g_isCanDownBroken = false;
						g_isCanLeftUpBroken = true;
						m_BrokenBlockBody = foundBody;
						break;
					}
					else {
						g_isCanLeftUpBroken = false;
						sp->setColor(Color3B(255, 255, 255));
					}
				}
				//	ccp(Player->getCharactorBody()->GetPosition().x * 32, Player->getCharactorBody()->GetPosition().y * 32 - 50), ccp(Player->getCharactorBody()->GetPosition().x * 32 + 30, Player->getCharactorBody()->GetPosition().y * 32 - 30)
			}
		}

	}


	// 줍기 key 클릭
	if (Player->getMoveState() == MS_GET) {
		// 물약과 충돌시
		if (gNumItemContacts > 0) {
			log("Item Contacts is Yes");
			PickupItem(&Map_Body.lb_Heartlist);

			n++;
			if (n > 10) n = 0;
		}

		if (gNumBombContacts > 0) {
			log("Bomb Contacts is Yes");
			PickupItem(&Map_Body.lb_Bomblist);
		}

		if (gNumLadderContacts > 0) {
			log("Ladder Contacts is Yes");
			PickupItem(&Map_Body.lb_Ladderlist);
		}

		if (gNumGoldContacts > 0) {
			log("Gold Contacts is Yes");
			PickupItem(&Map_Body.lb_Goldlist);
		}

		if (gNumShopManContacts > 0) {
			log("Shop Man Contacts is Yes");
			PickupItem(&Map_Body.lb_Shopmanlist);
		}

		// 곡괭이 아이템이 지면과 닿아있고 손에 든 것이 아무것도 없을 때
		if (gNumpickaxeContacts > 0 && !m_bTakePickaxe && !m_bTakeSword && !m_bTakeGun) {
			log("pickaxe contacts");
			PickupItem(&Map_Body.lb_Pickaxelist);

			n++;
			if (n > 10) n = 0;
		}
		// 칼 아이템이 지면과 닿아있고 손에 아무것도 든 것이 없을 때
		if (gNumSwordContacts > 0 && !m_bTakeSword && !m_bTakePickaxe && !m_bTakeGun) {
			log("sword contacts");
			PickupItem(&Map_Body.lb_Swordlist);

			n++;
			if (n > 10) n = 0;
		}
		// 총 아이템이 지면과 닿아있고 손에 아무것도 든 것이 없을 때
		if (gNumGunContacts > 0 && !m_bTakeGun && !m_bTakePickaxe && !m_bTakeSword) {
			log("gun contacts");
			PickupItem(&Map_Body.lb_Gunlist);

			n++;
			if (n > 10) n = 0;
		}
		if (gNumRedKeyContacts > 0 && g_CharactorData.key == NONE) {
			PickupItem(&Map_Body.lb_RedKeylist);

			n++;
			if (n > 10) n = 0;
		}
		if (gNumBlueKeyContacts > 0 && g_CharactorData.key == NONE) {
			PickupItem(&Map_Body.lb_BlueKeylist);

			n++;
			if (n > 10) n = 0;
		}
		if (gNumGreenKeyContacts > 0 && g_CharactorData.key == NONE) {
			PickupItem(&Map_Body.lb_GreenKeylist);

			n++;
			if (n > 10) n = 0;
		}
	}
	m_EPlayerSprite = static_cast<EffectSprite*>(Player->getCharactorBody()->GetUserData());
	Rect rect = m_EPlayerSprite->getBoundingBox();
	// key를 들고있으면 key color에 따라 같은 color의 보물상자에 접근했을때 Open Button이 활성화 되도록 해준다.
	// 또한 보물상자는 SHELTER Stage에만 존재하기때문에 Game Stage에는 버튼을 활성화 시켜줄 필요가 없다.
	if (g_CharactorData.key != NONE && g_CharactorData.StageNumber == SHELTER) {
		switch (g_CharactorData.key) {
		case REDKEY:
			this->OpenButtonEnable(&rect, Map_Body.getRedTreasureChestSp());
			break;
		case BLUEKEY:
			this->OpenButtonEnable(&rect, Map_Body.getBlueTreasureChestSp());
			break;
		case GREENKEY:
			this->OpenButtonEnable(&rect, Map_Body.getGreenTreasureChestSp());
			break;
		default:
			break;
		}
	}


	//log("Player Velocity = (%f , %f)", Player->getCharactorBody()->GetLinearVelocity().x, Player->getCharactorBody()->GetLinearVelocity().y);

	// 캐릭터가 몬스터와 충돌 후 캐릭터는 바라보는 방향 반대로 튕겨나가게 되는데 이순간 body에 또 다른 힘을 주면 impulse +연산되어
	// 캐릭터가 의도와 다르게 크게 튕겨나가는 경우가 발생하기 때문에 캐릭터의 Velocity가 일정 수준을 넘어가면 튕겨나가는 정도의 상한을 두었다.
	/*
	if (m_Charactor_body->GetLinearVelocity().x > 19 || m_Charactor_body->GetLinearVelocity().y > 14 &&
		m_Charactor_body->GetLinearVelocity().x > -19) {
		log("Player Velocity = (%f , %f)", Player->getCharactorBody()->GetLinearVelocity().x, Player->getCharactorBody()->GetLinearVelocity().y);
		log("!?");

		if(g_bViewing) m_Charactor_body->SetLinearVelocity(b2Vec2(-3, 5));
		else if (!g_bViewing) m_Charactor_body->SetLinearVelocity(b2Vec2(3, 5));
	}*/


	m_clayer->setPosition(Vec2(wlayer->getPosition() * -1));	// ui Layer를 캐릭터와 함께 이동시켜준다.
	m_RecognizeSp->setPosition(m_CameraSprite->getPosition()); // 캐릭터 인식 범위를 캐릭터와 함께 이동

	m_fdt += dt;
	m_Monster->MonsterPatrol(_world, &Monsters, Player->getCharactorBody(), dt, wlayer, gNumMonsterFootContacts);
	m_Opossum->MonsterPatrol(_world, &Monsters, Player->getCharactorBody(), dt, wlayer, gNumMonsterFootContacts);
	m_Frog->MonsterPatrol(_world, &Monsters, Player->getCharactorBody(), dt, wlayer, gNumFrogFootContacts);
	m_Bat->MonsterPatrol(_world, &Monsters, Player->getCharactorBody(), dt, wlayer, gNumMonsterFootContacts);
	if (g_CharactorData.StageNumber == BOSS) {
		m_Boss->MonsterPatrol(_world, Player->getCameraBody(), dt, GunEffect, wlayer);
		if (g_CharactorData.n_Hp_Count < nbefore_hp) {
			CSoundManager::getInstance()->playEffect(SEF_Charactor_Pain_PATH);

			m_fCharactorPainDelay = 1.5f;
			Player->setMoveState(MS_PAIN);
			if (m_bTakePickaxe) {
				KeyAction(Player->getMoveState(), PAIN, "Pickaxe");
			}
			else if (m_bTakeSword) {
				KeyAction(Player->getMoveState(), PAIN, "Sword");
			}
			else if (m_bTakeGun) {
				KeyAction(Player->getMoveState(), PAIN, "Gun");
			}
			else {
				KeyAction(Player->getMoveState(), PAIN, "");
			}

			m_fDelayTime = 2;

			if (g_bViewing)
				Player->getCharactorBody()->ApplyLinearImpulse(b2Vec2(-2, 3), Player->getCharactorBody()->GetWorldCenter(), true);
			else if (!g_bViewing)
				Player->getCharactorBody()->ApplyLinearImpulse(b2Vec2(2, 3), Player->getCharactorBody()->GetWorldCenter(), true);

			m_fdt = 0;

			if (g_CharactorData.n_Hp_Count <= 0) GameOver(this);
			//m_clayer->removeChild(m_HeartLabel, true);

			m_GameUi->Heartmanager(&g_CharactorData);
		}
		
		if (m_Boss->getBossState() == BOSS_PAIN) {
			float interval = 1 / 60;
			float duration = 0.8f;
			float speed = 6.0f;
			float magnitude = 4.f;
			this->runAction(ActionShake::create(duration, speed, magnitude));
		}
	}
	// 몬스터 충돌 및 죽음 체크

	std::list<Monster>::iterator iter;
	for (iter = Monsters.begin(); iter != Monsters.end();) { // 몬스터 리스트를 돌며 죽은 몬스터가 있는지 검사하고 있으면 죽은상태 애니메이션 출력 후 삭제.
		Sprite* sp = static_cast<Sprite*>(iter->body->GetUserData());
		Sprite* recognize_sp = static_cast<Sprite*>(iter->recognize_body->GetUserData());

		Rect Monster_rect = sp->getBoundingBox();

		b2Vec2 distance = iter->body->GetPosition() - m_Charactor_body->GetPosition();
		
		// 몬스터 body와 캐릭터 좌우body가 충돌 할 경우 두 거리를 계산해 가장 가까운 body
		if (gNumMonsterContacts > 0) {
			if (distance.x > -1.5 && distance.x < 1.5 &&
				distance.y > -0.5 && distance.y < 0.5 && m_fdt > 1.5) {
				log("Monster Contacts is Yes");

				CSoundManager::getInstance()->playEffect(SEF_Charactor_Pain_PATH);
				
				m_fCharactorPainDelay = 1.5f;
				Player->setMoveState(MS_PAIN);
				if (m_bTakePickaxe) {
					KeyAction(Player->getMoveState(), PAIN, "Pickaxe");
				}
				else if (m_bTakeSword) {
					KeyAction(Player->getMoveState(), PAIN, "Sword");
				}
				else if (m_bTakeGun) {
					KeyAction(Player->getMoveState(), PAIN, "Gun");
				}
				else {
					KeyAction(Player->getMoveState(), PAIN, "");
				}
				
				m_fDelayTime = 2;

				if (g_bViewing) 
					Player->getCharactorBody()->ApplyLinearImpulse(b2Vec2(-2, 3), Player->getCharactorBody()->GetWorldCenter(), true);
				else if (!g_bViewing) 
					Player->getCharactorBody()->ApplyLinearImpulse(b2Vec2(2, 3), Player->getCharactorBody()->GetWorldCenter(), true);
				
				m_fdt = 0;
				g_CharactorData.n_Hp_Count--;
				if (g_CharactorData.n_Hp_Count <= 0) GameOver(this);
				//m_clayer->removeChild(m_HeartLabel, true);

				m_GameUi->Heartmanager(&g_CharactorData);
			}
		}
		if (iter->state == MS_DEATH && iter->time < m_fdt) {
			wlayer->removeChild(sp, true);
			wlayer->removeChild(recognize_sp, true);
			_world->DestroyBody(iter->body);
			_world->DestroyBody(iter->recognize_body);
			Monsters.erase(iter++);
		}
		else ++iter;
	}

	if (gNumFootAttackContacts > 0) {
		AttackMonster(dt);
	}

	// 충돌 후 딜레이
	if (m_fDelayTime > (m_ftime += dt)) {
		g_isLeft = false;
		g_isRight = false;
		g_isStop = false;
	}
	if (m_ftime >= 2) {
		m_ftime = 0;
	}

	if (g_isLeft || g_isRight || g_isStop) {
		float velChange = desiredVel - vel.x;
		float impulse = Player->getCharactorBody()->GetMass() * velChange;

		Player->getCharactorBody()->ApplyLinearImpulse(b2Vec2(impulse, 0), Player->getCharactorBody()->GetWorldCenter(), true);

		log("%f", impulse);
	}
	if (g_isUp && !g_isDown) {
		float velChange = desiredVel - vel.y;
		float impulse = Player->getCharactorBody()->GetMass() * velChange;
		if (gNumLongLadderContacts > 0) { // 사다리 상한에 닿았을때 더 올라갈 수 없도록.
			impulse = 0;
		}

		Player->getCharactorBody()->SetLinearVelocity(b2Vec2(0, impulse));
		log("impulse %f", impulse);
	}
	if (g_isDown && !g_isUp) {
		float velChange = desiredVel - vel.y;
		float impulse = Player->getCharactorBody()->GetMass() * velChange;

		Player->getCharactorBody()->SetLinearVelocity(b2Vec2(0, -1.5));
		log("down!!!");
	}

	if (g_CharactorData.StageNumber == SHELTER && m_PortalSp != nullptr) {
		Rect rect = m_PortalSp->getBoundingBox();
		Sprite* player = static_cast<Sprite*>(Player->getCharactorBody()->GetUserData());

		if (rect.intersectsRect(player->getBoundingBox())) {
			m_pPortalButton->setEnabled(true);
			m_pPortalButton->setVisible(true);
		}
	}

	// Step : 물리 세계를 시뮬레이션 한다.
	_world->Step(dt, velocityIterations, positionIterations);

	// 모든 물리 객체들은 링크드 리스트에 저장되어 참조해 볼 수 있도록 구현되어 있다.
	// 만들어진 객체 만큼 루프를 돌리면서 바디에 붙인 스프라이트를 여기서 제어한다.

	for (b2Body* b = _world->GetBodyList(); b; b = b->GetNext()) {
		if (b->GetUserData() != nullptr) {
			Sprite* spriteData = (Sprite*)b->GetUserData();
			spriteData->setPosition(Vec2(b->GetPosition().x * PTMRATIO, b->GetPosition().y * PTMRATIO));
			spriteData->setRotation(-1 * CC_RADIANS_TO_DEGREES(b->GetAngle()));
			
			Sprite* Charactor = m_RecognizeSp;
			//log("%f, %f", Charactor->getPosition().x, Charactor->getPosition().y);
			Rect rect = spriteData->getBoundingBox();
			uint16 category = b->GetFixtureList()->GetFilterData().categoryBits;
			
			
			if (g_CharactorData.StageNumber == SHELTER || g_CharactorData.bStage_blind) {
				if (category == BLOCK_CATEGORY || category == 0x0010 || category == POTION_CATEGORY || category == OBSTACLE_CATEGORY || category == LADDER_CATEGORY || category == BOMB_CATEGORY || category == GUN_CATEGORY || category == SWORD_CATEGORY || category == PICKAXE_CATEGORY ||
					category == GBLOCK_CATEGORY || category == GOLD_CATEGORY || category == LLADDER_CATEGORY || category == HARDBLOCK_CATEGORY || category == SHOP_CATEGORY || category == REDKEY_CATEGORY || category == BLUEKEY_CATEGORY || category == GREENKEY_CATEGORY ||
					category == REDCHEST_CATEGORY || category == BLUECHEST_CATEGORY || category == GREENCHEST_CATEGORY || category == BLADEBLOCK_CATEGORY || category == FALLBLADE_CATEGORY|| category == LGUNTRAP_CATEGORY || category == RGUNTRAP_CATEGORY || category == LBLOCK_CATEGORY ||
					category == BRIDGE_CATEGORY || category == LBRIDGE_CATEGORY || category == BIGBLOCK_CATEGORY || category == BIGLBLOCK_CATEGORY || category == BIGHBLOCK_CATEGORY || category == LHARDBLOCK_CATEGORY) {
					EffectSprite* ES = static_cast<EffectSprite*>(b->GetUserData());
					ES->setCharactorPos(Vec2(Player->getCharactorBody()->GetPosition().x * 32, Player->getCharactorBody()->GetPosition().y * 32));
					//Sprite* Sp = static_cast<Sprite*>(b->GetUserData());
					//Sp->setVisible(false);
				}
				if (category == 0x0006) {
					m_EFFSprite = static_cast<EffectSprite*>(b->GetUserData());
					m_EFFSprite->setCharactorPos(Vec2(m_EFFSprite->getPosition()));
				}
			}
			
			

			// 캐릭터 부속 body 및 body의 userdata는 componentQ에서 제거해주기 때문에 기존body의 destroyQ에는 올라가지 않는다.
			if (category == COMPONENT_CATEGORY) {
				continue;
			}
			if (g_CharactorData.StageNumber != SHELTER) {
				if (rect.intersectsRect(m_Obstacle->getLavaSprite()->getBoundingBox()) || rect.intersectsRect(m_Obstacle->getLavaSpriteb()->getBoundingBox())) {
					int hp = b->GetHp();
					hp--;
					b->SetHp(hp);

					if (category == CHARACTOR_CATEGORY && (m_fCharactorPainDelay -= dt) <= 0) {
						m_fCharactorPainDelay = 2.0f;

						g_CharactorData.n_Hp_Count = g_CharactorData.n_Hp_Count - 2;
						m_clayer->removeChild(m_HeartLabel, true);
						
						m_GameUi->Heartmanager(&g_CharactorData);
						
						ParticleSystemQuad* Burn_Collision;
						Burn_Collision = ParticleSystemQuad::create("Images/effects/Burn_Collision.plist");
						Burn_Collision->setPosition(Vec2(Player->getCharactorBody()->GetPosition().x * PTMRATIO, Player->getCharactorBody()->GetPosition().y * PTMRATIO));
						Burn_Collision->setScale(0.3f);
						Burn_Collision->setAutoRemoveOnFinish(true);
						wlayer->addChild(Burn_Collision, 100);
						if (nbefore_hp != g_CharactorData.n_Hp_Count) {
							this->CharactorPainAction();
						}
					}
					else if (category == BLOCK_CATEGORY && b->GetBroken()) {
						b->SetBroken(false);
						std::string sPath = "n_Images/block/meltblock_.plist";
						std::string sPath_n = "n_Images/block/meltblock_n.png";
						Map_Body.setEffectAnimationSprite(b, sPath, 7, wlayer);
					}
					//if (b->GetType() == b2_staticBody) {
					//	b->SetType(b2_dynamicBody);
					//}

				}
			}
			if (category == 0x0001) {
				m_EFFSprite = static_cast<EffectSprite*>(b->GetUserData());
				m_EFFSprite->setCharactorPos(Vec2(m_EFFSprite->getPosition()));

				uint16 category = b->GetFixtureList()->GetFilterData().categoryBits;

				continue;
			}
			
			if (b->GetHp() <= 0 && (m_destroytime -= dt) <= 0) {
				m_DestroyQ.push(b);
				m_destroytime = 0.7f;
			}
		}
	}

//	HelloWorld::Destroy_Component(dt);

	for (int i = 0; i < m_DestroyQ.size() + i; i++) {
		b2Body* body = m_DestroyQ.front();
		
		EffectSprite* spriteData = static_cast<EffectSprite*>(body->GetUserData());
		
	
		//Sprite* spriteData = static_cast<Sprite*>(body->GetUserData());
		
		b2Body* bd;
		Sprite* otherData;
		std::vector<b2Body*> joint_body;
		std::vector<b2Joint*>::iterator iter;
		if (body->GetJointList() != nullptr) {
			b2Joint* jointfront = body->GetJointList()->joint;
			b2Joint* j = body->GetJointList()->joint;
			b2JointEdge* jointlist = body->GetJointList();
			for (b2Joint* joint = body->GetJointList()->joint; joint != nullptr; joint = joint->GetNext()) {
				// 이게 어떻게되있는건지 body가 서로 달라도 jointlist는 모든body의 jointbody가 연결되어있다 말도안돼
				// jointlist에서 joint로 연결된 body가 어떤body인지 구분하기위해 category bits를 부여해 구분한뒤 body에 붙어있는 부속body들만 리스트에 넣어 후에 제거해준다.
				// Monster 부속 body category bits 는 0x0030
				UINT16 category = joint->GetBodyA()->GetFixtureList()->GetFilterData().categoryBits;
				UINT16 category2 = joint->GetBodyB()->GetFixtureList()->GetFilterData().categoryBits;
				if(category == 0x0030) 
					joint_body.push_back(joint->GetBodyA());
				if (category2 == 0x0030) 
					joint_body.push_back(joint->GetBodyB());
			}
			std::sort(joint_body.begin(), joint_body.end());
			joint_body.erase(std::unique(joint_body.begin(), joint_body.end()), joint_body.end());
		}

		UINT16 obj_category = body->GetFixtureList()->GetFilterData().categoryBits;
		switch (obj_category) {
		case 0x0002:
		case 0x0100:
			wlayer->removeChild((Sprite*)body->GetUserData(), true);
			break;
		case 0x0007:
			m_Gold_pos = body->GetPosition();
			wlayer->removeChild(spriteData, true);
			m_Gold = Map_Body.CreateObj("Gold", m_Gold_pos, _world, wlayer);
			m_Gold->ApplyLinearImpulse(b2Vec2(0, 7), m_Gold->GetWorldCenter(), true);
			break;

		case 0x0010:
			wlayer->removeChild(spriteData, true);
	//		for (int i = 0; i < joint_body.size(); i++) {
		//		bd = joint_body.at(i);

		//		otherData = static_cast<Sprite*>(bd->GetUserData());
		//		wlayer->removeChild(otherData, true);
		//		_world->DestroyBody(bd);
				// 부속body는 Hp를 높게 설정했기때문에 DestoryQ에 올라오지 않는다. 따라서 Q에서 제거해줄 필요가 없다
			//}

			break;

		case 0x2000:
		case 0x3000:
			if (body->GetNewobj()) wlayer->removeChild(spriteData, true);
			else wlayer->removeChild(spriteData, true);
			break;
		case LLADDER_CATEGORY:
			m_EnableLadder.remove(body);
			break;
		case BULLET_CATEGORY:
			wlayer->removeChild(spriteData, true);
			break;
		}
		_world->DestroyBody(body);
		m_DestroyQ.pop();
	}
}

void HelloWorld::Destroy_Component(float dt) {
	std::vector<Monster_Component>::iterator iter;
	for (iter = m_Monster_Component->begin(); iter != m_Monster_Component->end();) {
		if (!iter->body->IsAwake()) {
			if ((iter->time -= dt) <= 0 && (iter->body->GetJointList() == nullptr)) {
				m_DestroyQ.push(iter->body);
				iter = m_Monster_Component->erase(iter);
			}
			else { iter++; }
		}
		else {
			iter->time = 10;
			iter++;
		}
	}
}

void HelloWorld::power_enegy(float power) {
	m_fpowertime += power;
}

void HelloWorld::TouchUpdate(float dt) {
	if (Player->getMoveState() == MS_PAIN) {
		return;
	}
	Vec2 centerPos = m_joystick->getCenterPos();
	Vec2 controlerPos = m_joystick->getControlerPos();

	if (m_joystick->getisTouch()) {
		float moveX = controlerPos.x - centerPos.x;
		float moveY = controlerPos.y - centerPos.y;

		float angle = m_joystick->getAngle(moveX, moveY);

		float degree = angle * RADTODEG;
		if (degree > -22.5 && degree < 22.5) {
			PlayCharactorStep();
			InitCharactorOption();
			
			if (m_bCan_Move && Player->getMoveState() != MS_UPDOWN && !m_bOnLadder) {
				if (!g_isThrow) {
					m_before_state = MS_RIGHT;
					m_before_handstate = RightHand_On;
					g_isUp = false;
					g_isDown = false;
					g_isLeft = false;
					g_isLadderRight = false;
					g_isLadderLeft = false;
					g_isRight = true;
					g_bViewing = true;
					g_isAttackOn = true;
					if (m_before_state != Player->getMoveState()){ // Frame Update마다 JoyStick position Cheak하기때문에 body Animation Sprite trade function에 동작이 바뀔때만 호출되도록 해야 똑같은 Sprite를 교체하지 않게 된다.
						if (m_bTakePickaxe) 
							HelloWorld::KeyAction(MS_RIGHT, RightHand_On, "Pickaxe");
						else if (m_bTakeSword)
							HelloWorld::KeyAction(MS_RIGHT, RightHand_On, "Sword");
						else if (m_bTakeGun)
							HelloWorld::KeyAction(MS_RIGHT, RightHand_On, "Gun");
						else
							HelloWorld::KeyAction(MS_RIGHT, RightHand_On, "");
					}
					g_isRightUpBroken = false;
					g_isLeftUpBroken = false;
				}
			}
			
			else if (!m_bCan_Move && Player->getMoveState() == MS_UPDOWN || Player->getMoveState() == MS_ONLADDER && m_bOnLadder) {
				Player->getCharactorBody()->SetGravityScale(-0.0338f);
				m_before_state = MS_UPDOWN;
				g_isUp = false;
				g_isDown = false;
				g_isLeft = false;
				g_isRight = false;
				g_isLadderRight = true;
				g_isLadderLeft = false;
				//log("Ladder On Right ready");
			}
			
			//log("right");
		}
		if (degree > 22.5 && degree < 67.5) {
			g_isLeft = false;
			g_isLadderLeft = false;
			g_isRight = false;
			g_isLadderRight = false;
			g_isRightBroken = false;
			g_isLeftBroken = false;
			g_isRightUpBroken = true;
			m_before_state = MS_RIGHTUP;
			log("right up");
		}
		if (degree > 67.5 && degree < 112.5) {
			CSoundManager::getInstance()->stopBgm(BGM_Charactor_Step_PATH);
			if (!g_isThrow) {
				g_isUpBroken = true;
				if (m_bCan_UpDown) {
					log("up");
					Player->getCharactorBody()->SetGravityScale(-0.0338f);
					g_isRight = false;
					g_isLeft = false;
					g_isLadderLeft = false;
					g_isLadderRight = false;
					g_isDown = false;
					g_isUp = true;
					g_isLadderStop = false;
					
					if (m_before_state != Player->getMoveState() || m_before_state == MS_STOP) {
						if (m_bTakePickaxe)
							HelloWorld::KeyAction(MS_UPDOWN, UPDOWN, "Pickaxe");
						else if (m_bTakeSword)
							HelloWorld::KeyAction(MS_UPDOWN, UPDOWN, "Sword");
						else if (m_bTakeGun)
							HelloWorld::KeyAction(MS_UPDOWN, UPDOWN, "Gun");
						else
							HelloWorld::KeyAction(MS_UPDOWN, Hand_On, "");
					}
					Player->setMoveState(MS_UPDOWN);
					m_before_state = MS_UPDOWN;
					g_isRightUpBroken = false;
					g_isLeftUpBroken = false;
				}
			}
			
		}
		if (degree > 112.5 && degree < 157.5) {
			g_isLeft = false;
			g_isLadderLeft = false;
			g_isRight = false;
			g_isLadderRight = false;
			g_isRightBroken = false;
			g_isLeftBroken = false;
			g_isLeftUpBroken = true;
			m_before_state = MS_LEFTUP;
			log("left up");
		}
		if (degree > 157.5 && degree < 180) {
			PlayCharactorStep();
			InitCharactorOption();

			if (m_bCan_Move && Player->getMoveState() != MS_UPDOWN && !m_bOnLadder) {
				if (!g_isThrow) {
					g_isUp = false;
					g_isDown = false;
					g_isRight = false;
					g_isLadderLeft = false;
					g_isLadderRight = false;
					g_isLeft = true;									
					g_bViewing = false;
					g_isAttackOn = true;
					if (m_before_state != Player->getMoveState()) {
						if (m_bTakePickaxe) 
							HelloWorld::KeyAction(MS_LEFT, LeftHand_On, "Pickaxe");
						else if (m_bTakeSword)
							HelloWorld::KeyAction(MS_LEFT, LeftHand_On, "Sword");
						else if (m_bTakeGun)
							HelloWorld::KeyAction(MS_LEFT, LeftHand_On, "Gun");
						else
							HelloWorld::KeyAction(MS_LEFT, LeftHand_On, "");
					}
					g_isRightUpBroken = false;
					g_isLeftUpBroken = false;
					m_before_state = MS_LEFT;
					m_before_handstate = LeftHand_On;
				}
			}

			else if (!m_bCan_Move && Player->getMoveState() == MS_UPDOWN || Player->getMoveState() == MS_ONLADDER && m_bOnLadder) {
				Player->getCharactorBody()->SetGravityScale(-0.0338f);
				m_before_state = MS_UPDOWN;
				g_isUp = false;
				g_isDown = false;
				g_isLeft = false;
				g_isRight = false;
				g_isLadderRight = false;
				g_isLadderLeft = true;
				log("Ladder On Left ready");
			}
			log("left");
		}
		if (degree > -180 && degree < -157.5) {
			PlayCharactorStep();
			InitCharactorOption();

			if (m_bCan_Move && Player->getMoveState() != MS_UPDOWN && !m_bOnLadder) {
				if (!g_isThrow) {
					g_isRight = false;
					g_isLadderLeft = false;
					g_isLadderRight = false;
					g_isLeft = true;									
					g_bViewing = false;
					g_isAttackOn = true;
					if (m_before_state != Player->getMoveState()) {
						if (m_bTakePickaxe) {
							HelloWorld::KeyAction(MS_LEFT, LeftHand_On, "Pickaxe");
						}
						else if (m_bTakeSword)
							HelloWorld::KeyAction(MS_LEFT, LeftHand_On, "Sword");
						else if (m_bTakeGun)
							HelloWorld::KeyAction(MS_LEFT, LeftHand_On, "Gun");
						else
							HelloWorld::KeyAction(MS_LEFT, LeftHand_On, "");
					}
					m_before_state = MS_LEFT;
					m_before_handstate = LeftHand_On;
					g_isRightUpBroken = false;
					g_isLeftUpBroken = false;
				}
			}
			else if (!m_bCan_Move && Player->getMoveState() == MS_UPDOWN || Player->getMoveState() == MS_ONLADDER && m_bOnLadder) {
				Player->getCharactorBody()->SetGravityScale(-0.0338f);
				m_before_state = MS_UPDOWN;
				g_isUp = false;
				g_isDown = false;
				g_isLeft = false;
				g_isRight = false;
				g_isLadderRight = false;
				g_isLadderLeft = true;
				log("Ladder On Left ready");
			}
			log("left");
		}
		if (degree > -157.5 && degree < -112.5) {
			log("left down");
		}
		if (degree > -112.5 && degree < -67.5) {
			if (!g_isThrow) {
				g_isDownBroken = true;
				if (m_bCan_UpDown) {
					log("down");
					Player->getCharactorBody()->SetGravityScale(-0.0355f);
					g_isDown = true;
					g_isUp = false;
					g_isLadderRight = false;
					g_isLadderLeft = false;
					g_isLadderStop = false;
					if (m_before_state != Player->getMoveState()) {
						if (m_bTakePickaxe)
							HelloWorld::KeyAction(MS_UPDOWN, UPDOWN, "Pickaxe");
						else if (m_bTakeSword)
							HelloWorld::KeyAction(MS_UPDOWN, UPDOWN, "Sword");
						else if (m_bTakeGun)
							HelloWorld::KeyAction(MS_UPDOWN, UPDOWN, "Gun");
						else
							HelloWorld::KeyAction(MS_UPDOWN, Hand_On, "");
					}
					Player->setMoveState(MS_UPDOWN);
					m_before_state = MS_UPDOWN;
					g_isRightUpBroken = false;
					g_isLeftUpBroken = false;
				}
			}
			
		}
		if (degree > -67.5 && degree < -22.5) {
			log("right down");
		}
	}
	m_joystick->update(dt);
}


void HelloWorld::onTouchesBegan(const std::vector<Touch*> &touches, Event* event) {
	m_bTouchHolding = true;
	m_clayer->removeChild(m_HeartLabel, true);
	/*
	m_touchPoint = touch->getLocation();
	m_touchPoint2 = Node::convertToNodeSpace(m_touchPoint);
	log("nodeSpace.. %f, %f", m_touchPoint2.x, m_touchPoint2.y);
	*/
	m_joystick->onTouchesBegan(touches, event);
}

void HelloWorld::onTouchesMoved(const std::vector<Touch*> &touches, Event* event) {
	m_joystick->onTouchesMoved(touches, event);
}

void HelloWorld::onTouchesEnded(const std::vector<Touch*> &touches, Event* event) {
	m_bTouchHolding = false;
	// 캐릭터의 왼손에 쥔 아이템 내구도가 0이 될때
	if (m_bTakePickaxe && Player->getCharactor_Hand()->getDurability() <= 0) {
		m_bTakePickaxe = false;
		
		if (g_CharactorData.StageNumber == SHELTER || g_CharactorData.bStage_blind) // 지워줄 Sprite가 EffectSprite or Sprite Data
			wlayer->removeChild(Player->getCharactor_Hand()->getSprite()->ESpriteData, true);
		else
			wlayer->removeChild(Player->getCharactor_Hand()->getSprite()->SpriteData, true);

		m_PlayerSprite = Player->AnimationState(MS_STOP, wlayer);		// 다시 body에 movestate 상태에 따라 애니메이션을 만들어 넣어줌
		m_PlayerSprite = Player->getPlayer();

		if (g_CharactorData.StageNumber == SHELTER || g_CharactorData.bStage_blind) {
			if (g_bViewing) m_PlayerSprite->ESpriteData->setFlippedX(false);
			else if (!g_bViewing) m_PlayerSprite->ESpriteData->setFlippedX(true);
		}
		else {
			if (g_bViewing) m_PlayerSprite->SpriteData->setFlippedX(false);
			else if (!g_bViewing) m_PlayerSprite->SpriteData->setFlippedX(true);
		}
	}
	/*
	else if(m_bTakePickaxe) {
		if (g_CharactorData.StageNumber == SHELTER || g_CharactorData.bStage_blind)
			wlayer->removeChild(Player->getCharactor_Hand()->getSprite()->ESpriteData, true);
		else 
			wlayer->removeChild(Player->getCharactor_Hand()->getSprite()->SpriteData, true);
		
		m_PlayerSprite = Player->getCharactor_Hand()->AnimationState("Pickaxe", Hand_On, wlayer, g_bViewing);
		m_PlayerSprite = Player->getPlayer();				// HelloWorld Scene에 넣기 위해 Sprite 가져옴

		if (g_CharactorData.StageNumber == SHELTER || g_CharactorData.bStage_blind) {
			if (g_bViewing) m_PlayerSprite->ESpriteData->setFlippedX(false);
			else if (!g_bViewing) m_PlayerSprite->ESpriteData->setFlippedX(true);
		}
		else {
			if (g_bViewing) m_PlayerSprite->SpriteData->setFlippedX(false);
			else if (!g_bViewing) m_PlayerSprite->SpriteData->setFlippedX(true);
		}
	}*/

	if (m_bCan_UpDown && gNumFootContacts < 1) {
		if (g_isUp || g_isDown || g_isRight || g_isLeft || g_isLadderRight || g_isLadderLeft ||m_before_state == MS_RIGHTUP || m_before_state == MS_LEFTUP) {
			m_bMove = false;
			Player->getCharactorBody()->SetGravityScale(-0.035f);
			Player->getCharactorBody()->SetLinearVelocity(b2Vec2(0, 0));
			Sprite* sp = static_cast<Sprite*>(Player->getCharactorBody()->GetUserData());
			sp->stopAllActions();
			if (!g_isThrow) {
				g_isUpBroken = false;
				g_isUp = false;
				g_isDown = false;
				g_isRight = false;
				g_isLeft = false;
				g_isLadderStop = true;
			}
		}
		else {
			Player->getCharactorBody()->SetType(b2_dynamicBody);
			Player->getCharactorBody()->SetGravityScale(1.0f);
		}
	}

	g_isDownBroken = false;
	if (!g_isThrow && Player->getMoveState() != MS_UPDOWN && Player->getMoveState() != MS_PAIN) {
		if (m_bTakePickaxe) {
			switch (m_before_state) {
			case MS_RIGHT:
				g_bViewing = true;
				g_isLeft = false;
				g_isLeftBroken = false;
				g_isUp = false;
				g_isUpBroken = false;
				g_isDown = false;
				g_isDownBroken = false;
				break;
			case MS_LEFT:
				g_bViewing = false;
				g_isRight = false;
				g_isRightBroken = false;
				g_isUp = false;
				g_isUpBroken = false;
				g_isDown = false;
				g_isDownBroken = false;
				break;
			case MS_RIGHTUP:
				g_isRight = false;
				g_isRightBroken = false;
				g_isUp = false;
				g_isUpBroken = false;
				g_isDown = false;
				g_isDownBroken = false;
				g_isRightUpBroken = true;
				break;
			case MS_LEFTUP:
				g_isRight = false;
				g_isRightBroken = false;
				g_isUp = false;
				g_isUpBroken = false;
				g_isDown = false;
				g_isDownBroken = false;
				g_isLeftUpBroken = true;
				break;
			}
		}
		else {
			g_isUp = false;
			g_isDown = false;
			g_isStop = false;
		}
		g_isRight = false;
		g_isLeft = false;
		g_isAttackOn = true;
		// 마우스 클릭을 땟을때 사다리를 탄 상태 + 사다리 Sprite와 캐릭터 Sprite가 겹쳐있을때 Stop 애니메이션 출력되지 않고 기존의 Sprite 유지
		if (m_before_state != MS_RIGHTUP && m_before_state != MS_LEFTUP && m_before_state != MS_UPDOWN) {
			if (m_bTakePickaxe)
				HelloWorld::KeyAction(MS_STOP, Hand_On, "Pickaxe");
			else if (m_bTakeSword)
				HelloWorld::KeyAction(MS_STOP, Hand_On, "Sword");
			else if (m_bTakeGun)
				HelloWorld::KeyAction(MS_STOP, Hand_On, "Gun");
			else {
				HelloWorld::KeyAction(MS_STOP, Hand_On, "");
				log("MS_STATE = %d", m_before_state);
			}
			m_before_state = MS_STOP;
		}
		if (g_CharactorData.StageNumber == SHELTER || g_CharactorData.bStage_blind) {
			if (g_bViewing) m_PlayerSprite->ESpriteData->setFlippedX(false);
			else if (!g_bViewing) m_PlayerSprite->ESpriteData->setFlippedX(true);
		}
		else {
			if (g_bViewing) m_PlayerSprite->SpriteData->setFlippedX(false);
			else if (!g_bViewing) m_PlayerSprite->SpriteData->setFlippedX(true);
		}
	}
	m_joystick->onTouchesEnded(touches, event);
}

bool HelloWorld::LadderControl() {
	if (m_bCan_UpDown) {
		//g_isDown = false;
		if (g_isUp || g_isDown && g_isRight || g_isLeft) {
			m_bMove = false;
			Player->getCharactorBody()->SetGravityScale(-0.0355f);
			Player->getCharactorBody()->SetLinearVelocity(b2Vec2(0, 0));
			Sprite* sp = static_cast<Sprite*>(Player->getCharactorBody()->GetUserData());
			sp->stopAllActions();
			if (!g_isThrow) {
				g_isUpBroken = false;
				g_isUp = false;
				g_isDown = false;
				g_isLadderStop = true;
			}
			return true;
		}
		else {
			Player->getCharactorBody()->SetType(b2_dynamicBody);
			Player->getCharactorBody()->SetGravityScale(1.0f);
			return false;
		}
	}
}

void HelloWorld::AttackMonster(float dt) {
	std::list<Monster>::iterator iter;
	for (iter = Monsters.begin(); iter != Monsters.end(); ++iter) {
		if (iter->state == MS_DEATH) break; // 몬스터가 죽은 후 바로 사라지는게 아니라 애니메이션 출력 후 죽은 상태라면 입력받는 Attack을 무시
		Sprite* sp = static_cast<Sprite*>(Player->getCharactorBody()->GetUserData());
		Sprite* MonsterSp = static_cast<Sprite*>(iter->body->GetUserData());
		Rect rect = sp->getBoundingBox();

		b2Vec2 distance = iter->body->GetPosition() - m_Charactor_body->GetPosition();

		if (rect.intersectsRect(MonsterSp->getBoundingBox()) && 
			distance.x > -1 && distance.x < 1 && distance.y > -1 && distance.y < 1 &&
			(iter->shooting_Delay -= dt) <= 0) {
			CSoundManager::getInstance()->playEffect(SEF_MONSTER_HIT_BY_FEET_PATH);
			iter->shooting_Delay = 0.05f;
			int hp = iter->body->GetHp();
			hp = hp - (1 + g_CharactorData.status.f_power);
			iter->body->SetHp(hp);
			m_Charactor_body->ApplyLinearImpulse(b2Vec2(3, 7), m_Charactor_body->GetWorldCenter(), true);

			if (hp <= 0) {
				iter->state = MS_DEATH;
				m_fdt = 0;
				iter->time = 0.7f;
			}
			else {
				iter->state = MS_PAIN;
				iter->dt = 0;
				iter->time = 0.1;
			}
		}
		/*
		else if (distance.x > -0.5 && distance.x < 0.5 && distance.y > -0.5 && distance.y < 0.5 && (iter->shooting_Delay -= dt) <= 0) {
			iter->shooting_Delay = 0.5f;
			int hp = iter->body->GetHp();
			hp--;
			iter->body->SetHp(hp);
			iter->recognize_body->SetHp(hp);
			//	if (hp <= 0) hp = 0;

			if (hp <= 0) {
				iter->state = MS_DEATH;
				m_fdt = 0;
				iter->time = 0.7f;
			}
			else {
				iter->state = MS_PAIN;
				iter->dt = 0;
				iter->time = 0.5;
			}

			m_Monster->SpriteChange(iter, wlayer);
		}*/

		if (m_bMonster_swordshot && (iter->shooting_Delay -= dt) <= 0) {
			iter->shooting_Delay = 0.1f;
			if (g_bViewing &&
				distance.x > 0 && distance.x < 3 &&
				distance.y > 0 && distance.y < 3) {
				int hp = iter->body->GetHp();
				hp = hp - (1 + g_CharactorData.status.f_power);
				iter->body->SetHp(hp);
				iter->recognize_body->SetHp(hp);
				//	if (hp <= 0) hp = 0;

				if (hp <= 0) {
					iter->state = MS_DEATH;
					m_fdt = 0;
					iter->time = 0.7f;
				}
				else {
					iter->state = MS_PAIN;
					iter->dt = 0;
					iter->time = 0.5;
				}
			}
			else if (!g_bViewing &&
				distance.x > -3 && distance.x < 0 &&
				distance.y > 0 && distance.y < 2) {
				int hp = iter->body->GetHp();
				hp = hp - (1 + g_CharactorData.status.f_power);
				iter->body->SetHp(hp);
				iter->recognize_body->SetHp(hp);
				//	if (hp <= 0) hp = 0;

				if (hp <= 0) {
					iter->state = MS_DEATH;
					m_fdt = 0;
					iter->time = 0.7f;
				}
				else {
					iter->state = MS_PAIN;
					iter->dt = 0;
					iter->time = 0.5;
				}
			}
		}

		if (iter->before_hp > iter->body->GetHp()) {
			log("데미지 받음!");
			iter->state = MS_PAIN;
			if (iter->body->GetHp() <= 0)
				iter->state = MS_DEATH;
			iter->before_hp = iter->body->GetHp();
			if (m_bMonster_swordshot) {
				m_bMonster_swordshot = false;
				CSoundManager::getInstance()->playEffect(SEF_Sword_Slice_PATH);
				std::string sPath = "Images/effects/slice_effect.png";
				//GunEffect->CreateEffect(sPath, 5, Size(32, 32), Vec2(iter->body->GetPosition().x, iter->body->GetPosition().y), 1, wlayer);
				Sprite* sp = GunEffect->CreateEffect(sPath, 5, Size(32, 32), Vec2(iter->body->GetPosition().x * PTMRATIO, iter->body->GetPosition().y * PTMRATIO), 0.5, wlayer);
			}
			if (iter->obj_name == "Skel") {
				iter->body->SetLinearVelocity(b2Vec2(0, 0));
				m_Monster->SpriteChange(iter, wlayer);
			}
			else if (iter->obj_name == "Opossum") {
				iter->body->SetLinearVelocity(b2Vec2(0, 0));
				m_Opossum->SpriteChange(iter, wlayer);
			}
			else if (iter->obj_name == "Frog") {
				iter->body->SetLinearVelocity(b2Vec2(0, 0));
				m_Frog->SpriteChange(iter, wlayer);
			}
			else if (iter->obj_name == "Bat") {
				iter->body->SetLinearVelocity(b2Vec2(0, 0));
				m_Bat->SpriteChange(iter, wlayer);
			}
		}
	}
}


void HelloWorld::AttackBlock(b2Body* block) {
	//Sprite* SpriteData = static_cast<Sprite*>(block->GetUserData());
				
	if (m_bTakePickaxe) {
		int nBlockHp = block->GetHp();
		nBlockHp -= PICKAXEDAMAGE;
		block->SetHp(nBlockHp);
		INT16 category = block->GetFixtureList()->GetFilterData().categoryBits;
		int Durability = Player->getCharactor_Hand()->getDurability();
		
		if (category == BLOCK_CATEGORY) {
			if (nBlockHp >= 18) {
				m_brokenbl = EffectSprite::create("Images/brokenblock1.png");
				m_brokenbl->setAnchorPoint(Vec2(0, 0));
				m_brokenbl->setEffect(Map_Body.getLightEffect(), "Images/brokenblock1_n.png");
				wlayer->removeChild((EffectSprite*)block->GetUserData(), true);	// 먼저 Tilemap Layer에 있는 Sprite를 지워주고
				wlayer->addChild(m_brokenbl, 3);									// 새로 그자리에 배경레이어위로 Sprite를 올려준다.
				block->SetUserData((EffectSprite*)m_brokenbl);
				Player->getCharactor_Hand()->setDurability(--Durability);			// 블럭을 부술때마다 내구도 감소
			}
			else if (nBlockHp == 9) {
				m_brokenbl = EffectSprite::create("Images/brokenblock2.png");
				m_brokenbl->setAnchorPoint(Vec2(0, 0));
				m_brokenbl->setEffect(Map_Body.getLightEffect(), "Images/brokenblock2_n.png");
				wlayer->removeChild((EffectSprite*)block->GetUserData(), true);				
				wlayer->addChild(m_brokenbl, 3);									
				block->SetUserData((EffectSprite*)m_brokenbl);
				Player->getCharactor_Hand()->setDurability(--Durability);			
			}
			else if (nBlockHp == 0) {
				wlayer->removeChild((EffectSprite*)block->GetUserData(), true);				
				_world->DestroyBody(block);												
				Player->getCharactor_Hand()->setDurability(--Durability);		
			}
		}
		else if (category == GBLOCK_CATEGORY) {
			if (nBlockHp >= 18) {
				m_brokenbl = EffectSprite::create("n_Images/block/broken_gold_block_1.png");
				m_brokenbl->setAnchorPoint(Vec2(0, 0));
				m_brokenbl->setEffect(Map_Body.getLightEffect(), "n_Images/block/broken_gold_block_1_n.png");
				wlayer->removeChild((EffectSprite*)block->GetUserData(), true);	
				wlayer->addChild(m_brokenbl, 3);									
				block->SetUserData((EffectSprite*)m_brokenbl);
				Player->getCharactor_Hand()->setDurability(--Durability);			
			}
			else if (nBlockHp == 9) {
				m_brokenbl = EffectSprite::create("n_Images/block/broken_gold_block_2.png");
				m_brokenbl->setAnchorPoint(Vec2(0, 0));
				m_brokenbl->setEffect(Map_Body.getLightEffect(), "n_Images/block/broken_gold_block_2_n.png");
				wlayer->removeChild((EffectSprite*)block->GetUserData(), true);				
				wlayer->addChild(m_brokenbl, 3);									
				block->SetUserData((EffectSprite*)m_brokenbl);
				Player->getCharactor_Hand()->setDurability(--Durability);			
			}
			else if (nBlockHp == 0) { // gold block을 부쉇을때에는 하나의 gold가 드랍된다.
				b2Vec2 gold_pos = block->GetPosition();
				wlayer->removeChild((EffectSprite*)block->GetUserData(), true);				
				_world->DestroyBody(block);													
				Player->getCharactor_Hand()->setDurability(--Durability);		

				b2Body* Gold = Map_Body.CreateObj("Gold", gold_pos, _world, wlayer);
				Gold->ApplyLinearImpulse(b2Vec2(0, 7), Gold->GetWorldCenter(), true);
			}
		}
	}
}

void HelloWorld::AttackBlock(b2Body* block, bool sprite) {
	if (m_bTakePickaxe) {
		int nBlockHp = block->GetHp();
		nBlockHp -= PICKAXEDAMAGE;
		block->SetHp(nBlockHp);
		INT16 category = block->GetFixtureList()->GetFilterData().categoryBits;
		int Durability = Player->getCharactor_Hand()->getDurability();

		if (category == BLOCK_CATEGORY) {
			if (nBlockHp >= 18) {
				m_brokenbl_Sp = Sprite::create("Images/brokenblock1.png");
				m_brokenbl_Sp->setAnchorPoint(Vec2(0, 0));
				wlayer->removeChild((Sprite*)block->GetUserData(), true);
				wlayer->addChild(m_brokenbl_Sp, 3);									
				block->SetUserData((Sprite*)m_brokenbl_Sp);
				Player->getCharactor_Hand()->setDurability(--Durability);			
			}
			else if (nBlockHp == 9) {
				m_brokenbl_Sp = Sprite::create("Images/brokenblock2.png");
				m_brokenbl_Sp->setAnchorPoint(Vec2(0, 0));
				wlayer->removeChild((Sprite*)block->GetUserData(), true);
				wlayer->addChild(m_brokenbl_Sp, 3);
				block->SetUserData((Sprite*)m_brokenbl_Sp);
				Player->getCharactor_Hand()->setDurability(--Durability);
			}
			else if (nBlockHp == 0) {
				wlayer->removeChild((Sprite*)block->GetUserData(), true);
				_world->DestroyBody(block);
				Player->getCharactor_Hand()->setDurability(--Durability);
			}
		}
		else if (category == GBLOCK_CATEGORY) {
			if (nBlockHp >= 18) {
				m_brokenbl_Sp = Sprite::create("n_Images/block/broken_gold_block_1.png");
				m_brokenbl_Sp->setAnchorPoint(Vec2(0, 0));
				wlayer->removeChild((Sprite*)block->GetUserData(), true);
				wlayer->addChild(m_brokenbl_Sp, 3);
				block->SetUserData((Sprite*)m_brokenbl_Sp);
				Player->getCharactor_Hand()->setDurability(--Durability);
			}
			else if (nBlockHp == 9) {
				m_brokenbl_Sp = Sprite::create("n_Images/block/broken_gold_block_2.png");
				m_brokenbl_Sp->setAnchorPoint(Vec2(0, 0));
				wlayer->removeChild((Sprite*)block->GetUserData(), true);
				wlayer->addChild(m_brokenbl_Sp, 3);
				block->SetUserData((Sprite*)m_brokenbl_Sp);
				Player->getCharactor_Hand()->setDurability(--Durability);
			}
			else if (nBlockHp == 0) { // gold block을 부쉇을때에는 하나의 gold가 드랍된다.
				b2Vec2 gold_pos = block->GetPosition();
				wlayer->removeChild((Sprite*)block->GetUserData(), true);
				_world->DestroyBody(block);
				Player->getCharactor_Hand()->setDurability(--Durability);

				b2Body* Gold = Map_Body.CreateObj("Gold", gold_pos, _world, wlayer);
				Gold->ApplyLinearImpulse(b2Vec2(0, 7), Gold->GetWorldCenter(), true);
			}
		}
	}
}

void HelloWorld::PickupItem(std::list<b2Body*> *_item) {
	std::list<b2Body*>::iterator iter;
	if (_item->size() <= 0) return;

	b2Body* b = _item->front();
	if (b->GetHp() < 0) {
		return;
	}
	b2Fixture* fixture = b->GetFixtureList();

	uint16 nCategory = fixture->GetFilterData().categoryBits;
	EffectSprite* Charactor = (EffectSprite*)Player->getCharactorBody()->GetUserData();

	for (iter = _item->begin(); iter != _item->end(); ++iter) {
		b2Body* item = *iter;
		EffectSprite* item_sp = static_cast<EffectSprite*>(item->GetUserData());
		
		if (item_sp == NULL) {
			log("There are no items nearby");
			break;
		}

		Rect rect = Charactor->getBoundingBox();
		if (rect.intersectsRect(item_sp->getBoundingBox())) {

			if (nCategory == 0x0070) {
				CSoundManager::getInstance()->playEffect(SEF_Gold_Pickup_PATH);
				g_CharactorData.n_Gold_Count += 100;

				m_GameUi->GoldManager(m_clayer, g_CharactorData.n_Gold_Count);
				
				wlayer->removeChild(item_sp, true);
				_world->DestroyBody(item);								// 또한 해당 Body도 같이 지워줌

				Map_Body.lb_Goldlist.erase(iter);

				break;												// 한번에 하나의 물약만 처리하므로 break;
			}
			else if (nCategory == 0x0700) {
				if (m_ItemShop->getShopEnable()) {
					m_ItemShop->ShopDisplay(true, g_CharactorData.n_Gold_Count, Map_Body.getShopBody());
				}
				Player->setMoveState(MS_STOP);
				break;
			}

			else if (nCategory == 0x1000) {
				CSoundManager::getInstance()->playEffect(SEF_Potion_Drink_PATH);
				g_CharactorData.n_Hp_Count++;
				m_clayer->removeChild(m_HeartLabel, true);
				
				m_GameUi->Heartmanager(&g_CharactorData);

				wlayer->removeChild(item_sp, true);
				_world->DestroyBody(item);								// 또한 해당 Body도 같이 지워줌

				Map_Body.lb_Heartlist.erase(iter);

				break;												// 한번에 하나의 물약만 처리하므로 break;
			}

			else if (nCategory == 0x2000) {
				CSoundManager::getInstance()->playEffect(SEF_Item_Pickup_PATH);
				m_bTakeSword = false;
				m_bTakeGun = false;
				m_bTakePickaxe = true;
				g_CharactorData.item = IS_PICKAXE;
				std::string pickaxe = "Pickaxe";
				if (item->GetNewobj()) {
					wlayer->removeChild(item_sp, true);
				}
				else wlayer->removeChild(item_sp, true);		// 초기 생성되는 Item은 itemLayer에 Node로 등록되어 있지만
																			// 캐릭터가 획득 후 버려진 Item은 wlayer에 Node로 등록되어있다.
				_world->DestroyBody(item);
				Map_Body.lb_Pickaxelist.erase(iter);

				Player->Pickup(&pickaxe, Player->getMoveState(), wlayer); // 아이템 이름을 넘김
				break;
			}

			else if (nCategory == 0x3000) {
				CSoundManager::getInstance()->playEffect(SEF_Item_Pickup_PATH);
				g_CharactorData.n_Bomb_Count++;
				if (item->GetNewobj()) {
					wlayer->removeChild(item_sp, true);
				}
				else wlayer->removeChild(item_sp, true);

				m_GameUi->Bombmanager(g_CharactorData.n_Bomb_Count);

				_world->DestroyBody(item);
				Map_Body.lb_Bomblist.erase(iter);

				break;
			}
			else if (nCategory == 0x4000) {
				CSoundManager::getInstance()->playEffect(SEF_Item_Pickup_PATH);
				m_bTakePickaxe = false;
				m_bTakeGun = false;
				m_bTakeSword = true;
				g_CharactorData.item = IS_SWORD;
				std::string sword = "Sword";
				if (item->GetNewobj()) {
					wlayer->removeChild(item_sp, true);
				}
				else wlayer->removeChild(item_sp, true);

				_world->DestroyBody(item);
				Map_Body.lb_Swordlist.erase(iter);

				Player->Pickup(&sword, Player->getMoveState(), wlayer);
				break;
			}

			else if (nCategory == 0x5000) {
				CSoundManager::getInstance()->playEffect(SEF_Gun_Pickup_PATH);
				m_bTakePickaxe = false;
				m_bTakeSword = false;
				m_bTakeGun = true;
				g_CharactorData.item = IS_GUN;
				std::string gun = "Gun";
				if (item->GetNewobj()) {
					wlayer->removeChild(item_sp, true);
				}
				else wlayer->removeChild(item_sp, true);

				_world->DestroyBody(item);
				Map_Body.lb_Gunlist.erase(iter);

				Player->Pickup(&gun, Player->getMoveState(), wlayer);
				break;
			}
			else if (nCategory == LADDER_CATEGORY) {
				CSoundManager::getInstance()->playEffect(SEF_Item_Pickup_PATH);
				g_CharactorData.n_Ladder_Count++;
				if (item->GetNewobj()) {
					wlayer->removeChild(item_sp, true);
				}
				else wlayer->removeChild(item_sp, true);

				m_GameUi->Laddermanager(g_CharactorData.n_Ladder_Count);
				_world->DestroyBody(item);
				Map_Body.lb_Ladderlist.erase(iter);
				break;
			}
			else if (nCategory == LLADDER_CATEGORY) {
				break;
			}
			else if (nCategory == REDKEY_CATEGORY) {
				CSoundManager::getInstance()->playEffect(SEF_Item_Pickup_PATH);
				
				g_CharactorData.key = REDKEY;
				wlayer->removeChild(item_sp, true);

				m_GameUi->getRedKeySprite()->setOpacity(255);
				_world->DestroyBody(item);
				Map_Body.lb_RedKeylist.erase(iter);
				break;
			}
			else if (nCategory == BLUEKEY_CATEGORY) {
				CSoundManager::getInstance()->playEffect(SEF_Item_Pickup_PATH);
				
				g_CharactorData.key = BLUEKEY;
				wlayer->removeChild(item_sp, true);

				m_GameUi->getBlueKeySprite()->setOpacity(255);
				_world->DestroyBody(item);
				Map_Body.lb_BlueKeylist.erase(iter);
				break;
			}
			else if (nCategory == GREENKEY_CATEGORY) {
				CSoundManager::getInstance()->playEffect(SEF_Item_Pickup_PATH);
				
				g_CharactorData.key = GREENKEY;
				wlayer->removeChild(item_sp, true);

				m_GameUi->getGreenKeySprite()->setOpacity(255);
				_world->DestroyBody(item);
				Map_Body.lb_GreenKeylist.erase(iter);
				break;
			}
		}
	}
}


void HelloWorld::OpenButtonEnable(Rect* _rect, Sprite* _Sprite) {
	if (_rect->intersectsRect(_Sprite->getBoundingBox())) {
		m_pOpenButton->setVisible(true);
		m_pOpenButton->setEnabled(true);
	}
	else {
		m_pOpenButton->setVisible(false);
		m_pOpenButton->setEnabled(false);
	}
}

bool HelloWorld::createBox2dWorld(bool debug) {

	HelloWorld::CreateMap();
	HelloWorld::CreateCharactor();	
	HelloWorld::CreateMonster();
	HelloWorld::CreateUI();
	log("%d", g_CharactorData.StageNumber);
	HelloWorld::initSounds();

	/*
	Sprite* sp = Sprite::create("Images/obstacle/lava_2.png");
	sp->setPosition(Vec2(0, 0));

	sp->setAnchorPoint(Vec2(0, 0));
	wlayer->addChild(sp, 4);*/
	//log("Jump %f", gNumFootContacts);

	// 디버그 드로잉 설정
	if (debug) {
		// 적색 : 현재 물리운동을 하는 것.
		// 회색 : 현재 물리 운동량이 없는것.
		m_debugDraw = new GLESDebugDraw(PTMRATIO);
		_world->SetDebugDraw(m_debugDraw);

		uint32 flags = 0;
		//flags += b2Draw::e_shapeBit;
		//flags += b2Draw::e_aabbBit;
		//flags += b2Draw::e_jointBit;
		//flags += b2Draw::e_pairBit;
		//flags += b2Draw::e_centerOfMassBit;
		m_debugDraw->SetFlags(flags);
	}

	return true;
}

void HelloWorld::CreateUI() {
	m_GameUi = new GameUI(_world, m_clayer, Player->getCharactorBody(), &g_CharactorData);
//	if (g_CharactorData.StageNumber != SHELTER) {
//		m_GameUi->CreateStageMessage(gStageNumber);
//	}
//	else {
		m_GameUi->CreateStageMessage(g_CharactorData.StageNumber);
//	}

	HelloWorld::CreateBt();
	m_ItemShop = new ItemShop(_world, m_clayer, wlayer);
	m_ItemShop->CreateShop(&Map_Body, m_GameUi);
	m_GameUi->CharactorStateSprite();
	m_GameUi->CreateGoldUi(g_CharactorData.n_Gold_Count);
	m_GameUi->CreateKeyUi(g_CharactorData.key);
	m_GameUi->CreatePauseButton();
}

void HelloWorld::CreateBt() {
	m_pAttackButton_Images = MenuItemImage::create("Images/ui/Attack_button.png", "Images/ui/Attack_button_selected.png",
		CC_CALLBACK_1(HelloWorld::doAction, this));

	m_pAttackButton = Menu::create(m_pAttackButton_Images, nullptr);
	m_pos = m_clayer->getContentSize();
	m_pAttackButton->setPosition(Vec2(m_pos.x - 60, m_pos.y - 290));
	m_clayer->addChild(m_pAttackButton, 4);
	
	m_pLadderButton_Images = MenuItemImage::create("Images/ui/Ladderbutton.png", "Images/ui/Ladderbutton_selected.png",
		CC_CALLBACK_1(HelloWorld::doLadderequip, this));
	m_pLadderButton = Menu::create(m_pLadderButton_Images, nullptr);
	m_pos = m_clayer->getContentSize();
	m_pLadderButton->setPosition(Vec2(m_pos.x - 120, m_pos.y - 290));
	m_clayer->addChild(m_pLadderButton, 4);

	m_pBombButton_Images = MenuItemImage::create("Images/ui/Bomb_button.png", "Images/ui/Bomb_button_selected.png",
		CC_CALLBACK_1(HelloWorld::doThrowBomb, this));
	m_pBombButton = Menu::create(m_pBombButton_Images, nullptr);
	m_pBombButton->setPosition(Vec2(m_pos.x - 60, m_pos.y - 250));
	m_clayer->addChild(m_pBombButton, 4);

	m_pPickupButton_Images = MenuItemImage::create("Images/ui/Pickup_button.png", "Images/ui/Pickup_button_selected.png",
		CC_CALLBACK_1(HelloWorld::doPickup, this));
	m_pPickupButton = Menu::create(m_pPickupButton_Images, nullptr);
	m_pPickupButton->setPosition(Vec2(m_pos.x - 120, m_pos.y - 250));
	m_clayer->addChild(m_pPickupButton, 4);

	m_pOpenButtonImage = MenuItemImage::create("Images/ui/OpenButton.png", "Images/ui/OpenButton_selected.png", CC_CALLBACK_1(HelloWorld::doOpen, this));
	m_pOpenButton = Menu::create(m_pOpenButtonImage, nullptr);
	m_pOpenButton->setPosition(Vec2(240, 120));
	m_pOpenButton->setEnabled(false);
	m_pOpenButton->setVisible(false);
	m_clayer->addChild(m_pOpenButton, 5);

	m_pPortalButtonImage = MenuItemImage::create("Images/ui/Portal_button.png", "Images/ui/Portal_button_selected.png", CC_CALLBACK_1(HelloWorld::doNext, this));
	m_pPortalButton = Menu::create(m_pPortalButtonImage, nullptr);
	m_pPortalButton->setPosition(Vec2(240, 120));
	m_pPortalButton->setEnabled(false);
	m_pPortalButton->setVisible(false);
	m_clayer->addChild(m_pPortalButton, 5);
}

void HelloWorld::CreateMap() {

	b2Vec2 gravity;
	gravity.Set(0.0f, -30.0f);
	_world = new b2World(gravity);

	_world->SetAllowSleeping(true);
	_world->SetContinuousPhysics(true);

	auto debugLayer = B2DebugDrawLayer::create(_world, PTMRATIO);
	this->addChild(debugLayer, 9999);

	int num;

	std::string TMXPath;
	Sprite* BackGroundSp;
	EffectSprite* BackGroundESp;

	// 같은 맵이 연속으로 출현할 수 없도록
	for (; true ;) {
		switch (g_CharactorData.StageNumber) {
		case STAGE_1_1:
		case STAGE_1_2:
		case STAGE_1_3:
			
		
			num = rand() % 17 + 1;
			break;
		case STAGE_2_1:
		case STAGE_2_2:
		case STAGE_2_3:
			
			
			num = rand() % (16 - 9 + 1) + 9;
			break;
		case STAGE_3_1:
		case STAGE_3_2:
		case STAGE_3_3:
			num = rand() % (24 - 17 + 1) + 17;
			break;

		default:
			num = 0;
			break;
		}
		if (g_CharactorData.before_stage != 0 && g_CharactorData.before_stage != num) {
			g_CharactorData.before_stage = num;
			break;
		}
		else if(g_CharactorData.before_stage == 0) {
			g_CharactorData.before_stage = num;
			break;
		}
	}
	//g_CharactorData.StageNumber = STAGE_3_1;
	g_CharactorData.StageNumber = BOSS;
	if (g_CharactorData.StageNumber == SHELTER) {
		TMXPath = "TileMaps/Shelter_Ground.tmx";
	}
	else {
		TMXPath = "TileMaps/BaseGround_Boss.tmx";
		//char str[100];
		//std::string sNum = std::to_string(num);
		//sprintf(str, "TileMaps/BaseGround%d.tmx", 17);
		//TMXPath = str;
	}

	map = TMXTiledMap::create(TMXPath);
	map->setAnchorPoint(Point::ZERO);
	map->setPosition(0, 0);

	m_Explosion = new Explosion;

	//First_Floor.CreateCave(map, _world);
	if (TMXPath[21] != 'l') {
		g_CharactorData.bStage_blind = false;
		Map_Body.initCollisionMap(map, _world, g_CharactorData.StageNumber, g_CharactorData.bStage_blind);
	}
	else {
		g_CharactorData.bStage_blind = true;
		Map_Body.initCollisionMap(map, _world, g_CharactorData.StageNumber, g_CharactorData.bStage_blind);
	}
	

	if (g_CharactorData.StageNumber == SHELTER) {
		m_PlayerLight = Map_Body.SpriteChange(wlayer, _world); // EffectSprite
		BackGroundESp = EffectSprite::create("Images/Background/Game_Loading_Background_.png");
		BackGroundESp->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		BackGroundESp->setPosition(Vec2::ZERO);
		BackGroundESp->setEffect(m_PlayerLight, "Images/Background/Game_Background_n.png");
		BackGroundESp->setColor(Color3B(255, 255, 255));
		wlayer->addChild(BackGroundESp);

		Texture2D* texture = Director::getInstance()->getTextureCache()->addImage("Images/effects/portal.png");
		Animation* animation = Animation::create();
		animation->setDelayPerUnit(0.1f);

		for (int i = 0; i < 9; i++) {
			animation->addSpriteFrameWithTexture(texture, Rect(i * 170, 0, 170, 320));
		}
		m_PortalSp = Sprite::createWithTexture(texture, Rect(0, 0, 170, 320));
		m_PortalSp->setPosition(Vec2(1600, 70));
		m_PortalSp->setScale(0.5f);
		wlayer->addChild(m_PortalSp, 2);

		Animate* animate = Animate::create(animation);
		RepeatForever* rep = RepeatForever::create(animate);

		m_PortalSp->runAction(rep);
	}
	else if (g_CharactorData.StageNumber != SHELTER && g_CharactorData.StageNumber != BOSS && !g_CharactorData.bStage_blind) {
		Map_Body.UserDataChange(wlayer, _world); // Sprite

		if (g_CharactorData.StageNumber >= STAGE_1_1 && g_CharactorData.StageNumber <= STAGE_1_3) {
			BackGroundSp = Sprite::create("Images/Background/Normal_Background.png");
			BackGroundSp->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
			BackGroundSp->setPosition(Vec2::ZERO);
			//BackGroundSp->setVisible(false);
			wlayer->addChild(BackGroundSp);
		}
		else if (g_CharactorData.StageNumber >= STAGE_2_1 && g_CharactorData.StageNumber <= STAGE_2_3) {
			BackGroundSp = Sprite::create("Images/Background/Ice_Background.png");
			BackGroundSp->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
			BackGroundSp->setPosition(Vec2::ZERO);
			wlayer->addChild(BackGroundSp);
		}
		else if (g_CharactorData.StageNumber >= STAGE_3_1 && g_CharactorData.StageNumber <= STAGE_3_3) {
			BackGroundSp = Sprite::create("Images/Background/Lava_Background2.png");
			BackGroundSp->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
			BackGroundSp->setPosition(Vec2::ZERO);
			wlayer->addChild(BackGroundSp);
		}

		m_Obstacle = new Obstacle(_world, wlayer);
		m_Obstacle->CreateObstacle(&g_CharactorData);

	}
	else if (g_CharactorData.bStage_blind) {
		m_PlayerLight = Map_Body.SpriteChange(wlayer, _world); // EffectSprite
		if (g_CharactorData.StageNumber >= STAGE_1_1 && g_CharactorData.StageNumber <= STAGE_1_3) {
			// Background Sprite
			BackGroundESp = EffectSprite::create("Images/Background/background_game.png");
			BackGroundESp->setEffect(m_PlayerLight, "Images/Background/background_game_n.png");
			BackGroundESp->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
			BackGroundESp->setPosition(Vec2::ZERO);
			BackGroundESp->setColor(Color3B(255, 255, 255));
			wlayer->addChild(BackGroundESp);

			// 설계된 Game Map Sprite
			EffectSprite* BaseGroundSp = EffectSprite::create("Images/Background/BaseGround_Blind.png");
			BaseGroundSp->setEffect(m_PlayerLight, "Images/Background/BaseGround_Blind_n.png");
			BaseGroundSp->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
			BaseGroundSp->setPosition(Vec2::ZERO);
			BaseGroundSp->setColor(Color3B(255, 255, 255));
			wlayer->addChild(BaseGroundSp);

			m_Obstacle = new Obstacle(_world, wlayer, m_PlayerLight);
			m_Obstacle->CreateObstacle(&g_CharactorData);
		}
		else if (g_CharactorData.StageNumber >= STAGE_2_1 && g_CharactorData.StageNumber <= STAGE_2_3) {
			BackGroundESp = EffectSprite::create("Images/Background/Ice_Background.png");
			BackGroundESp->setEffect(m_PlayerLight, "Images/Background/Ice_Background_n.png");
			BackGroundESp->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
			BackGroundESp->setPosition(Vec2::ZERO);
			BackGroundESp->setColor(Color3B(255, 255, 255));
			wlayer->addChild(BackGroundESp);
		}
	}
	else if (g_CharactorData.StageNumber == BOSS) {
		Map_Body.UserDataChange(wlayer, _world);

		m_Boss = new BossMonster;
		m_Boss->CreateBossMap(wlayer, _world, &g_CharactorData);

		m_Obstacle = new Obstacle(_world, wlayer, m_PlayerLight);
		m_Obstacle->CreateObstacle(&g_CharactorData);
	}

	

	//	m_Lava->CreateLava();

		//this->setScale(2.0f);
		//wlayer->setScale(2.0f);
		//m_clayer->setScale(0.7f);

	wlayer->addChild(map);
	//wlayer->setVisible(false);
	//m_clayer->setVisible(false);
}

void HelloWorld::CreateCharactor() {
	TMXObjectGroup* objects = map->getObjectGroup("SpawnPoint");
	ValueMap spawnPoint = objects->getObject("Sp");
	
	m_HeartLabel = new Label;

	float x = spawnPoint["x"].asFloat();
	float y = spawnPoint["y"].asFloat();

	g_CharactorData.Spawn_Point = b2Vec2(x / PTMRATIO, y / PTMRATIO);

	Player = new CharactorCreator(wlayer, m_clayer);
	Player->CreateCharactor(_world, g_CharactorData, wlayer, m_PlayerLight); // Charactor Sprite Charactor Layer
	m_beforeMstate = MS_STOP;
	m_EPlayerSprite = static_cast<EffectSprite*>(Player->getCharactorBody()->GetUserData());


	m_PlayerSprite = Player->getPlayer();
	m_Charactor_body = Player->getCharactorBody();
	m_CameraSprite = (EffectSprite*)Player->getCameraBody()->GetUserData();
	wlayer->addChild(m_CameraSprite, 1); // Camera Sprite Charactor Layer

	//m_BrokenAABB.lowerBound = Player->getCharactorBody()->GetPosition() - b2Vec2(1, 1);
	//m_BrokenAABB.upperBound = Player->getCharactorBody()->GetPosition() + b2Vec2(1, 1);

	auto texture = Director::getInstance()->getTextureCache()->addImage("Images/Recognize.png");
	m_RecognizeSp = EffectSprite::create("Images/Recognize.png");
	m_RecognizeSp->setVisible(false);
	m_RecognizeSp->setScale(1.5);
	wlayer->addChild(m_RecognizeSp, 3);

	b2WeldJointDef weldJointDef;
	weldJointDef.Initialize(Player->getCharactorBody(), Player->getCameraBody(), Player->getCharactorBody()->GetPosition());
	_world->CreateJoint(&weldJointDef);

	std::string item;
	switch (g_CharactorData.item) {
	case IS_NONE:
		break;
	case IS_PICKAXE:
		item = "Pickaxe";
		Player->Pickup(&item, Player->getMoveState(), wlayer);
		m_bTakePickaxe = true;
		m_bTakeSword = false;
		m_bTakeGun = false;
		break;
	case IS_SWORD:
		item = "Sword";
		Player->Pickup(&item, Player->getMoveState(), wlayer);
		m_bTakePickaxe = false;
		m_bTakeSword = true;
		m_bTakeGun = false;
		break;
	case IS_GUN:
		item = "Gun";
		Player->Pickup(&item, Player->getMoveState(), wlayer);
		m_bTakePickaxe = false;
		m_bTakeSword = false;
		m_bTakeGun = true;
		break;
	}

	m_BuffManager = new BuffManager(wlayer, m_clayer, winSize);
	m_BuffManager->CreateBuffSlot();

	if (g_CharactorData.buff.size() != 0) {
		m_BuffManager->CharactorInitBuff(&g_CharactorData);
	}
	

	Rect myBoundary = Rect(0, 0, 1920, 1024);

	wlayer->runAction(Follow::create(m_CameraSprite, myBoundary));  // Camera Action Charactor Sprite
	//this->runAction(Follow::create(m_CameraSprite, myBoundary));
	//m_clayer->runAction(Follow::create(m_CameraSprite, myBoundary));

	_world->SetContactListener(&gContactInstance); // contacts recognize
	m_fImpulse = Player->getCharactorBody()->GetMass() * 7; // Jump Impulse

	Size size = m_CameraSprite->getContentSize();

	//m_clayer->addChild(Player->getBombSprite(), 3);
	//m_clayer->addChild(Player->getLadderSprite(), 3);

	//Player->Heartmanager(m_clayer, g_CharactorData.n_Hp_Count);

	//Player->Bombmanager(m_clayer, g_CharactorData.n_Bomb_Count);

	//Player->Laddermanager(m_clayer, g_CharactorData.n_Ladder_Count);

	GunEffect = new EffectAnimation(wlayer);
}


void HelloWorld::CreateMonster() {
	
	TMXObjectGroup* objects = map->getObjectGroup("M_SpawnPoint");
	MonsterOnWorld(objects);

	Monster Lastbody = Monsters.back();
	m_Monster_Component = new std::vector<Monster_Component>;
	for (b2Joint* joint = Lastbody.body->GetJointList()->joint; joint != nullptr; joint = joint->GetNext()) {
		// 이게 어떻게되있는건지 body가 서로 달라도 jointlist는 world의 모든body들의 joint가 연결되어있다 말도안돼
		// 따라서 맨 마지막 생성된 body의 jointlist를 가져와 모든 몬스터의 joint를 저장한 뒤 일정시간 동안 움직임이 없을 경우 DestoryQ에 올려주는식으로 world에서 제거
		// Monster 부속 body category bits 는 0x0030
		UINT16 category = joint->GetBodyA()->GetFixtureList()->GetFilterData().categoryBits;
		UINT16 category2 = joint->GetBodyB()->GetFixtureList()->GetFilterData().categoryBits;
		if (category == 0x0030)
			m_Monster_Component->push_back(Monster_Component(joint->GetBodyA(), 10.0f));
		if (category2 == 0x0030)
			m_Monster_Component->push_back(Monster_Component(joint->GetBodyB(), 10.0f));
	}
}

void HelloWorld::MonsterOnWorld(TMXObjectGroup* obj) {

	cocos2d::ValueVector monster_collision = obj->getObjects();
	m_Monster = new MonsterCreator(g_CharactorData, Map_Body.getLightEffect());
	m_Opossum = new Opossum(g_CharactorData, Map_Body.getLightEffect());
	m_Frog = new Frog(g_CharactorData, Map_Body.getLightEffect());
	m_Bat = new Bat(g_CharactorData, Map_Body.getLightEffect());
	//Monsters = new std::list<Monster>;
	// 몬스터즈 자체를 구조체로

	for (Value object : monster_collision) {
		ValueMap monster = object.asValueMap();
		std::string obj_name = monster["Name"].asString();
	
		if (obj_name == "Skel") {
			float x = monster["x"].asFloat();
			float y = monster["y"].asFloat();

			b2Body* Mob = m_Monster->CreateMonster(_world, Vec2(x, y), wlayer);
			b2Body* recognize = m_Monster->getRecognizeBody();
			Monsters.push_back(Monster(obj_name, Mob, recognize, MS_STOP, Mob->GetHp(), 0));
		}
	
		else if (obj_name == "Opossum") {
			float x = monster["x"].asFloat();
			float y = monster["y"].asFloat();

			b2Body* Mob = m_Opossum->CreateMonster(_world, Vec2(x, y), wlayer);
			b2Body* recognize = m_Opossum->getRecognizeBody();
			Monsters.push_back(Monster(obj_name, Mob, recognize, MS_STOP, Mob->GetHp(), 0));
		}
		else if (obj_name == "Frog") {
			float x = monster["x"].asFloat();
			float y = monster["y"].asFloat();

			b2Body* Mob = m_Frog->CreateMonster(_world, Vec2(x, y), wlayer);
			b2Body* recognize = m_Frog->getRecognizeBody();
			Monsters.push_back(Monster(obj_name, Mob, recognize, MS_STOP, Mob->GetHp(), 0));
		}
		else if (obj_name == "Bat") {
			float x = monster["x"].asFloat();
			float y = monster["y"].asFloat();

			b2Body* Mob = m_Bat->CreateMonster(_world, Vec2(x, y), wlayer);
			b2Body* recognize = m_Bat->getRecognizeBody();
			Monsters.push_back(Monster(obj_name, Mob, recognize, MS_STOP, Mob->GetHp(), 0));
		}
	}
}

void HelloWorld::initSounds() {
	CSoundManager::getInstance()->preloadAudioFile(BGM_Background_1_PATH);
	CSoundManager::getInstance()->preloadAudioFile(BGM_Charactor_Step_PATH);
	CSoundManager::getInstance()->preloadAudioFile(BGM_CAUTION_SIREN_PATH);
	experimental::AudioEngine::preload(BGM_Background_1_PATH);
	experimental::AudioEngine::preload(BGM_Charactor_Step_PATH);
	PlayBackgroundSound();
}

void HelloWorld::PlayBackgroundSound() {
	CSoundManager::getInstance()->playBgm(BGM_Background_1_PATH, true, false);
	
	m_fsoundVolume = 1.0f;
	if (experimental::AudioEngine::getState(m_nAudioId1) != experimental::AudioEngine::AudioState::PLAYING) {
		m_nAudioId1 = experimental::AudioEngine::play2d(BGM_Background_1_PATH, true, m_fsoundVolume);
		
	}
	experimental::AudioEngine::setLoop(m_nAudioId1, true);
	experimental::AudioEngine::setVolume(m_nAudioId1, m_fsoundVolume);
	experimental::AudioEngine::getState(m_nAudioId1);

}

void HelloWorld::PlaySoundEffect(int ID) {
	//CSoundManager::getInstance()->playEffect(SEF_Charactor_Step_PATH);
	
	if (experimental::AudioEngine::getState(m_nStepAudioId) != experimental::AudioEngine::AudioState::PLAYING) {
	//	m_nStepAudioId = experimental::AudioEngine::play2d(SEF_Charactor_Step_PATH, true, m_fsoundVolume);
	}
}

void HelloWorld::PlayCharactorStep() {
	//CSoundManager::getInstance()->playBgm(BGM_Charactor_Step_PATH, true, false);
}

void HelloWorld::doAction(Ref* pSender) {
	log("어택!");
	if (m_bTakePickaxe) {
		if (g_CharactorData.StageNumber == SHELTER || g_CharactorData.bStage_blind) {
			m_GameUi->MessageOutput("Pickaxes are not available on the map.");
			CSoundManager::getInstance()->playEffect(SEF_HIT_FAILED_PATH);
		}
		else {
			if (g_isAttackOn) {
				CSoundManager::getInstance()->playEffect(SEF_Block_Attack_PATH);
				b2Vec2 ch_pos = Player->getCharactorBody()->GetPosition();
				g_ATTACK = true;
				g_isAttackOn = false;
				g_isLeft = false;
				g_isRight = false;
				m_fAttackDelaytime = 0.5f;
				m_fAttackCooltime = 1.5f;
				if (g_isLeftBroken) {
					b2Vec2 block_pos = m_BrokenBlockBody->GetPosition();
					if ((ch_pos.x - 2 < block_pos.x && ch_pos.x + 1 > block_pos.x) &&
						(ch_pos.y - 2 < block_pos.y && ch_pos.y + 1 > block_pos.y)) {
						HelloWorld::KeyAction(MS_STOP, ATTACK, "Pickaxe"); // 애니메이션
						if (g_CharactorData.StageNumber == SHELTER || g_CharactorData.bStage_blind) {
							AttackBlock(m_BrokenBlockBody);
						}
						else {
							AttackBlock(m_BrokenBlockBody, true);
						}
					}
				}
				else if (g_isRightBroken) {
					b2Vec2 block_pos = m_BrokenBlockBody->GetPosition();
					if ((ch_pos.x - 2 < block_pos.x && ch_pos.x + 1 > block_pos.x) &&
						(ch_pos.y - 2 < block_pos.y && ch_pos.y + 1 > block_pos.y)) {
						HelloWorld::KeyAction(MS_STOP, ATTACK, "Pickaxe"); // 애니메이션
						if (g_CharactorData.StageNumber == SHELTER || g_CharactorData.bStage_blind) {
							AttackBlock(m_BrokenBlockBody);
						}
						else {
							AttackBlock(m_BrokenBlockBody, true);
						}
					}
				}
				else if (g_isCanUpBroken) {
					b2Vec2 block_pos = m_BrokenBlockBody->GetPosition();
					if ((ch_pos.x - 2 < block_pos.x && ch_pos.x + 1 > block_pos.x) &&
						(ch_pos.y - 2 < block_pos.y && ch_pos.y + 1 > block_pos.y)) {
						HelloWorld::KeyAction(MS_STOP, UP_ATTACK, "Pickaxe"); // 애니메이션
						if (g_CharactorData.StageNumber == SHELTER || g_CharactorData.bStage_blind) {
							AttackBlock(m_BrokenBlockBody);
						}
						else {
							AttackBlock(m_BrokenBlockBody, true);
						}
					}
				}
				else if (g_isCanDownBroken) {
					b2Vec2 block_pos = m_BrokenBlockBody->GetPosition();
					if ((ch_pos.x - 2 < block_pos.x && ch_pos.x + 1 > block_pos.x) &&
						(ch_pos.y - 2 < block_pos.y && ch_pos.y + 1 > block_pos.y)) {
						HelloWorld::KeyAction(MS_STOP, UNDER_ATTACK, "Pickaxe"); // 애니메이션
						if (g_CharactorData.StageNumber == SHELTER || g_CharactorData.bStage_blind) {
							AttackBlock(m_BrokenBlockBody);
						}
						else {
							AttackBlock(m_BrokenBlockBody, true);
						}
					}
				}
				else if (g_isCanRightUpBroken) {
					b2Vec2 block_pos = m_BrokenBlockBody->GetPosition();
					if ((ch_pos.x - 2 < block_pos.x && ch_pos.x + 1 > block_pos.x) &&
						(ch_pos.y - 2 < block_pos.y && ch_pos.y + 1 > block_pos.y)) {
						HelloWorld::KeyAction(MS_STOP, UP_ATTACK, "Pickaxe"); // 애니메이션
						if (g_CharactorData.StageNumber == SHELTER || g_CharactorData.bStage_blind) {
							AttackBlock(m_BrokenBlockBody);
						}
						else {
							AttackBlock(m_BrokenBlockBody, true);
						}
					}
				}
				else if (g_isCanLeftUpBroken) {
					b2Vec2 block_pos = m_BrokenBlockBody->GetPosition();
					if ((ch_pos.x - 2 < block_pos.x && ch_pos.x + 1 > block_pos.x) &&
						(ch_pos.y - 2 < block_pos.y && ch_pos.y + 1 > block_pos.y)) {
						HelloWorld::KeyAction(MS_STOP, UP_ATTACK, "Pickaxe"); // 애니메이션
						if (g_CharactorData.StageNumber == SHELTER || g_CharactorData.bStage_blind) {
							AttackBlock(m_BrokenBlockBody);
						}
						else {
							AttackBlock(m_BrokenBlockBody, true);
						}
					}
				}
			}
		}
	}
	if (m_bTakeSword) {
		if (g_isAttackOn && m_fAttackCooltime <= 0) {
			CSoundManager::getInstance()->playEffect(SEF_Item_Swing_PATH);
			g_ATTACK = true;
			g_isAttackOn = false;
			g_isLeft = false;
			g_isRight = false;
			m_bMonster_swordshot = true;
			m_fAttackDelaytime = 0.15f; // 공격 모션 시간
			m_fAttackCooltime = 1.0f; // 공격 후 쿨타임
			if (m_bTakePickaxe)
				HelloWorld::KeyAction(MS_STOP, ATTACK, "Pickaxe");
			else if (m_bTakeSword)
				HelloWorld::KeyAction(MS_STOP, ATTACK, "Sword");
			else if (m_bTakeGun)
				HelloWorld::KeyAction(MS_STOP, ATTACK, "Gun");
			else
				HelloWorld::KeyAction(MS_GET, ATTACK, "");
		}
	}
	else if (m_bTakeGun) {
		if (g_isAttackOn) {
			CSoundManager::getInstance()->playEffect(SEF_Gun_Attack_PATH);
			g_ATTACK = true;
			g_isAttackOn = false;
			g_isLeft = false;
			g_isRight = false;
			m_fAttackDelaytime = 0.5f;
			m_fAttackCooltime = 1.5f;
			HelloWorld::KeyAction(MS_STOP, ATTACK, "Gun");
			HelloWorld::BulletFire();
		}
	}
}

void HelloWorld::doLadderequip(Ref* pSender) {
	
	log("사다리 설치!");
	
	if (g_CharactorData.n_Ladder_Count > 0 && m_fLadderCooltime <= 0) {
		m_fLadderCooltime = 2;

		g_CharactorData.n_Ladder_Count--;
		m_GameUi->Laddermanager(g_CharactorData.n_Ladder_Count);

		if (g_bViewing) {
			Map_Body.CreateObj("Ladder", Player->getCharactorBody()->GetPosition() + b2Vec2(1, 0), _world, wlayer);
			Map_Body.getItemBody()->ApplyLinearImpulse(b2Vec2(m_fImpulse / 3, m_fImpulse / 3), Player->getCharactorBody()->GetWorldCenter(), true);
		}
		else if (!g_bViewing) {
			Map_Body.CreateObj("Ladder", Player->getCharactorBody()->GetPosition() + b2Vec2(-1, 0), _world, wlayer);
			Map_Body.getItemBody()->ApplyLinearImpulse(b2Vec2(-m_fImpulse / 3, m_fImpulse / 3), Player->getCharactorBody()->GetWorldCenter(), true);
		}

		m_EnableLadder.push_back(Map_Body.getItemBody());
		if (g_CharactorData.StageNumber == SHELTER || g_CharactorData.bStage_blind) {
			if (g_bViewing) m_PlayerSprite->ESpriteData->setFlippedX(false);
			else if (!g_bViewing) m_PlayerSprite->ESpriteData->setFlippedX(true);
		}
		else {
			if (g_bViewing) m_PlayerSprite->SpriteData->setFlippedX(false);
			else if (!g_bViewing) m_PlayerSprite->SpriteData->setFlippedX(true);
		}
		//log("Player position %f %f", sp->getPosition().x, sp->getPosition().y);
	}
}

void HelloWorld::doThrowBomb(Ref* pSender) {
	this->clearFloor();
	// Stage를 이동하면서 Scene을 바꾸기 전 한번 Frame을 거쳐 Scene 전환이 이루어지기 때문에 Scene 전환 중 이라면 Frame 진행을 아예 할 수 없도록 막기 위해.
	m_bChangeStage = true; 	
	/*
	g_isThrow = false;
	m_bCan_Move = true;
	
	if (g_CharactorData.n_Bomb_Count > 0 && g_isThrowOn) {
		g_isThrowOn = false;
		m_fBombCooltime = 3.0f;
		
		CSoundManager::getInstance()->playEffect(SEF_Item_Throw_PATH);

		m_throwpower.x = 6.01;
		m_throwpower.y = 6;
	
		if (m_fpowertime > 6) m_fpowertime = 5.5;

		g_CharactorData.n_Bomb_Count--;
		m_GameUi->Bombmanager(g_CharactorData.n_Bomb_Count);
		b2Body* bomb_body;
		if (g_bViewing) {
			bomb_body = Map_Body.CreateObj("Bomb", Player->getCharactorBody()->GetPosition() + b2Vec2(-1, 0), _world, wlayer);
			bomb_body->ApplyLinearImpulse(b2Vec2(m_throwpower.x, m_throwpower.y), Player->getCharactorBody()->GetWorldCenter(), true);
		}
		else if (!g_bViewing) {
			bomb_body = Map_Body.CreateObj("Bomb",Player->getCharactorBody()->GetPosition() + b2Vec2(-1, 0), _world, wlayer);
			bomb_body->ApplyLinearImpulse(b2Vec2(-m_throwpower.x, m_throwpower.y), Player->getCharactorBody()->GetWorldCenter(), true);
		}
		m_Explosion->CreateExplosion(bomb_body, 3, wlayer);

		m_fpowertime = 0;
	}*/
}

void HelloWorld::doPickup(Ref* pSender) {
	Player->setMoveState(MS_GET);
	m_before_state = MS_GET;
}

void HelloWorld::doOpen(Ref* pSender) {
	std::string sPath;
	std::string sPath_n;

	EffectSprite* Esp;
	EffectSprite* beforeESp;

	switch (g_CharactorData.key) {
	case REDKEY:
		g_CharactorData.key = NONE;
		m_GameUi->getRedKeySprite()->setOpacity(80);

		m_pOpenButton->setEnabled(false);
		m_pOpenButton->setVisible(false);

		sPath = "n_Images/item/Treasure_chest_Red_Open.png";
		sPath_n = "n_Images/item/Treasure_chest_Red_Open_n.png";
		Map_Body.setEffectSprite(Map_Body.getRedTreasureChest(), sPath, sPath_n, "wlayer");
		m_BuffManager->OpenTreasureChest(REDKEY);
		m_BuffManager->BuffInSlot(&g_CharactorData, Player->getCharactorBody()->GetPosition());
		m_GameUi->Heartmanager(&g_CharactorData);


		//CSoundManager::getInstance()->playEffect(SEF_BUFFEFFECT_PATH);
		break;
	case BLUEKEY:
		g_CharactorData.key = NONE;
		m_GameUi->getBlueKeySprite()->setOpacity(80);

		m_pOpenButton->setEnabled(false);
		m_pOpenButton->setVisible(false);

		sPath = "n_Images/item/Treasure_chest_Blue_Open.png";
		sPath_n = "n_Images/item/Treasure_chest_Blue_Open_n.png";
		Map_Body.setEffectSprite(Map_Body.getBlueTreasureChest(), sPath, sPath_n, "wlayer");
		m_BuffManager->OpenTreasureChest(BLUEKEY);
		m_BuffManager->BuffInSlot(&g_CharactorData, Player->getCharactorBody()->GetPosition());
		//CSoundManager::getInstance()->playEffect(SEF_BUFFEFFECT_PATH);
		break;
	case GREENKEY:
		g_CharactorData.key = NONE;
		m_GameUi->getGreenKeySprite()->setOpacity(80);

		m_pOpenButton->setEnabled(false);
		m_pOpenButton->setVisible(false);

		sPath = "n_Images/item/Treasure_chest_Green_Open.png";
		sPath_n = "n_Images/item/Treasure_chest_Green_Open_n.png";
		Map_Body.setEffectSprite(Map_Body.getGreenTreasureChest(), sPath, sPath_n, "wlayer");
		m_BuffManager->OpenTreasureChest(GREENKEY);
		m_BuffManager->BuffInSlot(&g_CharactorData, Player->getCharactorBody()->GetPosition());
		//CSoundManager::getInstance()->playEffect(SEF_BUFFEFFECT_PATH);
		break;
	default:
		break;
	}
}

void HelloWorld::doNext(Ref* pSender) {
	m_pPortalButton->setEnabled(false);
	m_pPortalButton->setVisible(false);
	
	this->clearFloor();

	m_bChangeStage = true;
}

void HelloWorld::BulletFire() {
	b2Body* bullet;
	if (g_bViewing) {
		if (g_CharactorData.StageNumber == SHELTER || g_CharactorData.bStage_blind) {
			Map_Body.CreateBullet(Player->getCharactorBody()->GetPosition() + b2Vec2(1, 0), _world, wlayer);
			bullet = Map_Body.getBulletBody();
			bullet->ApplyLinearImpulse(b2Vec2(20, 0), Player->getCharactorBody()->GetWorldCenter(), true);
			Bullets.push_back(Bullet(bullet, bullet->GetPosition(), m_aabbb));
		}
		else {
			Map_Body.CreateBullet(Player->getCharactorBody()->GetPosition() + b2Vec2(1, 0), _world, wlayer, true);
			bullet = Map_Body.getBulletBody();
			bullet->ApplyLinearImpulse(b2Vec2(20, 0), Player->getCharactorBody()->GetWorldCenter(), true);
			Bullets.push_back(Bullet(bullet, bullet->GetPosition(), m_aabbb));
		}
	}
	else if (!g_bViewing) {
		if (g_CharactorData.StageNumber == SHELTER || g_CharactorData.bStage_blind) {
			Map_Body.CreateBullet(Player->getCharactorBody()->GetPosition() + b2Vec2(-1, 0), _world, wlayer);
			bullet = Map_Body.getBulletBody();
			bullet->ApplyLinearImpulse(b2Vec2(-20, 0), Player->getCharactorBody()->GetWorldCenter(), true);
			Bullets.push_back(Bullet(bullet, bullet->GetPosition(), m_aabbb));
		}
		else {
			Map_Body.CreateBullet(Player->getCharactorBody()->GetPosition() + b2Vec2(1, 0), _world, wlayer, true);
			bullet = Map_Body.getBulletBody();
			bullet->ApplyLinearImpulse(b2Vec2(-20, 0), Player->getCharactorBody()->GetWorldCenter(), true);
			Bullets.push_back(Bullet(bullet, bullet->GetPosition(), m_aabbb));
		}
	}
}

void HelloWorld::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
	switch (keyCode) {
	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		PlayCharactorStep();
		InitCharactorOption();
		if (Player->getMoveState() == MS_ONLADDER)
			g_isLeft = true;

		if (m_bCan_Move && Player->getMoveState() != MS_UPDOWN) {
			if (!g_isThrow) {
				g_isRight = false;
				g_isLeft = true;									// Body 움직임위해 변경
				g_bViewing = false;
				g_isAttackOn = true;
				if (m_bTakePickaxe) {
					HelloWorld::KeyAction(MS_LEFT, LeftHand_On, "Pickaxe");
				}
				else if (m_bTakeSword)
					HelloWorld::KeyAction(MS_LEFT, LeftHand_On, "Sword");
				else if (m_bTakeGun)
					HelloWorld::KeyAction(MS_LEFT, LeftHand_On, "Gun");
				else
					HelloWorld::KeyAction(MS_LEFT, LeftHand_On, "");

				m_before_state = MS_LEFT;
				m_before_handstate = LeftHand_On;
			}
		}
		break;
	case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
		CSoundManager::getInstance()->stopBgm(BGM_Charactor_Step_PATH);
		if (m_bCan_UpDown) {
			g_isDown = true;
			InitCharactorOption();
		}
		if (!g_isThrow) {
			g_isDownBroken = true;
			if (m_bCan_Move && Player->getMoveState() != MS_UPDOWN) {
				g_isRight = false;
				g_isLeft = false;
				g_isStop = true;
				g_isAttackOn = true;
				if (m_bTakePickaxe)
					HelloWorld::KeyAction(MS_GET, Hand_On, "Pickaxe");
				else if (m_bTakeSword)
					HelloWorld::KeyAction(MS_GET, Hand_On, "Sword");
				else if (m_bTakeGun)
					HelloWorld::KeyAction(MS_GET, Hand_On, "Gun");
				else
					HelloWorld::KeyAction(MS_GET, Hand_On, "");

				if (g_CharactorData.StageNumber == SHELTER || g_CharactorData.bStage_blind) {
					if (g_bViewing) m_PlayerSprite->ESpriteData->setFlippedX(false);
					else if (!g_bViewing) m_PlayerSprite->ESpriteData->setFlippedX(true);
				}
				else {
					if (g_bViewing) m_PlayerSprite->SpriteData->setFlippedX(false);
					else if (!g_bViewing) m_PlayerSprite->SpriteData->setFlippedX(true);
				}

				m_before_state = MS_GET;
				m_before_handstate = Hand_On;
			}
		}
		break;
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		PlayCharactorStep();
		InitCharactorOption();
		if (Player->getMoveState() == MS_ONLADDER)
			g_isRight = true;

		if (m_bCan_Move && Player->getMoveState() != MS_UPDOWN) {
			if (!g_isThrow) {
				g_isLeft = false;
				g_isRight = true;
				g_bViewing = true;
				g_isAttackOn = true;
				if (m_bTakePickaxe) {
					HelloWorld::KeyAction(MS_RIGHT, RightHand_On, "Pickaxe");
				}
				else if (m_bTakeSword)
					HelloWorld::KeyAction(MS_RIGHT, RightHand_On, "Sword");
				else if (m_bTakeGun)
					HelloWorld::KeyAction(MS_RIGHT, RightHand_On, "Gun");
				else
					HelloWorld::KeyAction(MS_RIGHT, RightHand_On, "");

				m_before_state = MS_RIGHT;
				m_before_handstate = RightHand_On;
			}
		}
		break;
	case EventKeyboard::KeyCode::KEY_UP_ARROW:
		CSoundManager::getInstance()->stopBgm(BGM_Charactor_Step_PATH);
		if (!g_isThrow) {
			g_isUpBroken = true;
			if (m_bCan_UpDown) {
				Player->getCharactorBody()->SetGravityScale(0.0f);
				g_isUp = true;
				g_isLadderStop = false;
				if (m_bTakePickaxe)
					HelloWorld::KeyAction(MS_RIGHT, UPDOWN, "Pickaxe");
				else if (m_bTakeSword)
					HelloWorld::KeyAction(MS_RIGHT, UPDOWN, "Sword");
				else if (m_bTakeGun)
					HelloWorld::KeyAction(MS_RIGHT, UPDOWN, "Gun");
				else
					HelloWorld::KeyAction(MS_UPDOWN, Hand_On, "");
			}
		}
		break;
	case EventKeyboard::KeyCode::KEY_ALT:
		if (Player->getMoveState() != MS_PAIN) {
			CSoundManager::getInstance()->stopBgm(BGM_Charactor_Step_PATH);
			InitCharactorOption();

			g_isJump = true;
			g_isUp = false;
			g_isDown = false;
			g_isLadderLeft = false;
			g_isLadderRight = false;
			m_before_state = MS_STOP;
			if (m_bCan_UpDown) {
				g_isLadderJump = true;
			}
			//if (m_bTakePickaxe)
			//	HelloWorld::KeyAction(MS_JUMP, JumpHand_On, "Pickaxe");
			//else if (m_bTakeSword)
			//	HelloWorld::KeyAction(MS_JUMP, JumpHand_On, "Sword");
			//else
			//	HelloWorld::KeyAction(MS_JUMP, JumpHand_On, "");
			if (g_CharactorData.StageNumber == SHELTER || g_CharactorData.bStage_blind) {
				if (g_isLeft) m_PlayerSprite->ESpriteData->setFlippedX(true);
				if (g_isRight) m_PlayerSprite->ESpriteData->setFlippedX(false);
			}
			else {
				if (g_isLeft) m_PlayerSprite->SpriteData->setFlippedX(true);
				if (g_isRight) m_PlayerSprite->SpriteData->setFlippedX(false);
			}
			m_nJumpStep = 5;
		}
		break;

	case EventKeyboard::KeyCode::KEY_Z:
		if (m_bTakePickaxe) {
			CSoundManager::getInstance()->playEffect(SEF_Item_Throw_PATH);
			m_bTakePickaxe = false;
			
			if (g_bViewing) {
				Map_Body.CreateObj("Pickaxe", Player->getCharactorBody()->GetPosition(), _world, wlayer);
				//Map_Body.CreatePickaxe(Player->getCharactorBody()->GetPosition(), _world, wlayer);
				Map_Body.getItemBody()->ApplyLinearImpulse(b2Vec2(m_fImpulse / 3, m_fImpulse / 3), Player->getCharactorBody()->GetWorldCenter(), true);
			}
			else if (!g_bViewing) {
				Map_Body.CreateObj("Pickaxe", Player->getCharactorBody()->GetPosition() + b2Vec2(-1, 0), _world, wlayer);
				//Map_Body.CreatePickaxe(Player->getCharactorBody()->GetPosition() + b2Vec2(-1, 0), _world, wlayer);
				Map_Body.getItemBody()->ApplyLinearImpulse(b2Vec2(-m_fImpulse / 3, m_fImpulse / 3), Player->getCharactorBody()->GetWorldCenter(), true);
				Sprite* sp = (Sprite*)Map_Body.getItemBody()->GetUserData();
				
			}
			if(g_CharactorData.StageNumber == SHELTER || g_CharactorData.bStage_blind)
				wlayer->removeChild(Player->getCharactor_Hand()->getSprite()->ESpriteData, true);
			else
				wlayer->removeChild(Player->getCharactor_Hand()->getSprite()->SpriteData, true);

			m_PlayerSprite = Player->AnimationState(MS_STOP, wlayer);		// 다시 body에 movestate 상태에 따라 애니메이션을 만들어 넣어줌
			m_PlayerSprite = Player->getPlayer();				// HelloWorld Scene에 넣기 위해 Sprite 가져옴
			
			if (g_CharactorData.StageNumber == SHELTER || g_CharactorData.bStage_blind) {
				if (g_bViewing) m_PlayerSprite->ESpriteData->setFlippedX(false);
				else if (!g_bViewing) m_PlayerSprite->ESpriteData->setFlippedX(true);
			}
			else {
				if (g_bViewing) m_PlayerSprite->SpriteData->setFlippedX(false);
				else if (!g_bViewing) m_PlayerSprite->SpriteData->setFlippedX(true);
			}
			//log("Player position %f %f", sp->getPosition().x, sp->getPosition().y);
		}
		else if (m_bTakeSword) {
			CSoundManager::getInstance()->playEffect(SEF_Item_Throw_PATH);
			m_bTakeSword = false;

			if (g_bViewing) {
				Map_Body.CreateObj("Sword", Player->getCharactorBody()->GetPosition(), _world, wlayer);
				//Map_Body.CreateSword(Player->getCharactorBody()->GetPosition(), _world, wlayer);
				Map_Body.getItemBody()->ApplyLinearImpulse(b2Vec2(m_fImpulse / 3, m_fImpulse / 3), Player->getCharactorBody()->GetWorldCenter(), true);
			}
			else if (!g_bViewing) {
				Map_Body.CreateObj("Sword", Player->getCharactorBody()->GetPosition(), _world, wlayer);
				//Map_Body.CreateSword(Player->getCharactorBody()->GetPosition() + b2Vec2(-1, 0), _world, wlayer);
				Map_Body.getItemBody()->ApplyLinearImpulse(b2Vec2(-m_fImpulse / 3, m_fImpulse / 3), Player->getCharactorBody()->GetWorldCenter(), true);
			}
			if (g_CharactorData.StageNumber == SHELTER || g_CharactorData.bStage_blind)
				wlayer->removeChild(Player->getCharactor_Hand()->getSprite()->ESpriteData, true);
			else
				wlayer->removeChild(Player->getCharactor_Hand()->getSprite()->SpriteData, true);

			m_PlayerSprite = Player->AnimationState(MS_STOP, wlayer);		// 다시 body에 movestate 상태에 따라 애니메이션을 만들어 넣어줌
			m_PlayerSprite = Player->getPlayer();				// HelloWorld Scene에 넣기 위해 Sprite 가져옴

			if (g_CharactorData.StageNumber == SHELTER || g_CharactorData.bStage_blind) {
				if (g_bViewing) m_PlayerSprite->ESpriteData->setFlippedX(false);
				else if (!g_bViewing) m_PlayerSprite->ESpriteData->setFlippedX(true);
			}
			else {
				if (g_bViewing) m_PlayerSprite->SpriteData->setFlippedX(false);
				else if (!g_bViewing) m_PlayerSprite->SpriteData->setFlippedX(true);
			}
		}
		else if (m_bTakeGun) {
			CSoundManager::getInstance()->playEffect(SEF_Item_Throw_PATH);
			m_bTakeGun = false;

			if (g_bViewing) {
				Map_Body.CreateObj("Gun", Player->getCharactorBody()->GetPosition(), _world, wlayer);
				//Map_Body.CreateGun(Player->getCharactorBody()->GetPosition(), _world, wlayer);
				Map_Body.getItemBody()->ApplyLinearImpulse(b2Vec2(m_fImpulse / 3, m_fImpulse / 3), Player->getCharactorBody()->GetWorldCenter(), true);
			}
			else if (!g_bViewing) {
				Map_Body.CreateObj("Gun", Player->getCharactorBody()->GetPosition(), _world, wlayer);
				//Map_Body.CreateGun(Player->getCharactorBody()->GetPosition() + b2Vec2(-1, 0), _world, wlayer);
				Map_Body.getItemBody()->ApplyLinearImpulse(b2Vec2(-m_fImpulse / 3, m_fImpulse / 3), Player->getCharactorBody()->GetWorldCenter(), true);
			}
			if (g_CharactorData.StageNumber == SHELTER || g_CharactorData.bStage_blind)
				wlayer->removeChild(Player->getCharactor_Hand()->getSprite()->ESpriteData, true);
			else
				wlayer->removeChild(Player->getCharactor_Hand()->getSprite()->SpriteData, true);

			m_PlayerSprite = Player->AnimationState(MS_STOP, wlayer);
			m_PlayerSprite = Player->getPlayer();

			if (g_CharactorData.StageNumber == SHELTER || g_CharactorData.bStage_blind) {
				if (g_bViewing) m_PlayerSprite->ESpriteData->setFlippedX(false);
				else if (!g_bViewing) m_PlayerSprite->ESpriteData->setFlippedX(true);
			}
			else {
				if (g_bViewing) m_PlayerSprite->SpriteData->setFlippedX(false);
				else if (!g_bViewing) m_PlayerSprite->SpriteData->setFlippedX(true);
			}
		}
		g_CharactorData.item = IS_NONE;
		break;
		
	case EventKeyboard::KeyCode::KEY_B:
		
		break;
	}

	if (g_isLeft && g_isUpBroken) {
		g_isLeftUpBroken = true;
	}
	else g_isLeftUpBroken = false;
	
	if (g_isRight && g_isUpBroken) {
		g_isRightUpBroken = true;
	}
	else g_isRightUpBroken = false;
	
	
}

void HelloWorld::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	switch (keyCode) {
	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		CSoundManager::getInstance()->stopBgm(BGM_Charactor_Step_PATH);
		if (!g_isThrow && Player->getMoveState() != MS_UPDOWN) {
			g_isRight = false;
			g_isLeft = false;
			g_bViewing = false;
			if (m_bTakePickaxe)
				HelloWorld::KeyAction(MS_STOP, Hand_On, "Pickaxe");
			else if (m_bTakeSword)
				HelloWorld::KeyAction(MS_STOP, Hand_On, "Sword");
			else if (m_bTakeGun)
				HelloWorld::KeyAction(MS_STOP, Hand_On, "Gun");
			else
				HelloWorld::KeyAction(MS_STOP, Hand_On, "");
		}
		break;
	case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
		CSoundManager::getInstance()->stopBgm(BGM_Charactor_Step_PATH);
		if (m_bCan_UpDown) {
			g_isDown = false;
			Player->getCharactorBody()->SetType(b2_kinematicBody);
			Player->getCharactorBody()->SetGravityScale(1.0f);
		}
		g_isDownBroken = false;
		if (!g_isThrow && Player->getMoveState() != MS_UPDOWN) {
			g_isStop = false;
			if (m_bTakePickaxe)
				HelloWorld::KeyAction(MS_STOP, Hand_On, "Pickaxe");
			else if (m_bTakeSword)
				HelloWorld::KeyAction(MS_STOP, Hand_On, "Sword");
			else if (m_bTakeGun)
				HelloWorld::KeyAction(MS_STOP, Hand_On, "Gun");
			else
				HelloWorld::KeyAction(MS_STOP, Hand_On, "");

			if (g_CharactorData.StageNumber == SHELTER || g_CharactorData.bStage_blind) {
				if (g_bViewing) m_PlayerSprite->ESpriteData->setFlippedX(false);
				else if (!g_bViewing) m_PlayerSprite->ESpriteData->setFlippedX(true);
			}
			else {
				if (g_bViewing) m_PlayerSprite->SpriteData->setFlippedX(false);
				else if (!g_bViewing) m_PlayerSprite->SpriteData->setFlippedX(true);
			}
		}
		break;
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		CSoundManager::getInstance()->stopBgm(BGM_Charactor_Step_PATH);
		if (!g_isThrow && Player->getMoveState() != MS_UPDOWN) {
			g_isLeft = false;
			g_isRight = false;
			g_bViewing = true;
			if (m_bTakePickaxe)
				HelloWorld::KeyAction(MS_STOP, Hand_On, "Pickaxe");
			else if (m_bTakeSword)
				HelloWorld::KeyAction(MS_STOP, Hand_On, "Sword");
			else if (m_bTakeGun)
				HelloWorld::KeyAction(MS_STOP, Hand_On, "Gun");
			else
				HelloWorld::KeyAction(MS_STOP, Hand_On, "");

		}
		break;
	case EventKeyboard::KeyCode::KEY_UP_ARROW:
		if (m_bCan_UpDown) {
			m_bMove = false;
			Player->getCharactorBody()->SetType(b2_kinematicBody);
			Player->getCharactorBody()->SetLinearVelocity(b2Vec2(0, 0));
			Sprite* sp = static_cast<Sprite*>(Player->getCharactorBody()->GetUserData());
			sp->stopAllActions();
		}
		if (!g_isThrow) {
			//	g_isLeft = false;
			//	g_isRight = false;
			g_isUpBroken = false;
			g_isUp = false;
			g_isLadderStop = true;

			//HelloWorld::KeyAction(MS_UPDOWN, Hand_On, "");

		}
		break;
	case EventKeyboard::KeyCode::KEY_ALT:
		if (g_isLeft || g_isRight && m_bDo_Jump) {
			//CSoundManager::getInstance()->playBgm(BGM_Charactor_Step_PATH, true, false);
		}
		g_isJump = false;
		g_isLadderJump = false;
		if (m_bTakePickaxe)
			HelloWorld::KeyAction(MS_STOP, Hand_On, "Pickaxe");
		else if (m_bTakeSword)
			HelloWorld::KeyAction(MS_STOP, Hand_On, "Sword");
		else if (m_bTakeGun) {
			HelloWorld::KeyAction(MS_STOP, Hand_On, "Gun");
		}
		else
			HelloWorld::KeyAction(MS_STOP, Hand_On, "");

		break;
	case EventKeyboard::KeyCode::KEY_Z:
		g_isLeft = false;
		g_isRight = false;
		g_isDown = false;
		g_isUp = false;
		g_isAttackOn = false;
		g_isJump = false;
		g_isStop = false;
		Player->setMoveState(MS_STOP);
		break;
	}
	
}

void HelloWorld::KeyAction(_moveState state, _HandState handstate, std::string Item) {
	Player->setMoveState(state);
	if (m_bTakePickaxe) {
		if(g_CharactorData.StageNumber == SHELTER || g_CharactorData.bStage_blind)
			wlayer->removeChild(Player->getCharactor_Hand()->getSprite()->ESpriteData, true);
		else
			wlayer->removeChild(Player->getCharactor_Hand()->getSprite()->SpriteData, true);
		if (handstate == PAIN) {
			log("a");
		}
		Player->getCharactor_Hand()->AnimationState(Item, handstate, wlayer, g_bViewing);
		m_PlayerSprite = Player->getCharactor_Hand()->getSprite();				// HelloWorld Scene에 넣기 위해 Sprite 가져옴
		if (g_CharactorData.StageNumber == SHELTER || g_CharactorData.bStage_blind)
			m_PlayerSprite->ESpriteData->setFlippedX(true);
		else
			m_PlayerSprite->SpriteData->setFlippedX(true);
	}
	else if (m_bTakeSword) {
		if (g_CharactorData.StageNumber == SHELTER || g_CharactorData.bStage_blind)
			wlayer->removeChild(Player->getCharactor_Hand()->getSprite()->ESpriteData, true);
		else
			wlayer->removeChild(Player->getCharactor_Hand()->getSprite()->SpriteData, true);
		Player->getCharactor_Hand()->AnimationState(Item, handstate, wlayer, g_bViewing);
		m_PlayerSprite = Player->getCharactor_Hand()->getSprite();				// HelloWorld Scene에 넣기 위해 Sprite 가져옴
		if (g_CharactorData.StageNumber == SHELTER || g_CharactorData.bStage_blind)
			m_PlayerSprite->ESpriteData->setFlippedX(true);
		else
			m_PlayerSprite->SpriteData->setFlippedX(true);
	}
	else if (m_bTakeGun) {
		if (g_CharactorData.StageNumber == SHELTER || g_CharactorData.bStage_blind)
			wlayer->removeChild(Player->getCharactor_Hand()->getSprite()->ESpriteData, true);
		else
			wlayer->removeChild(Player->getCharactor_Hand()->getSprite()->SpriteData, true);
		Player->getCharactor_Hand()->AnimationState(Item, handstate, wlayer, g_bViewing);
		m_PlayerSprite = Player->getCharactor_Hand()->getSprite();
		if (g_CharactorData.StageNumber == SHELTER || g_CharactorData.bStage_blind)
			m_PlayerSprite->ESpriteData->setFlippedX(true);
		else
			m_PlayerSprite->SpriteData->setFlippedX(true);
	}
	else {
		if (state == MS_GET)
			state = MS_STOP;
		if (g_CharactorData.StageNumber == SHELTER || g_CharactorData.bStage_blind)
			wlayer->removeChild(m_PlayerSprite->ESpriteData);					// 현재 body에 적용된 Sprite 지움
		else {
			//Player->SpriteChange(state, wlayer);
			wlayer->removeChild(m_PlayerSprite->SpriteData, true);
		}
		m_PlayerSprite = Player->AnimationState(state, wlayer);		// 다시 body에 movestate 상태에 따라 애니메이션을 만들어 넣어줌
	//	m_PlayerSprite = Player->getPlayer();				// HelloWorld Scene에 넣기 위해 Sprite 가져옴
		if (g_CharactorData.StageNumber == SHELTER || g_CharactorData.bStage_blind)
			m_PlayerSprite->ESpriteData->setFlippedX(true);
		else
			m_PlayerSprite->SpriteData->setFlippedX(true);
	}

	if (g_CharactorData.StageNumber == SHELTER || g_CharactorData.bStage_blind) {
		if (g_isLeft || !g_bViewing) m_PlayerSprite->ESpriteData->setFlippedX(true);
		else if (g_isRight || g_bViewing) m_PlayerSprite->ESpriteData->setFlippedX(false);
	}
	else {
		if (g_isLeft || !g_bViewing) m_PlayerSprite->SpriteData->setFlippedX(true);
		else if (g_isRight || g_bViewing) m_PlayerSprite->SpriteData->setFlippedX(false);
	}
}

void HelloWorld::JumpManager(float dt) {
	if ((m_fJumpTimeout -= dt) <= 0) {
		m_fJumpTimeout = 1;
		if (g_isLeft && g_isJump || g_isLadderJump)
			Player->setMoveState(MS_LEFTJUMP);
		if (g_isRight && g_isJump || g_isLadderJump)
			Player->setMoveState(MS_RIGHTJUMP);
		if (g_isJump && g_isLeft == false && g_isRight == false || g_isLadderJump)
			Player->setMoveState(MS_JUMP);
		m_bCan_Move = true;
		b2Vec2 force = Player->getCharactorBody()->GetLinearVelocity();
		if (force.y < 3) {
			switch (Player->getMoveState()) {
			case MS_RIGHTJUMP:
				//if (nJumpStep > 0) {
				Player->getCharactorBody()->ApplyLinearImpulse(b2Vec2(m_fImpulse * 2, m_fImpulse * 2), Player->getCharactorBody()->GetWorldCenter(), true);
				CSoundManager::getInstance()->playEffect(SEF_Charactor_Jump_PATH);
				//	m_nJumpStep--;
				log("RJ");
				nc++;
				//}
				break;
			case MS_LEFTJUMP:
				//if (nJumpStep > 0) {
				Player->getCharactorBody()->ApplyLinearImpulse(b2Vec2(-m_fImpulse * 2, m_fImpulse * 2), Player->getCharactorBody()->GetWorldCenter(), true);
				CSoundManager::getInstance()->playEffect(SEF_Charactor_Jump_PATH);
				//	m_nJumpStep--;
				log("LJ");
				nc++;
				//	}
				break;
			case MS_JUMP:
				//if (nJumpStep > 0) {
				Player->getCharactorBody()->ApplyLinearImpulse(b2Vec2(0, m_fImpulse * 2), Player->getCharactorBody()->GetWorldCenter(), true);
				CSoundManager::getInstance()->playEffect(SEF_Charactor_Jump_PATH);
				//Player->getCharactorBody()->ApplyForce(b2Vec2(0, JUMPPOW), Player->getCharactorBody()->GetWorldCenter(), true);
				//body->ApplyForceToCenter(b2Vec2(500,500), true);
				//body->ApplyTorque(45.0f, true);
			//	m_nJumpStep--;
				log("IJ");
				//}
				break;
			}
		}
	}
}

void HelloWorld::CharactorPainAction() {
	Player->setMoveState(MS_PAIN);

	CSoundManager::getInstance()->playEffect(SEF_Charactor_Pain_PATH);

	if (m_bTakePickaxe) {
		KeyAction(Player->getMoveState(), PAIN, "Pickaxe");
	}
	else if (m_bTakeSword) {
		KeyAction(Player->getMoveState(), PAIN, "Sword");
	}
	else if (m_bTakeGun) {
		KeyAction(Player->getMoveState(), PAIN, "Gun");
	}
	else {
		KeyAction(Player->getMoveState(), PAIN, "");
	}

	m_fDelayTime = 2;

	if (g_bViewing)
		Player->getCharactorBody()->ApplyLinearImpulse(b2Vec2(-2, 3), Player->getCharactorBody()->GetWorldCenter(), true);
	else if (!g_bViewing)
		Player->getCharactorBody()->ApplyLinearImpulse(b2Vec2(2, 3), Player->getCharactorBody()->GetWorldCenter(), true);

	m_GameUi->Heartmanager(&g_CharactorData);
	if (g_CharactorData.n_Hp_Count <= 0) {
		this->GameOver(this);
	}
}

void HelloWorld::InitCharactorOption() {
	Player->getCharactorBody()->SetType(b2_dynamicBody);
	Player->getCharactorBody()->SetGravityScale(1.0f);
}

void HelloWorld::clearFloor() {
	
	for (int i = 0; i < m_Contact_clear.size(); i++) {
		*m_Contact_clear.at(i) = 0;
	}

	// 씬을 정리할때 캐릭터가 살아있는지 죽어있는지 확인하고 GameOver인지
	// 다음 층으로 이동하기 위해 LoadingScene으로의 전환인지 결정한다.
	if (Player->getCharactorBody()->GetHp() > 0) {
		if (g_CharactorData.StageNumber == SHELTER) {
			gStageNumber++;
			m_GameUi->CreateStageMessage(gStageNumber);
			auto pLoading = LoadingScene::createScene(g_CharactorData);
			Director::getInstance()->replaceScene(pLoading);
		}
		else {
			g_CharactorData.StageNumber = SHELTER;
			auto pLoading = LoadingScene::createScene(g_CharactorData);
			Director::getInstance()->replaceScene(pLoading);
		}
	}
}


void HelloWorld::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) {
	Scene::draw(renderer, transform, flags);

	glLineWidth(2);
	ccDrawColor4F(255, 0, 0, 255);

	/*
	std::list<Bomb>::iterator iter;
	if (Bombs != nullptr) {
		for (iter = Bombs->begin(); iter != Bombs->end(); ++iter) {
			ccDrawRect(ccp(m_aabb.lowerBound.x * 32, m_aabb.lowerBound.y * 32), ccp(m_aabb.upperBound.x * 32, m_aabb.upperBound.y * 32));
		}
	}
	
	std::list<Bullet>::iterator biter;
	if (Bullets.size() > 0) {
		for (biter = Bullets.begin(); biter != Bullets.end(); ++biter) {
			ccDrawRect(ccp(m_aabbb.lowerBound.x * 32, m_aabbb.lowerBound.y * 32), ccp(m_aabbb.upperBound.x * 32, m_aabbb.upperBound.y * 32));
		}
	}*/
	
	ccDrawPoint(Vec2(Player->getCharactorBody()->GetPosition().x * 32, Player->getCharactorBody()->GetPosition().y * 32));
	//ccDrawRect(ccp(Player->getCharactorBody()->GetPosition().x * 32 - 70, Player->getCharactorBody()->GetPosition().y * 32 + 40), ccp(Player->getCharactorBody()->GetPosition().x * 32 - 30, Player->getCharactorBody()->GetPosition().y * 32 + 10));

	_customCmd.init(10, transform, flags);
	_customCmd.func = CC_CALLBACK_0(HelloWorld::onDraw, this, transform, flags);

	renderer->addCommand(&_customCmd);
}

void HelloWorld::onDraw(const Mat4 &transform, uint32_t flags) {
	Director* director = Director::getInstance();
	CCASSERT(nullptr != director, "Director is null when seting matrix stack");
	director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, transform);

	GL::enableVertexAttribs(cocos2d::GL::VERTEX_ATTRIB_FLAG_POSITION);
	//_world->DrawDebugData();
	CHECK_GL_ERROR_DEBUG();

	director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}

void HelloWorld::doNotification(CCObject* obj) {

}






CMyContactListener::CMyContactListener() {
	gNumFootContacts = 0;
}

CMyContactListener::~CMyContactListener() {

}


void CMyContactListener::BeginContact(b2Contact* contact) {
	// 3 = 캐릭터 발
	// 4 = 캐릭터 좌우
	// 5 = 장애물
	// 6 = 아이템
	// 7 = pickaxe
	// 8 = monster
	// 9 = bomb

	UINT16 Contact_Object_A = contact->GetFixtureA()->GetFilterData().categoryBits;
	UINT16 Contact_Object_B = contact->GetFixtureB()->GetFilterData().categoryBits;

	void* fixtureUserData = contact->GetFixtureA()->GetUserData(); // Charactor foot
	if ((int)fixtureUserData == 3) {			// 발이                       // Charactor foot
		if (Contact_Object_B == 0x0010)			// 몬스터 머리위에 닿았을때.
			gNumFootAttackContacts++;
		if (Contact_Object_B == 0x3000)	
			gNumBombContacts++;
		if (Contact_Object_B == 0x7077)
			gNumLongLadderContacts++;

		gNumFootContacts++;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 3) {                                          // Charactor foot
		if (Contact_Object_A == 0x0010)
			gNumFootAttackContacts++;
		if (Contact_Object_A == 0x3000)
			gNumBombContacts++;
		if (Contact_Object_A == 0x7077)
			gNumLongLadderContacts++;

		gNumFootContacts++;
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 4) {		// 캐릭터 좌우 body가             // Charactor Right & Left
		if (Contact_Object_B == 0x0010)		// 몬스터에 닿았을 경우
			gNumMonsterContacts++;
		if (Contact_Object_B == 0x3000)		// item상태의 폭탄에 닿았을때
			gNumBombContacts++;
		if (Contact_Object_B == 0x0003) {	// 폭탄파편에 닿았을 때
			gNumChipToCharactor++;
			gNumChipToCharactor++;
		}

		gNumRightContacts++;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 4) {											// Charactor Right & Left
		if (Contact_Object_A == 0x0010)
			gNumMonsterContacts++;
		if (Contact_Object_A == 0x3000)
			gNumBombContacts++;
		if (Contact_Object_A == 0x0003) {	// 폭탄파편에 닿았을 때
			gNumChipToCharactor++;
			gNumChipToCharactor++;
		}

		gNumRightContacts++;
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 5) {											// Obstacle
		gNumObstacleContacts++;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();					// Obstacle
	if ((int)fixtureUserData == 5) {
		gNumObstacleContacts++;
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();					// Potion
	if ((int)fixtureUserData == 6) {
		gNumItemContacts++;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();					// Potion
	if ((int)fixtureUserData == 6) {
		gNumItemContacts++;
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();					// Pickaxe
	if ((int)fixtureUserData == 7) {
		if (Contact_Object_B == BLOCK_CATEGORY || Contact_Object_B == HARDBLOCK_CATEGORY || Contact_Object_B == GBLOCK_CATEGORY)
			gNumpickaxeContacts++;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();					// Pickaxe
	if ((int)fixtureUserData == 7) {
		if (Contact_Object_A == BLOCK_CATEGORY || Contact_Object_A == HARDBLOCK_CATEGORY || Contact_Object_A == GBLOCK_CATEGORY)
			gNumpickaxeContacts++;
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();					// Charactor Foot Attack-> Monster 
	if ((int)fixtureUserData == 8) {
		if (Contact_Object_B == BLOCK_CATEGORY || Contact_Object_B == HARDBLOCK_CATEGORY || Contact_Object_B == GBLOCK_CATEGORY) { // categoryBits 가 기본 블럭일 시 충돌연산은 되지만 충돌한 것으로 치지 않기 위해(블럭이 아닌 다른 오브젝트와 충돌을 위해)
			gNumMonsterFootContacts++;
		}
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();					// Charactor Foot Attack-> Monster 
	if ((int)fixtureUserData == 8) {
		if (Contact_Object_A == BLOCK_CATEGORY || Contact_Object_A == HARDBLOCK_CATEGORY || Contact_Object_A == GBLOCK_CATEGORY) {
			gNumMonsterFootContacts++;
		}
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();					// Bomb Item
	if ((int)fixtureUserData == 9) {
		if (Contact_Object_B == BLOCK_CATEGORY || Contact_Object_B == HARDBLOCK_CATEGORY || Contact_Object_B == GBLOCK_CATEGORY)
			gNumBombContacts++;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();					// Bomb Item
	if ((int)fixtureUserData == 9) {
		if (Contact_Object_A == BLOCK_CATEGORY || Contact_Object_A == HARDBLOCK_CATEGORY || Contact_Object_A == GBLOCK_CATEGORY) {
			gNumBombContacts++;
		}
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();					// Frog foot
	if ((int)fixtureUserData == 10) {
		if (Contact_Object_B == BLOCK_CATEGORY || Contact_Object_B == HARDBLOCK_CATEGORY || Contact_Object_B == GBLOCK_CATEGORY)
			gNumFrogFootContacts++;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();					// Frog foot
	if ((int)fixtureUserData == 10) {
		if (Contact_Object_A == BLOCK_CATEGORY || Contact_Object_A == HARDBLOCK_CATEGORY || Contact_Object_A == GBLOCK_CATEGORY) {
			gNumFrogFootContacts++;
		}
	}

	fixtureUserData = contact->GetFixtureA()->GetUserData();					// Sword
	if ((int)fixtureUserData == 11) {
		if (Contact_Object_B == BLOCK_CATEGORY || Contact_Object_B == HARDBLOCK_CATEGORY || Contact_Object_B == GBLOCK_CATEGORY)
			gNumSwordContacts++;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();					// Sword
	if ((int)fixtureUserData == 11) {
		if (Contact_Object_A == BLOCK_CATEGORY || Contact_Object_A == HARDBLOCK_CATEGORY || Contact_Object_A == GBLOCK_CATEGORY) {
			gNumSwordContacts++;
		}
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();					// Gun
	if ((int)fixtureUserData == 12) {
		if (Contact_Object_B == BLOCK_CATEGORY || Contact_Object_B == HARDBLOCK_CATEGORY || Contact_Object_B == GBLOCK_CATEGORY)
			gNumGunContacts++;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();					// Gun
	if ((int)fixtureUserData == 12) {
		if (Contact_Object_A == BLOCK_CATEGORY || Contact_Object_A == HARDBLOCK_CATEGORY || Contact_Object_A == GBLOCK_CATEGORY) {
			gNumGunContacts++;
		}
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();					// Ladder Item
	if ((int)fixtureUserData == 13) {
		if (Contact_Object_B == BLOCK_CATEGORY || Contact_Object_B == HARDBLOCK_CATEGORY || Contact_Object_B == GBLOCK_CATEGORY)
			gNumLadderContacts++;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();					// Ladder Item
	if ((int)fixtureUserData == 13) {
		if (Contact_Object_A == BLOCK_CATEGORY || Contact_Object_A == HARDBLOCK_CATEGORY || Contact_Object_A == GBLOCK_CATEGORY) {
			gNumLadderContacts++;
		}
	}


	fixtureUserData = contact->GetFixtureA()->GetUserData();					
	if ((int)fixtureUserData == 15) {
		if ( Contact_Object_B == 3 || Contact_Object_B == 4){}
		//	gNumChipToCharactor++;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 15) { // 폭탄 파편과
		if (  Contact_Object_A == 3 || Contact_Object_A == 4) {// 캐릭터의 충돌
		//	gNumChipToCharactor++;
		}
	}

	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == FALLINGBLADE_CONTACT) {
		gNumFallingBladeContacts++;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == FALLINGBLADE_CONTACT) {
		gNumFallingBladeContacts++;
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == TRAPBULLET_CONTACT) {
		//if(Contact_Object_B == 3 || Contact_Object_B == 4)
			gNumTrapBulletContacts++;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == TRAPBULLET_CONTACT) {
		//if (Contact_Object_A == 3 || Contact_Object_A == 4)
			gNumTrapBulletContacts++;
	}

	fixtureUserData = contact->GetFixtureA()->GetUserData();					// Shop -> Man
	if ((int)fixtureUserData == 70) {
	//	if (Contact_Object_B == 2)
			gNumShopManContacts++;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();					// Shop -> Man
	if ((int)fixtureUserData == 70) {
	//	if (Contact_Object_A == 2)
			gNumShopManContacts++;
	}

	fixtureUserData = contact->GetFixtureA()->GetUserData();					
	if ((int)fixtureUserData == 90) {
		if (Contact_Object_B == BLOCK_CATEGORY || Contact_Object_B == HARDBLOCK_CATEGORY || Contact_Object_B == GBLOCK_CATEGORY)
			gNumRedKeyContacts++;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();					
	if ((int)fixtureUserData == 90) {
		if (Contact_Object_A == BLOCK_CATEGORY || Contact_Object_A == HARDBLOCK_CATEGORY || Contact_Object_A == GBLOCK_CATEGORY)
			gNumRedKeyContacts++;
	}

	fixtureUserData = contact->GetFixtureA()->GetUserData();					
	if ((int)fixtureUserData == 91) {
		if (Contact_Object_B == BLOCK_CATEGORY || Contact_Object_B == HARDBLOCK_CATEGORY || Contact_Object_B == GBLOCK_CATEGORY)
			gNumBlueKeyContacts++;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();					
	if ((int)fixtureUserData == 91) {
		if (Contact_Object_A == BLOCK_CATEGORY || Contact_Object_A == HARDBLOCK_CATEGORY || Contact_Object_A == GBLOCK_CATEGORY)
			gNumBlueKeyContacts++;
	}

	fixtureUserData = contact->GetFixtureA()->GetUserData();					
	if ((int)fixtureUserData == 92) {
		if (Contact_Object_B == BLOCK_CATEGORY || Contact_Object_B == HARDBLOCK_CATEGORY || Contact_Object_B == GBLOCK_CATEGORY)
			gNumGreenKeyContacts++;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();					
	if ((int)fixtureUserData == 92) {
		if (Contact_Object_A == BLOCK_CATEGORY || Contact_Object_A == HARDBLOCK_CATEGORY || Contact_Object_A == GBLOCK_CATEGORY)
			gNumGreenKeyContacts++;
	}

	fixtureUserData = contact->GetFixtureA()->GetUserData();					// Gold
	if ((int)fixtureUserData == 77) {
		if (Contact_Object_B == BLOCK_CATEGORY || Contact_Object_B == HARDBLOCK_CATEGORY || Contact_Object_B == GBLOCK_CATEGORY)
			gNumGoldContacts++;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();					// Gold
	if ((int)fixtureUserData == 77) {
		if (Contact_Object_A == BLOCK_CATEGORY || Contact_Object_A == HARDBLOCK_CATEGORY || Contact_Object_A == GBLOCK_CATEGORY)
			gNumGoldContacts++;
	}
}

void CMyContactListener::EndContact(b2Contact* contact) {
	UINT16 Contact_Object_A = contact->GetFixtureA()->GetFilterData().categoryBits;
	UINT16 Contact_Object_B = contact->GetFixtureB()->GetFilterData().categoryBits;

	void* fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 3) {
		if (Contact_Object_B == 0x0010) 
			gNumFootAttackContacts--;
		if (Contact_Object_B == 0x3000)			
			gNumBombContacts--;
		if (Contact_Object_B == 0x7077)
			gNumLongLadderContacts--;

		gNumFootContacts--;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 3) {
		if (Contact_Object_A == 0x0010) 
			gNumFootAttackContacts--;
		if (Contact_Object_A == 0x3000)
			gNumBombContacts--;
		if (Contact_Object_A == 0x7077)
			gNumLongLadderContacts--;

		gNumFootContacts--;
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 4) {
		if (Contact_Object_B == 0x0010)
			gNumMonsterContacts--;
		if (Contact_Object_B == 0x3000)
			gNumBombContacts--;
		if (Contact_Object_B == 0x0003)		// 폭탄파편에 닿았을 때
			gNumChipToCharactor--;

		gNumRightContacts--;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 4) {
		if (Contact_Object_A == 0x0010)
			gNumMonsterContacts--;
		if (Contact_Object_A == 0x3000)
			gNumBombContacts--;
		if (Contact_Object_A == 0x0003)		// 폭탄파편에 닿았을 때
			gNumChipToCharactor--;

		gNumRightContacts--;
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 5) {
		gNumObstacleContacts--;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 5) {
		gNumObstacleContacts--;
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 6) {
		gNumItemContacts--;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 6) {
		gNumItemContacts--;
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 7) {
		if (Contact_Object_B == BLOCK_CATEGORY || Contact_Object_B == HARDBLOCK_CATEGORY || Contact_Object_B == GBLOCK_CATEGORY)
			gNumpickaxeContacts--;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 7) {
		if (Contact_Object_A == BLOCK_CATEGORY || Contact_Object_A == HARDBLOCK_CATEGORY || Contact_Object_A == GBLOCK_CATEGORY)
			gNumpickaxeContacts--;
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 8) {
		if (Contact_Object_B == BLOCK_CATEGORY || Contact_Object_B == HARDBLOCK_CATEGORY || Contact_Object_B == GBLOCK_CATEGORY) {
			gNumMonsterFootContacts--;
		}
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 8) {
		if (Contact_Object_A == BLOCK_CATEGORY || Contact_Object_A == HARDBLOCK_CATEGORY || Contact_Object_A == GBLOCK_CATEGORY) {
			gNumMonsterFootContacts--;
		}
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 9) {
		if (Contact_Object_B == BLOCK_CATEGORY || Contact_Object_B == HARDBLOCK_CATEGORY || Contact_Object_B == GBLOCK_CATEGORY)
			gNumBombContacts--;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 9) {
		if (Contact_Object_A == BLOCK_CATEGORY || Contact_Object_A == HARDBLOCK_CATEGORY || Contact_Object_A == GBLOCK_CATEGORY)
			gNumBombContacts--;
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 10) {
		if (Contact_Object_B == BLOCK_CATEGORY || Contact_Object_B == HARDBLOCK_CATEGORY || Contact_Object_B == GBLOCK_CATEGORY)
			gNumFrogFootContacts--;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 10) {
		if (Contact_Object_A == BLOCK_CATEGORY || Contact_Object_A == HARDBLOCK_CATEGORY || Contact_Object_A == GBLOCK_CATEGORY) {
			gNumFrogFootContacts--;
		}
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 11) {
		if (Contact_Object_B == BLOCK_CATEGORY || Contact_Object_B == HARDBLOCK_CATEGORY || Contact_Object_B == GBLOCK_CATEGORY)
			gNumSwordContacts--;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 11) {
		if (Contact_Object_A == BLOCK_CATEGORY || Contact_Object_A == HARDBLOCK_CATEGORY || Contact_Object_A == GBLOCK_CATEGORY) {
			gNumSwordContacts--;
		}
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 12) {
		if (Contact_Object_B == BLOCK_CATEGORY || Contact_Object_B == HARDBLOCK_CATEGORY || Contact_Object_B == GBLOCK_CATEGORY)
			gNumGunContacts--;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 12) {
		if (Contact_Object_A == BLOCK_CATEGORY || Contact_Object_A == HARDBLOCK_CATEGORY || Contact_Object_A == GBLOCK_CATEGORY) {
			gNumGunContacts--;
		}
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 13) {
		if (Contact_Object_B == BLOCK_CATEGORY || Contact_Object_B == HARDBLOCK_CATEGORY || Contact_Object_B == GBLOCK_CATEGORY)
			gNumLadderContacts--;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 13) {
		if (Contact_Object_A == BLOCK_CATEGORY || Contact_Object_A == HARDBLOCK_CATEGORY || Contact_Object_A == GBLOCK_CATEGORY) {
			gNumLadderContacts--;
		}
	}

	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 15) {
		if (Contact_Object_B == 1 || Contact_Object_B == 3 || Contact_Object_B == 4) {}
		//	gNumChipToCharactor--;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 15) {
		if (Contact_Object_A == 1 || Contact_Object_A == 3 || Contact_Object_A == 4){
		//	gNumChipToCharactor--;
		}
	}

	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == FALLINGBLADE_CONTACT) {
		gNumFallingBladeContacts--;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == FALLINGBLADE_CONTACT) {
		gNumFallingBladeContacts--;
	}

	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == TRAPBULLET_CONTACT) {
		if (Contact_Object_B == 3 || Contact_Object_B == 4)
			gNumTrapBulletContacts--;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == TRAPBULLET_CONTACT) {
		if (Contact_Object_A == 3 || Contact_Object_A == 4)
			gNumTrapBulletContacts--;
	}

	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 70) {
		//if (Contact_Object_B == 2)
			gNumShopManContacts--;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 70) {
		//if (Contact_Object_A == 2)
			gNumShopManContacts--;
	}
	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 77) {
		if (Contact_Object_B == BLOCK_CATEGORY || Contact_Object_B == HARDBLOCK_CATEGORY || Contact_Object_B == GBLOCK_CATEGORY)
			gNumGoldContacts--;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 77) {
		if (Contact_Object_A == BLOCK_CATEGORY || Contact_Object_A == HARDBLOCK_CATEGORY || Contact_Object_A == GBLOCK_CATEGORY)
			gNumGoldContacts--;
	}

	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 90) {
		if (Contact_Object_B == BLOCK_CATEGORY || Contact_Object_B == HARDBLOCK_CATEGORY || Contact_Object_B == GBLOCK_CATEGORY)
			gNumRedKeyContacts--;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 90) {
		if (Contact_Object_A == BLOCK_CATEGORY || Contact_Object_A == HARDBLOCK_CATEGORY || Contact_Object_A == GBLOCK_CATEGORY)
			gNumRedKeyContacts--;
	}

	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 91) {
		if (Contact_Object_B == BLOCK_CATEGORY || Contact_Object_B == HARDBLOCK_CATEGORY || Contact_Object_B == GBLOCK_CATEGORY)
			gNumBlueKeyContacts--;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 91) {
		if (Contact_Object_A == BLOCK_CATEGORY || Contact_Object_A == HARDBLOCK_CATEGORY || Contact_Object_A == GBLOCK_CATEGORY)
			gNumBlueKeyContacts--;
	}

	fixtureUserData = contact->GetFixtureA()->GetUserData();
	if ((int)fixtureUserData == 92) {
		if (Contact_Object_B == BLOCK_CATEGORY || Contact_Object_B == HARDBLOCK_CATEGORY || Contact_Object_B == GBLOCK_CATEGORY)
			gNumGreenKeyContacts--;
	}
	fixtureUserData = contact->GetFixtureB()->GetUserData();
	if ((int)fixtureUserData == 92) {
		if (Contact_Object_A == BLOCK_CATEGORY || Contact_Object_A == HARDBLOCK_CATEGORY || Contact_Object_A == GBLOCK_CATEGORY)
			gNumGreenKeyContacts--;
	}

}


// Game Over Scene으로 전환
void HelloWorld::GameOver(Ref* pSender) {
	auto pScene = GameOverScene::createScene();
	Director::getInstance()->replaceScene(pScene);
}