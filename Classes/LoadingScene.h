#ifndef __LoadingScene_H__
#define __LoadingScene_H__

#include "cocos2d.h"
#include "ui/cocosGUI.h"
#include "HelloWorldScene.h"
#include <functional>

USING_NS_CC;

#define ReplaceScene(X) (CCDirector::sharedDirector()->replaceScene(LoadingScene<X>::scene()))
#define PushScene(X) (CCDirector::sharedDirector()->pushScene(LoadingScene<X>::scene()))
#define ReplaceSceneParam(X, P) (CCDirector::sharedDirector()->replaceScene(LoadingScene<X, decltype(P)>::scene(P)))
#define PushSceneParam(X, P) (CCDirector::sharedDirector()->pushScene(LoadingScene<X, decltype(P)>::scene(P)))

//template<typename P>
class LoadingScene : public cocos2d::Scene 
{
private:
	virtual void onEnterTransitionDidFinish() {
		
	}
	_StageNumber m_StageNumber;
	ui::LoadingBar* m_loadingBar;
	Sprite* m_Sprite;

public:

	static Scene* createScene(Charactor_Data _init_Charactor);
	virtual bool init();
	CREATE_FUNC(LoadingScene);

	void one(float dt);
	void two(float dt);
	void three(float dt);
	void four(float dt);
	void five(float dt);
};

#endif// __LoadingScene_H__