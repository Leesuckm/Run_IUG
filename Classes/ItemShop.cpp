#include "ItemShop.h"
#include "TiledBodyCreator.h"
#include "GameUI.h"
#include <random>

ItemShop::ItemShop() {
}

ItemShop::ItemShop(b2World* _world, Layer* _layer, Layer* _wlayer) {
	m_world = _world;
	m_layer = _layer;
	m_wlayer = _wlayer;

	m_layerSize = m_layer->getContentSize();
}

ItemShop::~ItemShop() {
}

void ItemShop::CreateShop(TiledBodyCreator* _bodycreator, GameUI* _ui) {
	m_Item_creator = _bodycreator;
	m_UI = _ui;
	if (m_enable) {
		ItemShop::CreateShopBackground();
		ItemShop::CreateShopButtons();
	}
	else {
		log("It's already open");
	}
}

void ItemShop::ShopDisplay(bool display, int _charactor_gold, b2Body* body) {
	m_select_list.clear();
	m_select_vc.clear();
	m_batchcount = 0;
	ShopBackground->setVisible(display);
	m_Charactor_Gold = _charactor_gold;
	if (display) {
		ShopBackground->setPosition(Vec2(m_layerSize.width - 400, m_layerSize.height - 320));
		m_position = body->GetPosition();
	}
	else {
		ShopBackground->setPosition(Vec2(0, -400));
	}
}

void ItemShop::CreateShopBackground() {
	ShopBackground = Sprite::create("Images/ui/menu.png");
	ShopBackground->setAnchorPoint(Vec2(0, 0));
	ShopBackground->setPosition(Vec2(0, -400));
	m_layer->addChild(ShopBackground, 5);
}

void ItemShop::CreateShopButtons() {
	m_pExit_Button_im = MenuItemImage::create("Images/ui/x_button.png", "Images/ui/x_button_selected.png",
		CC_CALLBACK_1(ItemShop::doExit, this));

	m_pExit = Menu::create(m_pExit_Button_im, nullptr);
	m_pExit->setPosition(Vec2(280, 250));
	ShopBackground->addChild(m_pExit, 5);

	m_pBuy_Button_im = MenuItemImage::create("Images/ui/buy_button.png", "Images/ui/buy_button_push.png",
		CC_CALLBACK_1(ItemShop::buy, this));
	m_pBuy = Menu::create(m_pBuy_Button_im, nullptr);
	m_pBuy->setPosition(Vec2(218, 40));
	ShopBackground->addChild(m_pBuy, 5);

	ItemShop::ItemToSell();
}

void ItemShop::ItemToSell() {
	int nItemCount = rand() % 7 + 1;
	MenuItemImage* menuitem;
	//Menu* menu;

	for (int i = 0; i < nItemCount; i++) {
		int nItem = rand() % 6;
		switch (nItem) {
		case 0:
			menuitem = MenuItemImage::create("Images/ui/Potion_button.png", "Images/ui/Potion_button_selected.png",
				CC_CALLBACK_1(ItemShop::buyPotion, this));
			menu = Menu::create(menuitem, nullptr);
			menu->setPosition(Vec2(0, 0));
			ShopBackground->addChild(menu, 5);
			m_sell_list.push_back(menu);
			break;
		case 1:
			menuitem = MenuItemImage::create("Images/ui/Bomb_button.png", "Images/ui/Bomb_button_selected.png",
				CC_CALLBACK_1(ItemShop::buyBomb, this));
			menu = Menu::create(menuitem, nullptr);
			menu->setPosition(Vec2(0, 0));
			ShopBackground->addChild(menu, 5);
			m_sell_list.push_back(menu);
			break;
		case 2:
			menuitem = MenuItemImage::create("Images/ui/Pickaxe_button.png", "Images/ui/Pickaxe_button_selected.png",
				CC_CALLBACK_1(ItemShop::buyPickaxe, this));
			menu = Menu::create(menuitem, nullptr);
			menu->setPosition(Vec2(0, 0));
			ShopBackground->addChild(menu, 5);
			m_sell_list.push_back(menu);
			break;
		case 3:
			menuitem = MenuItemImage::create("Images/ui/Sword_button.png", "Images/ui/Sword_button_selected.png",
				CC_CALLBACK_1(ItemShop::buySword, this));
			menu = Menu::create(menuitem, nullptr);
			menu->setPosition(Vec2(0, 0));
			ShopBackground->addChild(menu, 5);
			m_sell_list.push_back(menu);
			break;
		case 4:
			menuitem = MenuItemImage::create("Images/ui/Gun_button.png", "Images/ui/Gun_button_selected.png",
				CC_CALLBACK_1(ItemShop::buyGun, this));
			menu = Menu::create(menuitem, nullptr);
			menu->setPosition(Vec2(0, 0));
			ShopBackground->addChild(menu, 5);
			m_sell_list.push_back(menu);
			break;
		case 5:
			menuitem = MenuItemImage::create("Images/ui/Ladder_button.png", "Images/ui/Ladder_button_selected.png",
				CC_CALLBACK_1(ItemShop::buyLadder, this));
			menu = Menu::create(menuitem, nullptr);
			menu->setPosition(Vec2(0, 0));
			ShopBackground->addChild(menu, 5);
			m_sell_list.push_back(menu);
			break;
		default:
			log("no item");
			break;
		}
	}

	std::list<Menu*>::iterator iter;
	int i = 1;
	int j = 1;
	for (iter = m_sell_list.begin(); iter != m_sell_list.end(); iter++) {
		Menu* pMenu = *iter;
		pMenu->setPosition(Vec2(i * 60, 220));
		if (i > 4) {
			pMenu->setPosition(Vec2(j * 60, 150));
			j++;
		}
		i++;
	}
}

