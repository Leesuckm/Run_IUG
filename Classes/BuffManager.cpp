#include "BuffManager.h"
#include "HelloWorldScene.h"
#include "soundManager.h"

const std::string SEF_BUFFEFFECT_PATH = "Sounds/Buff.mp3";
const std::string SEF_DEBUFFEFFECT_PATH = "Sounds/Debuff.mp3";

BuffManager::BuffManager() {
}

BuffManager::BuffManager(Layer* _glayer, Layer* _layer, Size _size) {
	m_glayer = _glayer;
	m_layer = _layer;
	m_winSize = _size;
}

BuffManager::~BuffManager() {
}


void BuffManager::CreateBuffSlot() {
	m_nSlot_num = 0;
	Texture2D* texture = Director::getInstance()->getTextureCache()->addImage("Images/ui/Buff_Slot.png");

	m_SlotSprite[3];
	for (int i = 0; i < 3; i++) {
		m_SlotSprite[i] = Sprite::createWithTexture(texture);
		m_SlotSprite[i]->setPosition(Vec2(150 + (i * 40), m_winSize.height - 30));
		m_layer->addChild(m_SlotSprite[i], 5);
	}
}

void BuffManager::OpenTreasureChest(_KeyState _key) {
	int p;
	p = rand() % 10;
	switch (_key) {
	case REDKEY:
		if (p >= 0 && p <= 2) {
			slotin.buff_state = BS_HPMAXUP;
			slotin.debuff_state = DBS_NONE;
		}
		else if (p >= 3 && p <= 5) {
			slotin.debuff_state = DBS_HPMAXDOWN;
			slotin.buff_state = BS_NONE;
		}
		else if (p >= 6 && p <= 9) {
			slotin.debuff_state = DBS_LAVAUP;
			slotin.buff_state = BS_NONE;
		}
		break;
	case BLUEKEY:
		if (p >= 0 && p <= 2) {
			slotin.buff_state = BS_HASTE;
			slotin.debuff_state = DBS_NONE;
		}
		else if (p >= 3 && p <= 5) {
			slotin.buff_state = BS_JUMPUP;
			slotin.debuff_state = DBS_NONE;
		}
		else if (p >= 6 && p <= 9) {
			slotin.debuff_state = DBS_SLOW;
			slotin.buff_state = BS_NONE;
		}
		break;
	case GREENKEY:
		if (p >= 0 && p <= 5) {
			slotin.buff_state = BS_POWERUP;
			slotin.debuff_state = DBS_NONE;
		}
		else if (p >= 6 && p <= 9) {
			slotin.debuff_state = DBS_POWERDOWN;
			slotin.buff_state = BS_NONE;
		}
		break;
	default:
		slotin.buff_state = BS_NONE;
		slotin.debuff_state = DBS_NONE;
		break;
	}
}

