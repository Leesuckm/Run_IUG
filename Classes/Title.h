#ifndef __TITLE_H__
#define __TITLE_H__

#include "cocos2d.h"
#include "HelloWorldScene.h"
#include <functional>

enum _StageNumber;

class TextButton;

class Title : public cocos2d::Scene 
{
private:
	Charactor_Data m_Charactor_Data;

public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(Title);

	void CreateBackground();
	void CreateMenu();

	void addTextButton(cocos2d::Vec2& pos, const std::string& btnText, const std::function<void(TextButton*)> & btnCallback);
	void GameScene(Ref* pSender);
	void OptionScene(Ref* pSender);
};

#endif// __TITLE_H__