void ItemShop::doExit(Ref* pSender) {
	//ShopBackground->removeChild(m_pExit, true);
	//m_layer->removeChild(ShopBackground, true);
	//ShopBackground = nullptr;
	ItemShop::ShopClear();
}

void ItemShop::selectItem(Menu* item, _SelectedItem item_name) {
//	Sprite* SpriteData;
	//item->setEnabled(false);
	//item->setColor(Color3B(125, 125, 125));

	MenuItemImage* menuitem;
	Menu* menu;
	if (m_select_vc.size() < 4) {
		switch (item_name) {
		case IT_POTION:
			m_nSum += POTION_PRICE;
			menuitem = MenuItemImage::create("Images/ui/Potion_button.png", "Images/ui/Potion_button_selected.png",
				CC_CALLBACK_1(ItemShop::cancel_Potion, this));
			menu = Menu::create(menuitem, nullptr);
			menu->setPosition(Vec2(0, 0));
			ShopBackground->addChild(menu, 5);
			menu->setTag(IT_POTION);
			m_select_list.push_back(menu);
			m_select_vc.push_back(Select_Menu(menu, m_batchcount));
			break;
		case IT_BOMB:
			m_nSum += BOMB_PRICE;
			menuitem = MenuItemImage::create("Images/ui/Bomb_button.png", "Images/ui/Bomb_button_selected.png",
				CC_CALLBACK_1(ItemShop::cancel_Bomb, this));
			menu = Menu::create(menuitem, nullptr);
			menu->setPosition(Vec2(0, 0));
			ShopBackground->addChild(menu, 5);
			menu->setTag(IT_BOMB);
			m_select_list.push_back(menu);
			m_select_vc.push_back(Select_Menu(menu, m_batchcount));
			break;
		case IT_PICKAXE:
			m_nSum += PICKAXE_PRICE;
			menuitem = MenuItemImage::create("Images/ui/Pickaxe_button.png", "Images/ui/Pickaxe_button_selected.png",
				CC_CALLBACK_1(ItemShop::cancel_Pickaxe, this));
			menu = Menu::create(menuitem, nullptr);
			menu->setPosition(Vec2(0, 0));
			ShopBackground->addChild(menu, 5);
			menu->setTag(IT_PICKAXE);
			m_select_list.push_back(menu);
			m_select_vc.push_back(Select_Menu(menu, m_batchcount));
			break;
		case IT_SWORD:
			m_nSum += SWORD_PRICE;
			menuitem = MenuItemImage::create("Images/ui/Sword_button.png", "Images/ui/Sword_button_selected.png",
				CC_CALLBACK_1(ItemShop::cancel_Sword, this));
			menu = Menu::create(menuitem, nullptr);
			menu->setPosition(Vec2(0, 0));
			ShopBackground->addChild(menu, 5);
			menu->setTag(IT_SWORD);
			m_select_list.push_back(menu);
			m_select_vc.push_back(Select_Menu(menu, m_batchcount));
			break;
		case IT_GUN:
			m_nSum += GUN_PRICE;
			menuitem = MenuItemImage::create("Images/ui/Gun_button.png", "Images/ui/Gun_button_selected.png",
				CC_CALLBACK_1(ItemShop::cancel_Gun, this));
			menu = Menu::create(menuitem, nullptr);
			menu->setPosition(Vec2(0, 0));
			ShopBackground->addChild(menu, 5);
			menu->setTag(IT_GUN);
			m_select_list.push_back(menu);
			m_select_vc.push_back(Select_Menu(menu, m_batchcount));
			break;
		case IT_LADDER:
			m_nSum += LADDER_PRICE;
			menuitem = MenuItemImage::create("Images/ui/Ladder_button.png", "Images/ui/Ladder_button_selected.png",
				CC_CALLBACK_1(ItemShop::cancel_Ladder, this));
			menu = Menu::create(menuitem, nullptr);
			menu->setPosition(Vec2(0, 0));
			ShopBackground->addChild(menu, 5);
			menu->setTag(IT_LADDER);
			m_select_list.push_back(menu);
			m_select_vc.push_back(Select_Menu(menu, m_batchcount));
			break;
		}
	}
	m_batchcount++;
}

