#pragma once
#include "HelloWorldScene.h"

class JoyStick : public Layer {
private:
	Layer* m_layer;

	Sprite* m_Stick_area;
	Sprite* m_Stick_ball;
	b2Body* m_body;

	Vec2 m_controlerPos;
	Vec2 m_centerPos;

	float m_speed;
	bool m_isTouch;
	bool m_isTouchShow;
	bool m_isLimmitWinSize;
	Size m_winSize;

public:
	JoyStick();
	~JoyStick();

	virtual bool init();

	void update(float dt);
	void setBody(b2Body* _body);
	void setforce(float _force);
	void setTouchShow(bool _check);
	void setLimitScreen(bool _check);

	bool checkLimit();

	bool getisTouch() { return m_isTouch; };
	float getAngle(float x, float y);
	Vec2 getControlerPos() { return m_controlerPos; };
	Vec2 getCenterPos() { return m_centerPos; };

	bool isTouchCircle(Point pos, Point center, float radius);

	virtual void onTouchesBegan(const std::vector<Touch*> &touches, Event* unused_event) override;
	virtual void onTouchesMoved(const std::vector<Touch*> &touches, Event* unused_event) override;
	virtual void onTouchesEnded(const std::vector<Touch*> &touches, Event* unused_event) override;

	CREATE_FUNC(JoyStick);
};