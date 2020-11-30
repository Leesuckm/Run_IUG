#ifndef __GameOverScene__ GO__
#define __GameOverScene__ GO__

#include "cocos2d.h"

class GameOverScene : public cocos2d::Scene {
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	CREATE_FUNC(GameOverScene);

	void doClose(Ref* pSender);
};

#endif