void ItemShop::select_batch() {
	Menu* pFront = m_select_list.front();

	std::vector<Select_Menu>::iterator iter;
	for (iter = m_select_vc.begin(); iter != m_select_vc.end(); ++iter) {
		Select_Menu batch_item = *iter;
		if (batch_item.room == 0) {
			iter->menu->setPosition(Vec2(170, 80));
		}
		else if (batch_item.room == 1) {
			iter->menu->setPosition(Vec2(200, 80));
		}
		else if (batch_item.room == 2) {
			iter->menu->setPosition(Vec2(230, 80));
		}
		else if (batch_item.room == 3) {
			iter->menu->setPosition(Vec2(260, 80));
		}
	}	
}

void ItemShop::buyPotion(Ref* pSender) {
	ItemShop::selectItem(menu, IT_POTION);
	ItemShop::select_batch();
}
void ItemShop::buyBomb(Ref* pSender) {
	ItemShop::selectItem(menu, IT_BOMB);
	ItemShop::select_batch();
}
void ItemShop::buyPickaxe(Ref* pSender) {
	ItemShop::selectItem(menu, IT_PICKAXE);
	ItemShop::select_batch();
}
void ItemShop::buySword(Ref* pSender) {
	ItemShop::selectItem(menu, IT_SWORD);
	ItemShop::select_batch();
}
void ItemShop::buyGun(Ref* pSender) {
	ItemShop::selectItem(menu, IT_GUN);
	ItemShop::select_batch();
}
void ItemShop::buyLadder(Ref* pSender) {
	ItemShop::selectItem(menu, IT_LADDER);
	ItemShop::select_batch();
}
void ItemShop::buy(Ref* pSender) {
	if (m_Charactor_Gold - m_nSum >= 0) {
		std::vector<Select_Menu>::iterator iter;
		for (iter = m_select_vc.begin(); iter != m_select_vc.end();) {
			Select_Menu Select_item = *iter;
			int tag = Select_item.menu->getTag();
			b2Body* item;

			switch (tag) {
			case IT_POTION:
				m_Charactor_Gold -= POTION_PRICE;
				item = m_Item_creator->CreateObj("Potion", m_position, m_world, m_wlayer);
				m_sell_ItemQ.push(item);
				ShopBackground->removeChildByTag(IT_POTION, true);
				iter = m_select_vc.erase(iter);
				break;
			case IT_BOMB:
				m_Charactor_Gold -= BOMB_PRICE;
				item = m_Item_creator->CreateObj("Bomb", m_position, m_world, m_wlayer);
				m_sell_ItemQ.push(item);
				ShopBackground->removeChildByTag(IT_BOMB, true);
				iter = m_select_vc.erase(iter);
				break;
			case IT_PICKAXE:
				m_Charactor_Gold -= PICKAXE_PRICE;
				item = m_Item_creator->CreateObj("Pickaxe", m_position, m_world, m_wlayer);
				m_sell_ItemQ.push(item);
				ShopBackground->removeChildByTag(IT_PICKAXE, true);
				iter = m_select_vc.erase(iter);
				break;
			case IT_SWORD:
				m_Charactor_Gold -= SWORD_PRICE;
				item = m_Item_creator->CreateObj("Sword", m_position, m_world, m_wlayer);
				m_sell_ItemQ.push(item);
				ShopBackground->removeChildByTag(IT_SWORD, true);
				iter = m_select_vc.erase(iter);
				break;
			case IT_GUN:
				m_Charactor_Gold -= GUN_PRICE;
				item = m_Item_creator->CreateObj("Gun", m_position, m_world, m_wlayer);
				m_sell_ItemQ.push(item);
				ShopBackground->removeChildByTag(IT_GUN, true);
				iter = m_select_vc.erase(iter);
				break;
			case IT_LADDER:
				m_Charactor_Gold -= LADDER_PRICE;
				item = m_Item_creator->CreateObj("Ladder_item", m_position, m_world, m_wlayer);
				m_sell_ItemQ.push(item);
				ShopBackground->removeChildByTag(IT_LADDER, true);
				iter = m_select_vc.erase(iter);
				break;
			}
		}
		m_small_change = m_Charactor_Gold;
	}
	else {
		std::string message = "Not enough Gold.";
		m_UI->ItemShopMessage(message);
	}

	ItemShop::ShopClear();
}

