#include "PauseLayer.h"
#include "Title.h"
#include "Option.h"
#include "HelloWorldScene.h"

using namespace cocos2d;

PauseLayer::PauseLayer(Layer* _layer, Menu* _button) {
	m_layer = _layer;
	m_Enable_Button = _button;
}

/*
Scene* PauseLayer::createScene()
{
	Scene *scene = Scene::create();

	return scene;
}*/



bool PauseLayer::init(PauseLayer* _pause)
{
	if (!LayerColor::initWithColor(ccc4(0, 0, 0, 0)))  //투명하게
	{
		return false;
	}
	winSize = Director::sharedDirector()->getWinSize();

	CreatePauseLayer();
	m_deletePause = _pause;

	return true;
}

void PauseLayer::CreatePauseLayer() {
	//backLayer추가
	backLayer = CCLayerColor::create(ccc4(0, 0, 0, 125), winSize.width, winSize.height);
	backLayer->setAnchorPoint(ccp(0, 0));
	backLayer->setPosition(ccp(0, 0));
	m_layer->addChild(backLayer);

	m_Pause_Background = Sprite::create("Images/ui/PauseBackground.png");
	m_Pause_Background->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_Pause_Background->setPosition(Vec2(winSize.width / 2, winSize.height / 2));
	backLayer->addChild(m_Pause_Background);

	TTFConfig ttfConfig("fonts/slkscr.ttf", 27);

	Label* Pauselabel = Label::createWithTTF(ttfConfig, "Pause");
	Pauselabel->setPosition(Vec2(240, 233));
	backLayer->addChild(Pauselabel);

	MenuItemImage* ResumeButton = MenuItemImage::create("Images/ui/Resume_button.png", "Images/ui/Resume_button_selected.png", CC_CALLBACK_1(PauseLayer::doResume, this));
	MenuItemImage* TitleButton = MenuItemImage::create("Images/ui/Title_button.png", "Images/ui/Title_button_selected.png", CC_CALLBACK_1(PauseLayer::doTitle, this));
	MenuItemImage* OptionButton = MenuItemImage::create("Images/ui/Option_button.png", "Images/ui/Option_button_selected.png", CC_CALLBACK_1(PauseLayer::doOption, this));
	MenuItemImage* ExitButton = MenuItemImage::create("Images/ui/Exit_button.png", "Images/ui/Exit_button_selected.png", CC_CALLBACK_1(PauseLayer::doExit, this));

	Menu* pButton = Menu::create(ResumeButton, TitleButton, OptionButton, ExitButton, nullptr);
	pButton->alignItemsVertically();
	pButton->setPosition(Vec2(240, 130));
	backLayer->addChild(pButton);
}

void PauseLayer::doResume(Ref* pSender)
{
	m_layer->removeChild(backLayer, true);

	Director::sharedDirector()->resume();
	m_Enable_Button->setEnabled(true);

	delete m_deletePause;
	//this->removeFromParentAndCleanup(true);
}

void PauseLayer::doTitle(Ref* pSender) {
	m_layer->removeChild(backLayer, true);

	Director::sharedDirector()->resume();
	m_Enable_Button->setEnabled(true);
	delete m_deletePause;

	auto pTitle = Title::createScene();
	Director::getInstance()->replaceScene(pTitle);
}

void PauseLayer::doOption(Ref* pSender) {
	auto pOption = Option::createScene("Game");
	Director::getInstance()->pushScene(pOption);
}

void PauseLayer::doExit(Ref* pSender) {
	Director::sharedDirector()->end();
}