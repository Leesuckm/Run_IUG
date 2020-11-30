#include "CaveCreator.h"

USING_NS_CC;

#define PTMRATIO 32

void CaveCreator::CreateCave(TMXTiledMap* map, b2World* world) {
	ground = map->getLayer("Ground"); // ����� �ϳ��� Layer���� �ϳ��� TileSet�� ����ϸ� ���� �ΰ�°�� TileSet�� ����ϸ� �ΰ�°�� TileSet���� ù��° TileSet�� Sprite ������ ã���Ƿ� ������ �߻��Ѵ�.

	mapSize = ground->getLayerSize();

	// ������ ��ġ ����Ʈ
	for (int i = 0; i < 5; i++) {
		Rand_Point.push_back(Vec2(rand() % ((((static_cast<int>(mapSize.width) - 15) - 2) - 2) + 1) + 2,
			rand() % (((static_cast<int>(mapSize.height)- 2) - 2) + 1) + 2));

		// ��ġ ����Ʈ�� �ʹ� ������ �ʵ���
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
			// Ÿ�ϸʿ��� GID�� �ش��ϴ� �κ��� �Ӽ� �о����
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