void BuffManager::BuffInSlot(Charactor_Data* _charactor_data, b2Vec2 _position) {
	slotin.slot_number = _charactor_data->buff_num++;
	m_glayer->removeChildByTag(10);
	Sprite* BuffSprite;
	std::string sPath;
	ParticleSystem* particle_Effect;
	
	switch (slotin.buff_state) {
	case BS_HPMAXUP:
		BuffSprite = Sprite::create("Images/Buff/Heart_MaxUp.png");
		sPath = "Images/Buff/Red_Up_buff.plist";
		CSoundManager::getInstance()->playEffect(SEF_BUFFEFFECT_PATH);
		_charactor_data->status.n_MaxHp += 3;
		break;
	case BS_HASTE:
		BuffSprite = Sprite::create("Images/Buff/Haste.png");
		sPath = "Images/Buff/Blue_Up_buff.plist";
		CSoundManager::getInstance()->playEffect(SEF_BUFFEFFECT_PATH);
		_charactor_data->status.f_Speed = 2.0f;
		break;
	case BS_JUMPUP:
		BuffSprite = Sprite::create("Images/Buff/Jump_Up.png");
		sPath = "Images/Buff/Blue_Up_buff.plist";
		CSoundManager::getInstance()->playEffect(SEF_BUFFEFFECT_PATH);
		_charactor_data->status.f_JumpPower = 2.0f;
		break;
	case BS_POWERUP:
		BuffSprite = Sprite::create("Images/Buff/Power_Up.png");
		sPath = "Images/Buff/Green_Up_buff.plist";
		CSoundManager::getInstance()->playEffect(SEF_BUFFEFFECT_PATH);
		_charactor_data->status.f_power = 2.0f;
		break;
	default:
		//sPath = "Images/Buff/None_buff.plist";
		break;
	}

	switch (slotin.debuff_state) {
	case DBS_HPMAXDOWN:
		BuffSprite = Sprite::create("Images/Buff/Heart_MaxDown.png");
		sPath = "Images/Buff/Red_Down_debuff.plist";
		CSoundManager::getInstance()->playEffect(SEF_DEBUFFEFFECT_PATH);
		_charactor_data->status.n_MaxHp -= 3;
		break;
	case DBS_SLOW:
		BuffSprite = Sprite::create("Images/Buff/Slow.png");
		sPath = "Images/Buff/Blue_Down_debuff.plist";
		CSoundManager::getInstance()->playEffect(SEF_DEBUFFEFFECT_PATH);
		_charactor_data->status.f_Speed = -2;
		break;
	case DBS_JUMPDOWN:
		BuffSprite = Sprite::create("Images/Buff/Slow.png");
		sPath = "Images/Buff/Blue_Down_debuff.plist";
		CSoundManager::getInstance()->playEffect(SEF_DEBUFFEFFECT_PATH);
		_charactor_data->status.f_JumpPower = -2;
		break;
	case DBS_POWERDOWN:
		BuffSprite = Sprite::create("Images/Buff/Power_Down.png");
		sPath = "Images/Buff/Green_Down_debuff.plist";
		CSoundManager::getInstance()->playEffect(SEF_DEBUFFEFFECT_PATH);
		_charactor_data->status.f_power = -2;
		break;
	case DBS_LAVAUP:
		BuffSprite = Sprite::create("Images/Buff/LavaUp.png");
		sPath = "Images/Buff/Red_Down_debuff.plist";
		CSoundManager::getInstance()->playEffect(SEF_DEBUFFEFFECT_PATH);
		_charactor_data->status.b_LavaDebuff = true;
		break;
	default:
		//sPath = "Images/Buff/None_buff.plist";
		break;
	}
	

	particle_Effect = ParticleSystemQuad::create(sPath);
	particle_Effect->setPosition(Vec2(_position.x * 32, _position.y * 32));
	particle_Effect->setScale(0.2f);
	particle_Effect->setTag(10);
	//Red_Up->setDuration(2.0f);
	particle_Effect->setAutoRemoveOnFinish(false);
	m_glayer->addChild(particle_Effect, 100);

	BuffSprite->setPosition(Vec2(240, 160));
	m_layer->addChild(BuffSprite, 6);
	slotin.buff_Sprite = BuffSprite;

	_charactor_data->buff.push_back(slotin);
	int n_min = _charactor_data->buff.front().slot_number;
	
	
	if (_charactor_data->buff.size() > 3) {
		

		std::vector<BuffSlot>::iterator iter;
		for (iter = _charactor_data->buff.begin(); iter != _charactor_data->buff.end();) {
			if (n_min >= iter->slot_number) {
				n_min = iter->slot_number;

				switch (iter->buff_state) {
				case BS_HPMAXUP:
					_charactor_data->status.n_MaxHp = 15;
					break;
				case BS_HASTE:
					_charactor_data->status.f_Speed = 0;
					break;
				case BS_JUMPUP:
					_charactor_data->status.f_JumpPower = 0;
					break;
				case BS_POWERUP:
					_charactor_data->status.f_power = 0;
					break;
				}

				switch (iter->debuff_state) {
				case DBS_HPMAXDOWN:
					_charactor_data->status.n_MaxHp = 15;
					break;
				case DBS_SLOW:
					_charactor_data->status.f_Speed = 0;
					break;
				case DBS_JUMPDOWN:
					_charactor_data->status.f_JumpPower = 0;
					break;
				case DBS_POWERDOWN:
					_charactor_data->status.f_power = 0;
					break;
				case DBS_LAVAUP:
					_charactor_data->status.b_LavaDebuff = true;
					break;
				}

				Sprite* frontSprite = _charactor_data->buff.front().buff_Sprite;
				Sprite* secondSprite = _charactor_data->buff.at(1).buff_Sprite;
				Sprite* thirdSprite = _charactor_data->buff.at(2).buff_Sprite;

				

				thirdSprite->setPosition(secondSprite->getPosition());
				secondSprite->setPosition(frontSprite->getPosition());
				m_layer->removeChild(frontSprite, true);
				iter = _charactor_data->buff.erase(iter);
			}
			else {
				iter++;
			}
		}
	}

	if (_charactor_data->buff.size() == 1) {
		m_Sprites_slot.push(BuffSprite);
		MoveTo* BuffMove = MoveTo::create(2, m_SlotSprite[0]->getPosition());
		EaseOut* easeout = EaseOut::create(BuffMove, 2.0f);
		BuffSprite->runAction(easeout);
	}

	else if (_charactor_data->buff.size() == 2) {
		m_Sprites_slot.push(BuffSprite);
		MoveTo* BuffMove = MoveTo::create(2, m_SlotSprite[1]->getPosition());
		EaseOut* easeout = EaseOut::create(BuffMove, 2.0f);
		BuffSprite->runAction(easeout);
	}

	else if (_charactor_data->buff.size() >= 3) {
		m_Sprites_slot.push(BuffSprite);
		MoveTo* BuffMove = MoveTo::create(2, m_SlotSprite[2]->getPosition());
		EaseOut* easeout = EaseOut::create(BuffMove, 2.0f);
		BuffSprite->runAction(easeout);
	}
}

