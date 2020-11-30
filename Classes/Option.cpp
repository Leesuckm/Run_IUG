#include "Option.h"
#include "GameUI.h"
#include "HelloWorldScene.h"
#include "textButton.h"
#include "imageButton.h"
#include "cocos2d.h"
#include "Title.h"
#include "soundManager.h"

USING_NS_CC;

const std::string BGM_Background_1_PATH = "Sounds/Background_Sound.mp3";
const std::string BGM_Charactor_Step_PATH = "Sounds/Charactor_step.mp3";
const std::string SEF_Test_PATH = "Sounds/Test_SEF.mp3";

static std::string g_beforeScene;

Scene* Option::createScene(std::string _beforeScene) {
	g_beforeScene = _beforeScene;
	return Option::create();
}

bool Option::init() {
	if (!Scene::init()) {
		return false;
	}

	auto wlayer = LayerColor::create(Color4B(0, 0, 0, 255));
	this->addChild(wlayer);

	//	m_Size = wlayer->getContentSize();

	CreateBackground();
	CreateButton();

	CSoundManager::getInstance()->preloadAudioFile(BGM_Background_1_PATH);
	CSoundManager::getInstance()->preloadAudioFile(BGM_Charactor_Step_PATH);

	return true;
}

void Option::CreateBackground() {
	Sprite* BackSprite = Sprite::create("Images/Background/Background.png");
	BackSprite->setAnchorPoint(Vec2(0, 0));
	BackSprite->setPosition(Vec2::ZERO);
	this->addChild(BackSprite);

	std::string *pSound[3];

	std::string Sound = "SOUND";
	std::string BGM = "BGM";
	std::string EFFECT_Sound = "EFFECT SOUND";
	
	pSound[0] = &Sound;
	pSound[1] = &BGM;
	pSound[2] = &EFFECT_Sound;

	TTFConfig ttfConfig("fonts/slkscr.ttf", 30);

	for (int i = 0; i < 3; i++) {
		
		Label* label = Label::createWithTTF(ttfConfig, *pSound[i]);
		label->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		label->setPosition(Vec2(100, 110 - i * 40));
		this->addChild(label);
	}
}

void Option::CreateButton() {
	Sprite* PlusButton = Sprite::create("");
	Sprite* PlusButton_selected = Sprite::create("btn-play-selected.png");

	//addTextButton(Vec2(100, 100), "btn-play-normal.png", CC_CALLBACK_1(Option::BGMPLUS, this));
	addImageButton(Vec2(360, 125), "Images/ui/volume_ON.png", "Images/ui/volume_ON_selected.png", "VOLUMEON");
	addImageButton(Vec2(430, 125), "Images/ui/volume_OFF.png", "Images/ui/volume_OFF_selected.png", "VOLUMEOFF");
	addImageButton(Vec2(360, 85), "Images/ui/PlusButton.png", "Images/ui/PlusButton_selected.png", "BGMPLUS");
	addImageButton(Vec2(430, 85), "Images/ui/MinusButton.png", "Images/ui/MinusButton_selected.png", "BGMMINUS");
	addImageButton(Vec2(360, 45), "Images/ui/PlusButton.png", "Images/ui/PlusButton_selected.png", "SEFPLUS");
	addImageButton(Vec2(430, 45), "Images/ui/MinusButton.png", "Images/ui/MinusButton_selected.png", "SEFMINUS");
	addImageButton(Vec2(450, 290), "Images/ui/ReturnButton.png", "Images/ui/ReturnButton_selected.png", "RETURN");

}

void Option::addTextButton(Vec2& pos, const std::string& btnText, const std::function<void(TextButton*)>& btnCallback) {
	if (btnCallback) {
		auto textButton = TextButton::create(btnText, btnCallback);
		textButton->setPosition(pos);
		textButton->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
		addChild(textButton);
	}
}

