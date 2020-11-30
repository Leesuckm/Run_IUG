#include "GameUI.h"
#include "PauseLayer.h"

GameUI::GameUI() {
	m_bCaution_OnOff = true;
	m_bCaution_Turnoff = true;
	m_fCaution_time = 5;
}

GameUI::GameUI(b2World* _world, Layer* _layer, b2Body* CharactorBody, Charactor_Data* Charactor_data) {
	m_world = _world;
	m_layer = _layer;
	m_Charactorbody = CharactorBody;
	m_Charactor_data = Charactor_data;

	m_bCaution_OnOff = true;
	m_bCaution_Turnoff = true;
	m_fCaution_time = 5;

	m_layerSize = m_layer->getContentSize();
}

GameUI::~GameUI() {
}

void GameUI::CreateUI() {
	this->CreateCautionMessage();
	this->CreateGoldUi(0);
}

void GameUI::CreatePauseButton() {
	m_pPause_Image = MenuItemImage::create("Images/ui/Pausebutton.png", "Images/ui/Pausebutton_selected.png", CC_CALLBACK_1(GameUI::doPause, this));
	m_pPause_Image->setScale(0.5f);
	m_pPause = Menu::create(m_pPause_Image, nullptr);
	m_pPause->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_pPause->setPosition(Vec2(450, 290));
	m_layer->addChild(m_pPause, 5);
}

TTFConfig ttfConfig("fonts/slkscr.ttf", 15);

Sprite* GameUI::CreateCautionMessage() { // Lava 출현 시 Caution Message 출력
	Texture2D* texture = Director::getInstance()->getTextureCache()->addImage("Images/ui/Caution.png");

	Animation* animation = Animation::create();
	animation->setDelayPerUnit(0.4f);

	for (int i = 0; i < 8; i++) {
		animation->addSpriteFrameWithTexture(texture, Rect(i * 480, 0, 480, 320));
	}

	m_Caution_Sprite = Sprite::createWithTexture(texture, Rect(0, 0, 480, 320));
	m_Caution_Sprite->setAnchorPoint(Vec2(0, 0));
	m_Caution_Sprite->setPosition(Vec2(0, 0));
	m_layer->addChild(m_Caution_Sprite, 4);

	Animate* animate = Animate::create(animation);
	RepeatForever* rep = RepeatForever::create(animate);
	m_Caution_Sprite->runAction(rep);

	return m_Caution_Sprite;
}

Sprite* GameUI::CreateCautionBackGround() { // Lava 출현 시 메시지 출력과 동시에 Background 전체가 깜빡이는 듯한 효과
	Texture2D* texture = Director::getInstance()->getTextureCache()->addImage("Images/ui/Caution_background.png");

	Animation* animation = Animation::create();
	animation->setDelayPerUnit(2.0f);

	for (int i = 0; i < 4; i++) {
		animation->addSpriteFrameWithTexture(texture, Rect(i * 480, 0, 480, 320));
	}

	m_Caution_BackSprite = Sprite::createWithTexture(texture, Rect(0, 0, 480, 320));
	m_Caution_BackSprite->setAnchorPoint(Vec2(0, 0));
	m_Caution_BackSprite->setPosition(Vec2(0, 0));
	m_layer->addChild(m_Caution_BackSprite, 4);

	Animate* animate = Animate::create(animation);
	RepeatForever* rep = RepeatForever::create(animate);
	m_Caution_Sprite->runAction(rep);

	return m_Caution_BackSprite;
}

void GameUI::CharactorStateSprite() {
	m_nHeartCount = m_Charactor_data->n_Hp_Count;
	m_sHeart = CreateStateSprite("Images/Heart.png", Vec2(0, 310), Size(32, 32));

	m_nBombCount = m_Charactor_data->n_Bomb_Count;
	m_sBomb = CreateStateSprite("Images/Bomb.png", Vec2(0, 280), Size(34, 32));

	m_nLadderCount = m_Charactor_data->n_Ladder_Count;
	m_sLadder = CreateStateSprite("Images/Ladder.png", Vec2(0, 250), Size(32, 32));

	this->Heartmanager(m_Charactor_data);
	this->Bombmanager(m_nBombCount);
	this->Laddermanager(m_nLadderCount);
}

