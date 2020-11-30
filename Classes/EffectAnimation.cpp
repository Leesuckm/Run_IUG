#include "EffectAnimation.h"

EffectAnimation::EffectAnimation() {
	Gun_Crash_Effects = new std::list<Effecter>;
	Sword_Slice_Effects = new std::list<Effecter>;
	Bomb_poong_Effects = new std::list<Effecter>;
	GunTrap_Crash_Effects = new std::list<Effecter>;
}

EffectAnimation::~EffectAnimation() {
	delete  Gun_Crash_Effects;
	delete  Sword_Slice_Effects;
	delete	Bomb_poong_Effects;
	delete  GunTrap_Crash_Effects;
}

Sprite* EffectAnimation::CreateEffect(std::string& sPath, int max, Size size, Vec2 position, float animation_time, Layer* layer) {
	m_layer = layer;
	m_texture = Director::getInstance()->getTextureCache()->addImage(sPath);

	Animation* animation = Animation::create();
	animation->setDelayPerUnit(0.1f);

	for (int i = 0; i < max; i++) {
		animation->addSpriteFrameWithTexture(m_texture, Rect(i * size.width, 0, size.width, size.height));
	}

	Sprite* sprite = Sprite::createWithTexture(m_texture, Rect(0, 0, size.width, size.height));
	sprite->setPosition(position);
	sprite->setAnchorPoint(Vec2(0.5, 0.5));
	if (size.height == 15) {
		sprite->setFlippedX(true);
	}
	else {
		sprite->setFlippedX(false);
	}
	m_layer->addChild(sprite, 5);

	Animate* animate = Animate::create(animation);
	RepeatForever* rep = RepeatForever::create(animate);
	sprite->runAction(rep);

	if (sPath == "Images/effects/gun_crash_effect.png") {
		Gun_Crash_Effects->push_back(Effecter(sprite, animation_time));
	}
	else if (sPath == "Images/effects/slice_effect.png") {
		sprite->setScale(2.0f);
		Sword_Slice_Effects->push_back(Effecter(sprite, animation_time));
	}
	else if (sPath == "Images/effects/explosion.png") {
		Bomb_poong_Effects->push_back(Effecter(sprite, animation_time));
	}
	else if (sPath == "Images/effects/guntrap_crash_effect.png") {
		GunTrap_Crash_Effects->push_back(Effecter(sprite, animation_time));
	}

	return sprite;
}