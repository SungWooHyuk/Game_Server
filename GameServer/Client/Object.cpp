#include "pch.h"
#include "Object.h"
#include "utils.h"
#include "MapData.h"

Object* Object::o_instance = nullptr;

Object::Object()
{
	platTile = new Object(platTile, *SFSYSTEM->board, 130, 0, TILE_WIDTH, TILE_WIDTH);
	obstacleTile = new Object(obstacleTile, *SFSYSTEM->board, 195, 0, TILE_WIDTH, TILE_WIDTH);
	blueTownTile = new Object(blueTownTile, *SFSYSTEM->board, 65, 0, TILE_WIDTH, TILE_WIDTH);
	greyTownTile = new Object(greyTownTile, *SFSYSTEM->board, 0, 0, TILE_WIDTH, TILE_WIDTH);
}
Object::Object(Object* _tile, sf::Texture& t, int _x, int _y, int _x2, int _y2)
{
	tileSprite.setTexture(t);
	tileSprite.setTextureRect(sf::IntRect(_x, _y, _x2, _y2));
}

Object::~Object()
{
	delete platTile;
	delete obstacleTile;
	delete blueTownTile;
	delete greyTownTile;
}

Object* Object::GetInstance()
{
	if (o_instance == nullptr)
	{
		o_instance = new Object();
	}
	return o_instance;
}

void Object::TileDraw()
{
	for (int i = 0; i < SCREEN_WIDTH; ++i)
		for (int j = 0; j < SCREEN_HEIGHT; ++j)
		{
			int tile_x = i + g_left_x;
			int tile_y = j + g_top_y;
			if ((tile_x < 0) || (tile_y < 0)) continue;

			if (MAPDATA->GetTile(tile_y, tile_x) == MAPDATA->MAP_TYPE::e_PLAT) {
				platTile->Move(platTile, TILE_WIDTH * i, TILE_WIDTH * j);
				platTile->Draw(platTile);
			}
			else if (MAPDATA->GetTile(tile_y, tile_x) == MAPDATA->MAP_TYPE::e_OBSTACLE)
			{
				obstacleTile->Move(obstacleTile, TILE_WIDTH * i, TILE_WIDTH * j);
				obstacleTile->Draw(obstacleTile);
			}
			else if (MAPDATA->GetTile(tile_y, tile_x) == MAPDATA->MAP_TYPE::e_BTOWN)
			{
				blueTownTile->Move(blueTownTile, TILE_WIDTH * i, TILE_WIDTH * j);
				blueTownTile->Draw(blueTownTile);
			}
			else if (MAPDATA->GetTile(tile_y, tile_x) == MAPDATA->MAP_TYPE::e_GTOWN)
			{
				greyTownTile->Move(greyTownTile, TILE_WIDTH * i, TILE_WIDTH * j);
				greyTownTile->Draw(greyTownTile);
			}
		}
}

void Object::Draw(Object* _tile)
{
	SFSYSTEM->window->draw(_tile->tileSprite);
}

void Object::Move(Object* _tile, int _x, int _y)
{
	_tile->tileSprite.setPosition(static_cast<float>(_x), static_cast<float>(_y));
}