Sprite* GameUI::CreateStateSprite(std::string sPath, Vec2 pos, Size size) {
	Sprite* StateSprite;

	auto texture = Director::getInstance()->getTextureCache()->addImage(sPath);
	StateSprite = Sprite::createWithTexture(texture, Rect(0, 0, size.width, size.height));
	StateSprite->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	StateSprite->setScale(0.7f);
	StateSprite->setPosition(pos);
	m_layer->addChild(StateSprite, 5);

	return StateSprite;
}

void GameUI::Heartmanager(Charactor_Data* _charactor_data) {
	if (_charactor_data->n_Hp_Count > _charactor_data->status.n_MaxHp) {
		_charactor_data->n_Hp_Count = _charactor_data->status.n_MaxHp;
	}

	m_nHeartCount = _charactor_data->n_Hp_Count;
	m_Charactorbody->SetHp(_charactor_data->n_Hp_Count);

	std::string Heart = std::to_string(m_Charactorbody->GetHp());
	m_layer->removeChild(m_HeartLabel, true);
	m_HeartLabel = Label::createWithSystemFont(Heart, "", 23);
	m_HeartLabel->setPosition(m_sHeart->getPosition() + Vec2(40, -10));
	m_HeartLabel->setColor(Color3B(255, 255, 255));
	m_layer->addChild(m_HeartLabel, 5);

	m_layer->removeChild(m_HeartMaxLabel, true);
	std::string maxHeart = std::to_string(_charactor_data->status.n_MaxHp);
	m_HeartMaxLabel = Label::createWithSystemFont("/" + maxHeart, "", 23);
	m_HeartMaxLabel->setPosition(m_sHeart->getPosition() + Vec2(70, -10));
	m_HeartMaxLabel->setColor(Color3B(255, 255, 255));
	m_layer->addChild(m_HeartMaxLabel, 5);
}

void GameUI::Bombmanager(int bombcount) {
	std::string Bomb = std::to_string(bombcount);
	m_layer->removeChild(m_BombLabel, true);
	m_BombLabel = Label::createWithSystemFont(Bomb, "", 23);
	m_BombLabel->setPosition(m_sBomb->getPosition() + Vec2(40, -10));
	m_BombLabel->setColor(Color3B(255, 255, 255));
	m_layer->addChild(m_BombLabel, 5);
}

void GameUI::Laddermanager(int laddercount) {
	std::string Ladder = std::to_string(laddercount);
	m_layer->removeChild(m_LadderLabel, true);
	m_LadderLabel = Label::createWithSystemFont(Ladder, "", 23);
	m_LadderLabel->setPosition(m_sLadder->getPosition() + Vec2(40, -10));
	m_LadderLabel->setColor(Color3B(255, 255, 255));
	m_layer->addChild(m_LadderLabel, 5);
}


Sprite* GameUI::CreateGoldUi(int _nGold) {
	m_Gold_Sprite = Sprite::create("n_Images/item/Gold.png");
	m_Gold_Sprite->setPosition(Vec2(m_layerSize.width - 150, m_layerSize.height - 15));
	m_Gold_Sprite->setScale(1.5f);
	m_layer->addChild(m_Gold_Sprite, 4);

	m_Gold_Num = std::to_string(_nGold);
	
	m_Gold_Label = Label::createWithTTF(ttfConfig, m_Gold_Num);
	m_Gold_Label->setPosition(Vec2(40, 5));
	m_Gold_Label->setColor(Color3B::YELLOW);
	m_Gold_Sprite->addChild(m_Gold_Label, 5);

	return m_Gold_Sprite;
}

void GameUI::GoldManager(Layer* layer, int _nGold) {
	m_Gold_Num = std::to_string(_nGold);
	m_Gold_Sprite->removeChild(m_Gold_Label, true);
	m_Gold_Label = Label::createWithTTF(ttfConfig, m_Gold_Num);
	m_Gold_Label->setPosition(Vec2(40, 5));
	m_Gold_Label->setColor(Color3B::YELLOW);
	m_Gold_Sprite->addChild(m_Gold_Label, 5);
}

