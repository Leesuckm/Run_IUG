#include "CaveCreator.h"

USING_NS_CC;

#define PTMRATIO 32

void CaveCreator::CreateCave(TMXTiledMap* map, b2World* world) {
	ground = map->getLayer("Ground"); // 참고로 하나의 Layer에는 하나의 TileSet을 사용하며 만약 두개째의 TileSet을 사용하면 두개째의 TileSet에서 첫번째 TileSet의 Sprite 정보를 찾으므로 에러가 발생한다.

	mapSize = ground->getLayerSize();

	// 동굴의 위치 포인트
	for (int i = 0; i < 5; i++) {
		Rand_Point.push_back(Vec2(rand() % ((((static_cast<int>(mapSize.width) - 15) - 2) - 2) + 1) + 2,
			rand() % (((static_cast<int>(mapSize.height)- 2) - 2) + 1) + 2));

		// 위치 포인트가 너무 가깝지 않도록
		for (int j = 0; j < i; j++) {
			if (Rand_Point.at(i).x + 10 > Rand_Point.at(j).x &&
				Rand_Point.at(i).x - 10 < Rand_Point.at(j).x &&
				Rand_Point.at(i).y + 10 > Rand_Point.at(j).y &&
				Rand_Point.at(i).y - 10 < Rand_Point.at(j).y) {
				Rand_Point.erase(Rand_Point.begin() + i);
				i--;
				continue;
			}
		}
	}

	CaveShape(map, Rand_Point);

	//CaveBody(map, Rand_Point, world);
}

void CaveCreator::CaveShape(TMXTiledMap* map, std::vector<Vec2> TilePoint) {

	for (int i = 0; i < TilePoint.size(); i++) {
		tileGid = this->ground->getTileGIDAt(TilePoint.at(i));

		if (tileGid) {
			// 타일맵에서 GID에 해당하는 부분의 속성 읽어오기
			Value properties = map->getPropertiesForGID(tileGid);

			if (!properties.isNull()) {
				std::string remove = properties.asValueMap()["Name"].asString();

				nRand_width = rand() % (15 - 10 + 1) + 10;
				nRand_height = rand() % (10 - 7 + 1) + 7;

				for (int j = 0; j < nRand_height; j++) {
					for (int k = 0; k < nRand_width; k++) {
						nShape[j][k] = rand() % 5 + 1;
						TilePoint.at(i).x++;

						if (remove == "Ground" && 
							TilePoint.at(i).x < mapSize.width &&
							TilePoint.at(i).x > 0 &&
							nShape[j][k] != 1) {

							Sprite* spr = ground->getTileAt(TilePoint.at(i));
							ground->removeTileAt(TilePoint.at(i));
							
							this->removeChild(spr, true);
						}
					}
					TilePoint.at(i).x = TilePoint.at(i).x - nRand_width + rand() % (2 - -2 +1) + -2;
					if (TilePoint.at(i).y < mapSize.height && TilePoint.at(i).y > 1) {
						TilePoint.at(i).y--;
					}
					else if (TilePoint.at(i).y <= 0) {
						break;
					}
				}
			}
		}
	}
}

void CaveCreator::CaveBody(TMXTiledMap* map, std::vector<Vec2> TilePoint, b2World* world) {
	for (b2Body* b = world->GetBodyList(); b; b = b->GetNext()) {
		if (b->GetUserData() != nullptr) {
			Sprite* spriteData = (Sprite*)b->GetUserData();
			if (spriteData == nullptr) {
				world->DestroyBody(b);
			}

		}
	}
}