void BuffManager::CharactorInitBuff(Charactor_Data* _charactor_data) {

	for (int i = 0; i < _charactor_data->buff.size(); i++) {
		Sprite* BuffSprite;
		switch (_charactor_data->buff.at(i).buff_state) {
		case BS_HPMAXUP:
			BuffSprite = Sprite::create("Images/Buff/Heart_MaxUp.png");
			break;
		case BS_HASTE:
			BuffSprite = Sprite::create("Images/Buff/Haste.png");
			break;
		case BS_JUMPUP:
			BuffSprite = Sprite::create("Images/Buff/Jump_Up.png");
			break;
		case BS_POWERUP:
			BuffSprite = Sprite::create("Images/Buff/Power_Up.png");
			break;
		default:
			break;
		}

		switch (_charactor_data->buff.at(i).debuff_state) {
		case DBS_HPMAXDOWN:
			BuffSprite = Sprite::create("Images/Buff/Heart_MaxDown.png");
			break;
		case DBS_SLOW:
			BuffSprite = Sprite::create("Images/Buff/Slow.png");
			break;
		case DBS_JUMPDOWN:
			BuffSprite = Sprite::create("Images/Buff/Jump_Down.png");
			break;
		case DBS_POWERDOWN:
			BuffSprite = Sprite::create("Images/Buff/Power_Down.png");
			break;
		case DBS_LAVAUP:
			BuffSprite = Sprite::create("Images/Buff/LavaUp.png");
			break;
		default:
			break;
		}

		if (BuffSprite != nullptr) {
			if (i == 0) {
				BuffSprite->setPosition(m_SlotSprite[0]->getPosition());
				m_layer->addChild(BuffSprite, 6);
				_charactor_data->buff.at(0).buff_Sprite = BuffSprite;
			}
			else if (i == 1) {
				BuffSprite->setPosition(m_SlotSprite[1]->getPosition());
				m_layer->addChild(BuffSprite, 6);
				_charactor_data->buff.at(1).buff_Sprite = BuffSprite;
			}
			else if (i == 2) {
				BuffSprite->setPosition(m_SlotSprite[2]->getPosition());
				m_layer->addChild(BuffSprite, 6);
				_charactor_data->buff.at(2).buff_Sprite = BuffSprite;
			}
			
		}
	}
}

void BuffManager::BuffApplication(Charactor_Data* _charactor_data) {
	for (int i = 0; i < _charactor_data->buff.size(); i++) {
		switch (_charactor_data->buff.at(i).buff_state) {
			case BS_HPMAXUP:
				break;
			case BS_HASTE:
				break;
			case BS_JUMPUP:
				break;
			case BS_POWERUP:
				break;
		}
	}
}