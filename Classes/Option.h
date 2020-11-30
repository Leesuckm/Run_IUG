#ifndef __OPTION_H__
#define __OPTION_H__

#include "cocos2d.h"
#include <functional>

class TextButton;
class ImageButton;

class Option : public cocos2d::Scene {
private:
	float m_fBGM_Volume = 0.5f;
	float m_fSEF_Volume = 0.5f;


public:
	static cocos2d::Scene* createScene(std::string _beforeScene);
	virtual bool init();
	CREATE_FUNC(Option);

	void CreateBackground();
	void CreateButton();

	void addTextButton(cocos2d::Vec2& pos, const std::string& btnText, const std::function<void(TextButton*)> & btnCallback);
	void addImageButton(cocos2d::Vec2& pos, const std::string& btnImagePath, const std::string& s_btnImagePath, const std::string& name);

	void VOLUMEON(Ref* pSender);
	void VOLUMEOFF(Ref* pSender);
	void BGMPLUS(Ref* pSender);
	void BGMMINUS(Ref* pSender);
	void SEFPLUS(Ref* pSender);
	void SEFMINUS(Ref* pSender);
	void RETURN(Ref* pSender);
};



#endif //__OPTION_H__