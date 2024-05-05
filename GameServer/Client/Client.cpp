#include "pch.h"
#include "Client.h"

Client::Client(sf::Texture& _t, POS _pos, int _x, int _y, int _x2, int _y2, int _id, const char* _name)
{
	Sprite.setTexture(_t);
	Sprite.setTextureRect(sf::IntRect(_x, _y, _x2, _y2));
	myId = _id;
	myName = _name;
	myPos = _pos;

	Move(myPos.posx, myPos.posy);
}

Client::~Client()
{

}

void Client::Move(int _x, int _y)
{
	myPos.posx = _x;
	myPos.posy = _y;
}

void Client::Draw()
{
	float fX = (myPos.posx - g_left_x)	*	65.f + 1;
	float fY = (myPos.posy - g_top_y)	*	65.f + 1;
	Sprite.setPosition(fX, fY);

	SFSYSTEM->window->draw(Sprite);
}