void GameUI::CreateKeyUi() {
	m_RedKey_Sprite = Sprite::create("Images/ui/RedKey_ui.png");
	m_RedKey_Sprite->setPosition(Vec2(m_layerSize.width - 150, m_layerSize.height - 50));
	m_RedKey_Sprite->setOpacity(80);
	m_RedKey_Sprite->setScale(0.8f);
	m_layer->addChild(m_RedKey_Sprite, 4);

	m_BlueKey_Sprite = Sprite::create("Images/ui/BlueKey_ui.png");
	m_BlueKey_Sprite->setPosition(Vec2(m_layerSize.width - 120, m_layerSize.height - 50));
	m_BlueKey_Sprite->setOpacity(80);
	m_BlueKey_Sprite->setScale(0.8f);
	m_layer->addChild(m_BlueKey_Sprite, 4);

	m_GreenKey_Sprite = Sprite::create("Images/ui/GreenKey_ui.png");
	m_GreenKey_Sprite->setPosition(Vec2(m_layerSize.width - 90, m_layerSize.height - 50));
	m_GreenKey_Sprite->setOpacity(80);
	m_GreenKey_Sprite->setScale(0.8f);
	m_layer->addChild(m_GreenKey_Sprite, 4);
}

void GameUI::MessageOutput(std::string comment) {
	if (m_layer->getChildByTag(MessageTag) == NULL) { // Message off 중 일때만 메시지 출력
		m_MessageComment = Label::createWithTTF(ttfConfig, comment);
		m_MessageComment->setScale(1.0f);
		m_MessageComment->setPosition(Vec2(240, 260));
		m_MessageComment->setColor(Color3B::YELLOW);
		m_layer->addChild(m_MessageComment, 5, MessageTag);
		m_bMessage = true;
	}
}


void GameUI::ItemShopMessage(std::string comment) {
	m_ItemShopMessage = Label::createWithTTF(ttfConfig, comment);
	m_ItemShopMessage->setScale(2.0f);
	m_ItemShopMessage->setPosition(Vec2(240, 160));
	m_ItemShopMessage->setColor(Color3B::YELLOW);
	m_layer->addChild(m_ItemShopMessage, 5);

	if (comment == "Not enough Gold.") {
		//comment = nullptr;
		m_bMessage = true;
	}
	else {
		m_bMessage = false;
	}
}

void GameUI::CreateStageMessage(int _StageNumber) {
	std::string _message;

	switch (_StageNumber) {
	case STAGE_1_1:
		m_Charactor_data->StageNumber = STAGE_1_1;
		_message = "STAGE 1 - 1";
		break;
	case STAGE_1_2:
		m_Charactor_data->StageNumber = STAGE_1_2;
		_message = "STAGE 1 - 2";
		break;
	case STAGE_1_3:
		m_Charactor_data->StageNumber = STAGE_1_3;
		_message = "STAGE 1 - 3";
		break;
	case STAGE_2_1:
		m_Charactor_data->StageNumber = STAGE_2_1;
		_message = "STAGE 2 - 1";
		break;
	case STAGE_2_2:
		m_Charactor_data->StageNumber = STAGE_2_2;
		_message = "STAGE 2 - 2";
		break;
	case STAGE_2_3:
		m_Charactor_data->StageNumber = STAGE_2_3;
		_message = "STAGE 2 - 3";
		break;
	case STAGE_3_1:
		m_Charactor_data->StageNumber = STAGE_3_1;
		_message = "STAGE 3 - 1";
		break;
	case STAGE_3_2:
		m_Charactor_data->StageNumber = STAGE_3_2;
		_message = "STAGE 3 - 2";
		break;
	case STAGE_3_3:
		m_Charactor_data->StageNumber = STAGE_3_3;
		_message = "STAGE 3 - 3";
		break;
	case BOSS:
		m_Charactor_data->StageNumber = BOSS;
		_message = "BOSS";
		break;
	case SHELTER:
		m_Charactor_data->StageNumber = SHELTER;
		_message = "Shelter";
		break;
	}

	m_StageMessageTime = 2.0f;

	m_StageMessage = Label::createWithTTF(ttfConfig, _message);
	m_StageMessage->setScale(4.0f);
	m_StageMessage->setPosition(Vec2(240, 160));
	m_StageMessage->setColor(Color3B::YELLOW);
	m_layer->addChild(m_StageMessage, 5);
}

bool GameUI::TemporaryMessageManager(float* max, float dt) {
	if ((*max -= dt) <= 0) {
		return true;
	}
	else {
		m_bMessage = false;
		return false;
	}
}

void GameUI::MessageDelete(Label* _label) {
	Label* label = _label;
	m_layer->removeChild(label, true);
}

void GameUI::doPause(Ref* pSender) {
	PauseLayer* pause = new PauseLayer(m_layer, m_pPause);
	pause->init(pause);

	m_pPause->setEnabled(false);

	Director::sharedDirector()->pause();
	//delete pause;
}