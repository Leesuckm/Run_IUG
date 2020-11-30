#ifndef __PAUSELAYER_H_
#define __PAUSELAYER_H_

#include "cocos2d.h"
#include "HelloWorldScene.h"

class PauseLayer : public cocos2d::LayerColor
{
private:
	Layer* m_layer;

	cocos2d::CCSize winSize;

	LayerColor* backLayer;
	
	Sprite* m_Pause_Background;

	PauseLayer* m_deletePause;
	Menu* m_Enable_Button;

public:
	virtual bool init(PauseLayer* _pause);

//	static cocos2d::Scene* createScene();
//	CREATE_FUNC(PauseLayer);

	PauseLayer(Layer* _layer, Menu* _button);

	void CreatePauseLayer();

	void PauseLayer::doResume(Ref* pSender);
	void PauseLayer::doTitle(Ref* pSender);
	void PauseLayer::doOption(Ref* pSender);
	void PauseLayer::doExit(Ref* pSender);
};

#endif // _PopLayer__