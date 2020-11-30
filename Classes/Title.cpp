#include "Title.h"
#include "HelloWorldScene.h"
#include "Option.h"
#include "textButton.h"
#include "soundManager.h"

const std::string BGM_Background_1_PATH = "Sounds/Background_Sound.mp3";
const std::string BGM_Charactor_Step_PATH = "Sounds/Charactor_step.mp3";
const std::string SEF_Test_PATH = "Sounds/Test_SEF.mp3";

USING_NS_CC;

Scene* Title::createScene() {
	return Title::create();
}

bool Title::init() {
	if (!Scene::init()) {
		return false;
	}

	m_Charactor_Data.StageNumber = STAGE_1_1;
	m_Charactor_Data.n_Hp_Count = Heart_Count;
	m_Charactor_Data.n_Gold_Count = GOLD;
	m_Charactor_Data.n_Bomb_Count = Bomb_Count;
	m_Charactor_Data.n_Ladder_Count = Ladder_Count;
	m_Charactor_Data.item = IS_NONE;
	m_Charactor_Data.bStage_blind = true;

	auto wlayer = LayerColor::create(Color4B(0, 0, 0, 255));
	this->addChild(wlayer);

	CreateBackground();
	CreateMenu();

	CSoundManager::getInstance()->preloadAudioFile(BGM_Background_1_PATH);
	CSoundManager::getInstance()->preloadAudioFile(BGM_Charactor_Step_PATH);

	CSoundManager::getInstance()->playBgm(BGM_Background_1_PATH, true, false, 0.5f);

	return true;
}

void Title::CreateBackground() {
	Sprite* BackSprite = Sprite::create("Images/Background/Background.png");
	BackSprite->setAnchorPoint(Vec2(0, 0));
	BackSprite->setPosition(Vec2::ZERO);
	this->addChild(BackSprite);
}

void Title::CreateMenu() {
	addTextButton(Vec2(200, 140), "START", CC_CALLBACK_1(Title::GameScene, this));
	addTextButton(Vec2(200, 100), "OPTION", CC_CALLBACK_1(Title::OptionScene, this));
}

void Title::addTextButton(Vec2& pos, const std::string& btnText, const std::function<void(TextButton*)>& btnCallback) {
	if (btnCallback) {
		auto textButton = TextButton::create(btnText, btnCallback);
		textButton->setPosition(pos);
		textButton->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
		addChild(textButton);
	}
}

void Title::GameScene(Ref* pSender) {
	auto pGameScene = HelloWorld::createScene(m_Charactor_Data);
	Director::getInstance()->replaceScene(pGameScene);
	CSoundManager::getInstance()->playEffect(SEF_Test_PATH);
}

void Title::OptionScene(Ref* pSender) {
	auto pOptionScene = Option::createScene("Title");
	Director::getInstance()->pushScene(pOptionScene);
	CSoundManager::getInstance()->playEffect(SEF_Test_PATH);
}