void ItemShop::ThrowItem(float dt) {
	for (int i = 0; i < m_sell_ItemQ.size(); i++) {
		int rand_power = rand() % 5 + 1;
		b2Body* Item = m_sell_ItemQ.front();

		//if ((m_Throwcount -= dt) <= 0) {
		//	m_Throwcount = 2;
			Item->ApplyLinearImpulse(b2Vec2(rand_power, 5), Item->GetWorldCenter(), true);
			m_sell_ItemQ.pop();
		//}
	}
}


void ItemShop::cancel_Potion(Ref* pSender) {
	m_batchcount--;
	std::vector<Select_Menu>::iterator iter;
	for (iter = m_select_vc.begin(); iter != m_select_vc.end();) {
		Select_Menu Select_menu = *iter;
		int tag = Select_menu.menu->getTag();
		if (tag == IT_POTION) {
			iter = m_select_vc.erase(iter);
			ShopBackground->removeChildByTag(IT_POTION, true);
			break;
		}
		else
			iter++;
	}
}

void ItemShop::cancel_Bomb(Ref* pSender) {
	m_batchcount--;
	std::vector<Select_Menu>::iterator iter;
	for (iter = m_select_vc.begin(); iter != m_select_vc.end();) {
		Select_Menu Select_menu = *iter;
		int tag = Select_menu.menu->getTag();
		if (tag == IT_BOMB) {
			iter = m_select_vc.erase(iter);
			ShopBackground->removeChildByTag(IT_BOMB, true);
			break;
		}
		else
			iter++;
	}
}
void ItemShop::cancel_Pickaxe(Ref* pSender) {
	m_batchcount--;
	std::vector<Select_Menu>::iterator iter;
	for (iter = m_select_vc.begin(); iter != m_select_vc.end();) {
		Select_Menu Select_menu = *iter;
		int tag = Select_menu.menu->getTag();
		if (tag == IT_PICKAXE) {
			iter = m_select_vc.erase(iter);
			ShopBackground->removeChildByTag(IT_PICKAXE, true);
			break;
		}
		else
			iter++;
	}
}
void ItemShop::cancel_Sword(Ref* pSender) {
	m_batchcount--;
	std::vector<Select_Menu>::iterator iter;
	for (iter = m_select_vc.begin(); iter != m_select_vc.end();) {
		Select_Menu Select_menu = *iter;
		int tag = Select_menu.menu->getTag();
		if (tag == IT_SWORD) {
			iter = m_select_vc.erase(iter);
			ShopBackground->removeChildByTag(IT_SWORD, true);
			break;
		}
		else
			iter++;
	}

}
void ItemShop::cancel_Gun(Ref* pSender) {
	m_batchcount--;
	std::vector<Select_Menu>::iterator iter;
	for (iter = m_select_vc.begin(); iter != m_select_vc.end();) {
		Select_Menu Select_menu = *iter;
		int tag = Select_menu.menu->getTag();
		if (tag == IT_GUN) {
			iter = m_select_vc.erase(iter);
			ShopBackground->removeChildByTag(IT_GUN, true);
			break;
		}
		else
			iter++;
	}
}
void ItemShop::cancel_Ladder(Ref* pSender) {
	m_batchcount--;
	std::vector<Select_Menu>::iterator iter;
	for (iter = m_select_vc.begin(); iter != m_select_vc.end();) {
		Select_Menu Select_menu = *iter;
		int tag = Select_menu.menu->getTag();
		if (tag == IT_LADDER) {
			iter = m_select_vc.erase(iter);
			ShopBackground->removeChildByTag(IT_LADDER, true);
			break;
		}
		else
			iter++;
	}
}

void ItemShop::ShopClear() {
	std::vector<Select_Menu>::iterator iter;
	for (iter = m_select_vc.begin(); iter != m_select_vc.end(); ++iter) {
		Select_Menu menu = *iter;
		int tag = menu.menu->getTag();
		switch (tag) {
		case IT_POTION:
			ShopBackground->removeChildByTag(IT_POTION, true);
			break;
		case IT_BOMB:
			ShopBackground->removeChildByTag(IT_BOMB, true);
			break;
		case IT_PICKAXE:
			ShopBackground->removeChildByTag(IT_PICKAXE, true);
			break;
		case IT_SWORD:
			ShopBackground->removeChildByTag(IT_SWORD, true);
			break;
		case IT_GUN:
			ShopBackground->removeChildByTag(IT_GUN, true);
			break;
		case IT_LADDER:
			ShopBackground->removeChildByTag(IT_LADDER, true);
			break;
		}
	}
	m_nSum = 0;
	m_batchcount = 0;
	m_select_list.clear();
	m_select_vc.clear();
	ItemShop::ShopDisplay(false, 0, nullptr);
	m_enable = true;
}