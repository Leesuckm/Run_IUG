#include "LoadingScene.h"
#include "HelloWorldScene.h"

USING_NS_CC;

Charactor_Data g_Charactor_Data;


Scene* LoadingScene::createScene(Charactor_Data _init_Charactor){
	g_Charactor_Data = _init_Charactor;
	return LoadingScene::create();
}

bool LoadingScene::init() {
	if (!Scene::init()) {
		return false;
	}

	auto wlayer = LayerColor::create(Color4B(0, 0, 0, 255));
	this->addChild(wlayer);

	/*
	m_Sprite = Sprite::create("Images/Background/Game_Loading_Background_.png");
	m_Sprite->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	m_Sprite->setPosition(Vec2(0, 0));
	this->addChild(m_Sprite);*/

	
	m_loadingBar = ui::LoadingBar::create("Images/ui/Loading Bar.png");
	m_loadingBar->setPosition(Vec2(240, 100));
	m_loadingBar->setPercent(0);
	this->addChild(m_loadingBar);

	this->scheduleOnce(schedule_selector(LoadingScene::one, this), 0.2f);
	this->scheduleOnce(schedule_selector(LoadingScene::two, this), 0.4f);
	this->scheduleOnce(schedule_selector(LoadingScene::three, this), 0.6f);
	this->scheduleOnce(schedule_selector(LoadingScene::four, this), 0.8f);
	this->scheduleOnce(schedule_selector(LoadingScene::five, this), 1.0f);

	return true;
}

void LoadingScene::one(float dt) {
	m_loadingBar->setPercent(20);
}

void LoadingScene::two(float dt) {
	m_loadingBar->setPercent(40);
}

void LoadingScene::three(float dt) {
	m_loadingBar->setPercent(60);
}

void LoadingScene::four(float dt) {
	m_loadingBar->setPercent(80);
}

void LoadingScene::five(float dt) {
	m_loadingBar->setPercent(100);

	auto NextFloorScene = HelloWorld::createScene(g_Charactor_Data);
	Director::getInstance()->replaceScene(NextFloorScene);
}
