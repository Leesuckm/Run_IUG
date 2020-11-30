#include "GameOverScene.h"
#include "Title.h"
#include "HelloWorldScene.h"

using namespace cocos2d;

Scene* GameOverScene::createScene() {
	return GameOverScene::create();
}

bool GameOverScene::init() {
	if (!Scene::init()) {
		return false;
	}

	auto wlayer = LayerColor::create(Color4B(255, 255, 0,255));
	this->addChild(wlayer);

	Label* pGameOverLabel = Label::createWithSystemFont("Game Over", "", 50);

	pGameOverLabel->setPosition(Vec2(140, 220));
	pGameOverLabel->setColor(Color3B::WHITE);
	this->addChild(pGameOverLabel);

	auto ExitTxt = MenuItemFont::create("³ª°¡±â", CC_CALLBACK_1(GameOverScene::doClose, this));
	ExitTxt->setColor(Color3B::BLACK);
	
	auto Exit = Menu::create(ExitTxt, nullptr);
	Exit->setPosition(Vec2(300, 50));
	this->addChild(Exit);

	return true;
	
}


void GameOverScene::doClose(Ref* pSender) {
	auto pScene = Title::createScene();
	Director::getInstance()->replaceScene(pScene);
}