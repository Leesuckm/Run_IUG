#ifndef __CAVE_CREATOR_H__
#define __CAVE_CREATOR_H__

#include "cocos2d.h"
#include <algorithm>
#include <Box2D/Box2D.h>

USING_NS_CC;

class CaveCreator : public cocos2d::Scene {
private:
	std::vector<Vec2> Tile_Point;
	std::vector<Vec2> Rand_Point;

	cocos2d::Size mapSize;
	cocos2d::TMXLayer* ground;
	cocos2d::TMXLayer* metainfo;
	
	int tileGid;
	int nShape[10][15];
	int nRand_height;
	int nRand_width;

public:
	void CreateCave(TMXTiledMap* map, b2World* world);
	void CaveShape(TMXTiledMap* map, std::vector<Vec2> TilePoint);
	void CaveBody(TMXTiledMap* map, std::vector<Vec2> TilePoint, b2World* world);
	//CaveCreator();
	//~CaveCreator();

};


#endif // __CAVE_CREATOR_H__