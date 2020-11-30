#pragma once
#include "cocos2d.h"

class ImageButton : public cocos2d::Sprite
{
public:

	static ImageButton *create(Sprite* button, Sprite* button_selected, const std::function<void(ImageButton*)> &onTriggered)
	{
		auto ret = new (std::nothrow) ImageButton();
		ret->setUserData((Sprite*)button);
		if (ret && ret->create(button, button_selected, onTriggered)) {
			ret->_onTriggered = onTriggered;

			ret->autorelease();

			return ret;
		}

		delete ret;
		return nullptr;
	}

	void setEnabled(bool enabled)
	{
		_enabled = enabled;
		if (_enabled) {
			this->setColor(cocos2d::Color3B::WHITE);
		}
		else {
			this->setColor(cocos2d::Color3B::GRAY);
		}
	}

private:
	ImageButton()
		: _onTriggered(nullptr)
		, _enabled(true)
	{
		auto listener = cocos2d::EventListenerTouchOneByOne::create();
		listener->setSwallowTouches(true);

		listener->onTouchBegan = CC_CALLBACK_2(ImageButton::onTouchBegan, this);
		listener->onTouchEnded = CC_CALLBACK_2(ImageButton::onTouchEnded, this);
		listener->onTouchCancelled = CC_CALLBACK_2(ImageButton::onTouchCancelled, this);

		_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	}

	bool touchHits(cocos2d::Touch  *touch)
	{
		auto hitPos = this->convertToNodeSpace(touch->getLocation());
		if (hitPos.x >= 0 && hitPos.y >= 0 && hitPos.x <= _contentSize.width && hitPos.y <= _contentSize.height) {
			return true;
		}
		return false;
	}

	bool onTouchBegan(cocos2d::Touch  *touch, cocos2d::Event  *event)
	{
		auto hits = touchHits(touch);
		if (hits) {
			scaleButtonTo(0.95f);
		}
		return hits;
	}

	void onTouchEnded(cocos2d::Touch  *touch, cocos2d::Event  *event)
	{
		if (_enabled) {
			auto hits = touchHits(touch);
			if (hits && _onTriggered) {
				_onTriggered(this);
			}
		}

		scaleButtonTo(1);
	}

	void onTouchCancelled(cocos2d::Touch  *touch, cocos2d::Event  *event)
	{
		scaleButtonTo(1);
	}

	void scaleButtonTo(float scale)
	{
		auto action = cocos2d::ScaleTo::create(0.05f, scale);
		action->setTag(10000);
		stopActionByTag(10000);
		runAction(action);
	}

	std::function<void(ImageButton*)> _onTriggered;

	bool _enabled;
};
