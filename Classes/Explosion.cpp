#include "Explosion.h"

Explosion::Explosion() {
}

Explosion::~Explosion() {
	
}

void Explosion::CreateExplosion(b2Body* body, float time, Layer* layer) {
	m_layer = layer;

	aabb.lowerBound = b2Vec2(0, 0);
	aabb.upperBound = b2Vec2(0, 0);
	Bombs.push_back(Bomb(body, time, body->GetPosition(), aabb));
	
}

bool Explosion::burst(b2World* world, float dt, Layer* layer) {
	std::list<Bomb>::iterator iter;
	for (iter = Bombs.begin(); iter != Bombs.end();) {
		if (iter->body->GetHp() <= 0) {
			Bombs.erase(iter++);
			continue;
		}
		Sprite* Bomb = static_cast<Sprite*>(iter->body->GetUserData());
		iter->aabb.lowerBound = iter->body_pos - b2Vec2(1, 1);
		iter->aabb.upperBound = iter->body_pos + b2Vec2(1, 1);

		if ((iter->time -= dt) <= 0) {
			/*
			m_Exposition = Bomb->getPosition();
			m_texture = Director::getInstance()->getTextureCache()->addImage("Images/effects/explosion.png");
			
			Animation* animation = Animation::create();
			animation->setDelayPerUnit(0.2f);

			for (int i = 0; i < 4; i++) {
				animation->addSpriteFrameWithTexture(m_texture, Rect(i * 40, 0, 40, 41));
			}

			m_explosion = Sprite::createWithTexture(m_texture, Rect(0, 0, 40, 41));
			m_explosion->setScale(1.5f);
			m_explosion->setAnchorPoint(Vec2(0.5, 0.2));
			m_explosion->setPosition(m_Exposition);
			layer->addChild(m_explosion, 3);

			Animate* animate = Animate::create(animation);
			Repeat* rep = Repeat::create(animate, 1);
			m_explosion->runAction(rep);*/

			boom(world, iter->body->GetPosition());

			layer->removeChild(Bomb, true);
			world->DestroyBody(iter->body);
			Bombs.erase(iter++);

			return true;
		}
		else ++iter;
	}
	return false;
}

void Explosion::boom(b2World* world, b2Vec2 position) {
	b2Fixture* fix;

	for (int i = 0; i < m_numRays; i++) {
		float angle = (i / (float)m_numRays) * 360 * DEGTORAD;
		b2Vec2 rayDir(sinf(angle), cosf(angle));

		Sprite* Chip_Sp = Sprite::create("n_Images/item/Bomb_Chip.png");
		m_layer->addChild(Chip_Sp);

		b2BodyDef bd;
		bd.type = b2_dynamicBody;
		bd.fixedRotation = true;
		bd.bullet = true;
		bd.linearDamping = 10;
		bd.gravityScale = 1;
		bd.position = position;
		bd.linearVelocity = m_blastPower * rayDir; 
		bd.userData = static_cast<Sprite*>(Chip_Sp);
		b2Body* body = world->CreateBody(&bd);
		body->SetHp(100);

		b2CircleShape circleShape;
		circleShape.m_radius = 0.05f;

		b2FixtureDef fd;
		fd.shape = &circleShape;
		fd.density = 60 / (float)m_numRays;
		fd.friction = 0;
		fd.restitution = 0.5f;
		fd.filter.categoryBits = 0x0003;
	//	fd.filter.maskBits = 0x000F;
		fd.filter.groupIndex = -1;
		fix = body->CreateFixture(&fd);
		fix->SetUserData((void*)15);
		
		chips.push_back(BombChip(body, true));
	}
}

void Explosion::cleanChips(b2World* world) {
	std::vector<BombChip>::iterator iter;
	for (iter = chips.begin(); iter != chips.end();) {
		b2Body* b = iter->body;
		Sprite* Chip_Sp = static_cast<Sprite*>(b->GetUserData());
		m_layer->removeChild(Chip_Sp, true);
		world->DestroyBody(b);
		iter = chips.erase(iter);
	}
	chips.clear();
}