void Option::addImageButton(cocos2d::Vec2& pos, const std::string& btnImagePath, const std::string& s_btnImagePath, const std::string& name) {
	MenuItemImage* menuItem;
	if (name == "BGMPLUS") {
		menuItem = MenuItemImage::create(btnImagePath, s_btnImagePath, CC_CALLBACK_1(Option::BGMPLUS, this));
		menuItem->setScale(0.5f);
	}
	else if (name == "BGMMINUS") {
		menuItem = MenuItemImage::create(btnImagePath, s_btnImagePath, CC_CALLBACK_1(Option::BGMMINUS, this));
		menuItem->setScale(0.5f);
	}
	else if (name == "SEFPLUS") {
		menuItem = MenuItemImage::create(btnImagePath, s_btnImagePath, CC_CALLBACK_1(Option::SEFPLUS, this));
		menuItem->setScale(0.5f);
	}
	else if (name == "SEFMINUS") {
		menuItem = MenuItemImage::create(btnImagePath, s_btnImagePath, CC_CALLBACK_1(Option::SEFMINUS, this));
		menuItem->setScale(0.5f);
	}
	else if (name == "VOLUMEON") {
		menuItem = MenuItemImage::create(btnImagePath, s_btnImagePath, CC_CALLBACK_1(Option::VOLUMEON, this));
		menuItem->setScale(0.5f);
	}
	else if (name == "VOLUMEOFF") {
		menuItem = MenuItemImage::create(btnImagePath, s_btnImagePath, CC_CALLBACK_1(Option::VOLUMEOFF, this));
		menuItem->setScale(0.5f);
	}
	else if (name == "RETURN") {
		menuItem = MenuItemImage::create(btnImagePath, s_btnImagePath, CC_CALLBACK_1(Option::RETURN, this));
		menuItem->setScale(2.0f);
	}
	Menu* menu = Menu::create(menuItem, nullptr);
	menu->setPosition(pos);
	this->addChild(menu);
}

void Option::BGMPLUS(Ref* pSender) {
	m_fBGM_Volume += 0.1;
	if (m_fBGM_Volume > 1) m_fBGM_Volume = 1;
	CSoundManager::getInstance()->setBgmVolume(m_fBGM_Volume);
}

void Option::BGMMINUS(Ref* pSender) {
	m_fBGM_Volume -= 0.1;
	if (m_fBGM_Volume < 0.1) m_fBGM_Volume = 0.1;
	CSoundManager::getInstance()->setBgmVolume(m_fBGM_Volume);
}

void Option::SEFPLUS(Ref* pSender) {
	m_fSEF_Volume += 0.1;
	if (m_fSEF_Volume > 1) m_fSEF_Volume = 1;
	CSoundManager::getInstance()->setEffectVolume(m_fSEF_Volume);
	CSoundManager::getInstance()->playEffect(SEF_Test_PATH);
}

void Option::SEFMINUS(Ref* pSender) {
	m_fSEF_Volume -= 0.1;
	if (m_fSEF_Volume < 0.1) m_fSEF_Volume = 0.1;
	CSoundManager::getInstance()->setEffectVolume(m_fSEF_Volume);
	CSoundManager::getInstance()->playEffect(SEF_Test_PATH);
}

void Option::VOLUMEON(Ref* pSender) {
	CSoundManager::getInstance()->playBgm(BGM_Background_1_PATH);
	CSoundManager::getInstance()->setBgmVolume(0.5f);
	CSoundManager::getInstance()->setEffectVolume(0.5f);
	CSoundManager::getInstance()->setEffectOn(true);
}

void Option::VOLUMEOFF(Ref* pSender) {
	CSoundManager::getInstance()->stopAllBgms();
	CSoundManager::getInstance()->setBgmVolume(0);
	CSoundManager::getInstance()->setEffectVolume(0);
	CSoundManager::getInstance()->setEffectOn(false);
}

void Option::RETURN(Ref* pSender) {

	if (g_beforeScene == "Title") {
		/*
		auto pTitle = Title::createScene();
		Director::getInstance()->replaceScene(pTitle);*/
		Director::getInstance()->popScene();
		CSoundManager::getInstance()->playEffect(SEF_Test_PATH);
	}
	else if (g_beforeScene == "Game") {
		Director::getInstance()->popScene();
	}
	
}