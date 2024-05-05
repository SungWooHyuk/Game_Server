#pragma once
#include "utils.h"
#include "SFSystem.h"

class Object
{
private:
	Object();
	static	Object* o_instance;
	Object(Object* _tile, sf::Texture& t, int _x, int _y, int _x2, int _y2);

public:
	~Object();
	static Object* GetInstance();


	void	TileDraw();
	void	Draw(Object* _tile);
	void	Move(Object* _tile, int _x, int _y);

private:
	Object*		platTile;
	Object*		obstacleTile;
	Object*		blueTownTile;
	Object*		greyTownTile;

private:
	sf::Sprite	tileSprite;
};

