#ifndef __ItemShop_H__
#define __ItemShop_H__
#pragma once
#include "HelloWorldScene.h"

#define POTION_PRICE	 500
#define BOMB_PRICE		 700
#define PICKAXE_PRICE	 1000
#define SWORD_PRICE		 1500
#define GUN_PRICE		 2000
#define LADDER_PRICE	 1000

enum _moveState;
class TiledBodyCreator;
class GameUI;

enum _SelectedItem {
	IT_POTION,
	IT_BOMB,
	IT_PICKAXE,
	IT_SWORD,
	IT_GUN,
	IT_LADDER,
};

struct Select_Menu {
	Menu* menu;
	int room;

	Select_Menu(Menu* _menu, int _room) {
		menu = _menu;
		room = _room;
	}
};

class ItemShop {
private:
	b2World* m_world;
	Layer* m_layer;
	Layer* m_wlayer;
	Size m_layerSize;

	bool m_enable;
	b2Vec2 m_position;

	int m_nSum = 0;
	int m_Charactor_Gold;

	Menu* menu;

	Sprite* ShopBackground;
	MenuItem* m_pExit_Button_im;
	Menu* m_pExit;

	MenuItem* m_pPotion_Button_im;
	Menu* m_pPotion;

	MenuItem* m_pBomb_Button_im;
	Menu* m_pBomb;

	MenuItem* m_pPickaxe_Button_im;
	Menu* m_pPickaxe;

	MenuItem* m_pSword_Button_im;
	Menu* m_pSword;

	MenuItem* m_pGun_Button_im;
	Menu* m_pGun;

	MenuItem* m_pLadder_Button_im;
	Menu* m_pLadder;

	MenuItem* m_pBuy_Button_im;
	Menu* m_pBuy;

	TiledBodyCreator* m_Item_creator;
	GameUI* m_UI;

	std::list<Menu*> m_sell_list;
	std::vector<Select_Menu> m_select_vc;
	std::vector<Menu*> m_select_list;
	std::queue<b2Body*> m_sell_ItemQ;

	int m_batchcount = 0;
	float m_Throwcount = 2;
	int m_small_change;

public:
	ItemShop();
	ItemShop(b2World* world, Layer* layer, Layer* _wlayer);
	~ItemShop();

	void CreateShop(TiledBodyCreator* _bodycreator, GameUI* _ui);
	void CreateShopBackground();
	void CreateShopButtons();

	void ItemToSell();

	void doExit(Ref* pSender);

	void selectItem(Menu* item, _SelectedItem item_name);
	void select_batch();

	void buyPotion(Ref* pSender);
	void buyBomb(Ref* pSender);
	void buyPickaxe(Ref* pSender);
	void buySword(Ref* pSender);
	void buyGun(Ref* pSender);
	void buyLadder(Ref* pSender);

	void buy(Ref* pSender);
	void ThrowItem(float dt);

	void cancel_Potion(Ref* pSender);
	void cancel_Bomb(Ref* pSender);
	void cancel_Pickaxe(Ref* pSender);
	void cancel_Sword(Ref* pSender);
	void cancel_Gun(Ref* pSender);
	void cancel_Ladder(Ref* pSender);

	bool getShopEnable() { return m_enable; };
	void setShopEnable(bool tf) { m_enable = tf; };

	int getThrowItemQSize() { return m_sell_ItemQ.size(); };

	void ShopDisplay(bool display, int _charactor_gold, b2Body* body);
	int small_change() { return m_small_change; };
	void ShopClear();

};
#endif __ItemShop_H__