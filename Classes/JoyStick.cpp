#include "JoyStick.h"
bool JoyStick::isTouchCircle(Point pos, Point center, float radius) {
	float distanceX = pos.x - center.x;
	float distanceY = pos.y - center.y;
	return radius >= sqrt(pow(distanceX, 2) + pow(distanceY, 2));
}

JoyStick::JoyStick() {

}

JoyStick::~JoyStick() {
	Director::getInstance()->getScheduler()->unschedule("JoyStickUpdate", this);
}

bool JoyStick::init()
{
	if (!Layer::init())
		return false;


	m_winSize = Director::getInstance()->getWinSize();

	m_isTouch = false;

	// speed
	m_speed = 0.1;

	// touch show
	m_isTouchShow = false;

	// limit win size
	m_isLimmitWinSize = false;

	// Joystick - Standard Position
	m_centerPos = Vec2(125, 125);

	// Joystick - Back Sprite (Limit)
	m_Stick_area = Sprite::create("Images/JoystickSplitted.png");
	m_Stick_area->setScale(0.3f);
	m_Stick_area->setPosition(m_centerPos);
	this->addChild(m_Stick_area);


	// Joystick - Front Sprite (Controler)
	m_Stick_ball = Sprite::create("Images/LargeHandleFilledGrey.png");
	m_Stick_ball->setScale(0.3f);
	m_Stick_ball->setPosition(m_centerPos);
	this->addChild(m_Stick_ball);

	// Joystick - Controler Position
	m_controlerPos = m_centerPos;

	// Touch Enabled
	this->setTouchEnabled(true);
	this->setTouchMode(Touch::DispatchMode::ALL_AT_ONCE);

	// setSchedule Update
	cocos2d::Director::getInstance()->getScheduler()->schedule(
		std::bind(&JoyStick::update, this, std::placeholders::_1),
		this, 1 / 60.f, false, "JoyStickUpdate");

	return true;
}

void JoyStick::update(float dt) {
	m_Stick_ball->setPosition(m_controlerPos);
	/*
	if (m_isTouch) {
		float moveX = m_controlerPos.x - m_centerPos.x;
		float moveY = m_controlerPos.y - m_centerPos.y;

		float angle = this->getAngle(moveX, moveY);

		float degree = angle * RADTODEG;
		if (degree > -22.5 && degree < 22.5) {
			log("right");
		}
		if (degree > 22.5 && degree < 67.5) {
			log("right up");
		}
		if (degree > 67.5 && degree < 112.5) {
			log("up");
		}
		if (degree > 112.5 && degree < 157.5) {
			log("left up");
		}
		if (degree > 157.5 && degree < 180) {
			log("left");
		}
		if (degree > -180 && degree < -157.5) {
			log("left");
		}
		if (degree > -157.5 && degree < -112.5) {
			log("left down");
		}
		if (degree > -112.5 && degree < -67.5) {
			log("down");
		}
		if (degree > -67.5 && degree < -22.5) {
			log("right down");
		}
	}*/
}

float JoyStick::getAngle(float x, float y) {
	float distance = sqrt(pow(x, 2) + pow(y, 2));
	float angle = atan2(y, x);

	return angle;
}

void JoyStick::setBody(b2Body* _body)
{
	m_body = _body;
}

void JoyStick::setforce(float _force)
{
	m_speed = _force;
}


void JoyStick::setTouchShow(bool check)
{
	m_isTouchShow = check;

	if (m_isTouchShow)
		this->setVisible(false);
}

void JoyStick::setLimitScreen(bool check)
{
	m_isLimmitWinSize = check;
}

bool JoyStick::checkLimit()
{
	return m_isLimmitWinSize;
}

void JoyStick::onTouchesBegan(const std::vector<Touch*> &touches, Event* unused_event) {
	for (auto _touch : touches) {
		Touch* touch = _touch;
		if (m_isTouchShow) {
			m_centerPos = touch->getLocation();
			m_Stick_area->setPosition(m_centerPos);
			m_Stick_ball->setPosition(m_centerPos);
			this->setVisible(true);
		}

		if (isTouchCircle(touch->getLocation(), m_centerPos, m_Stick_area->getContentSize().width / 4)) {
			m_isTouch = true;
			m_controlerPos = touch->getLocation();
		}
	}
}


void JoyStick::onTouchesMoved(const std::vector<Touch*> &touches, Event* unused_event) {
	for (auto _touch : touches) {
		Touch* touch = _touch;
		if (m_isTouch) {
			float areaSize = m_Stick_area->getContentSize().width / 4;

			if (!(isTouchCircle(touch->getLocation(), m_centerPos, areaSize))) {
				Point touchPos = touch->getLocation();

				float distanceX = touchPos.x - m_centerPos.x;
				float distanceY = touchPos.y - m_centerPos.y;

				float distance = sqrt(pow(distanceX, 2) + pow(distanceY, 2));
				float angle = atan2(distanceY, distanceX);



				if (distance > areaSize) {
					distanceX = cos(angle) * areaSize;
					distanceY = sin(angle) * areaSize;

					touchPos.x = m_centerPos.x + distanceX;
					touchPos.y = m_centerPos.y + distanceY;
				}

				m_controlerPos = Vec2(touchPos);
			}

			else {
				m_controlerPos = touch->getLocation();
			}
		}
	}
}

void JoyStick::onTouchesEnded(const std::vector<Touch*> &touches, Event* unused_event)
{
	for (auto it : touches)
	{
		if (m_isTouch)
		{
			m_controlerPos = m_centerPos;
		}
		m_isTouch = false;

		if (m_isTouchShow)
			this->setVisible(false);
	}
}