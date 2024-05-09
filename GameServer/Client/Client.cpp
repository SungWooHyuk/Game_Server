#include "pch.h"
#include "protocol.h"
#include "utils.h"
#include "Client.h"

Client::Client(sf::Texture& _t, POS _pos, int _x, int _y, int _x2, int _y2, int _id, const char* _name)
{
	Sprite.setTexture(_t);
	Sprite.setTextureRect(sf::IntRect(_x, _y, _x2, _y2));
	myId = _id;
	myName = _name;
	myPos = _pos;

	SetNameColor(_name);
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
	
	auto size = myTextName.getGlobalBounds();

	Sprite.setPosition(fX, fY);
	myTextName.setPosition(fX + 32 - size.width / 2, fY - 10);

	SFSYSTEM->window->draw(Sprite);
	SFSYSTEM->window->draw(myTextName);
}

void Client::SetNameColor(const char str[])
{
	myTextName.setFont(SFSYSTEM->font);
	myTextName.setString(str);
	myTextName.setStyle(sf::Text::Bold);
	if (myId < MAX_USER)
		myTextName.setFillColor(sf::Color(WHITE));
	else if(myId > MAX_USER && myId < MAX_NPC / 4)
		myTextName.setFillColor(sf::Color(MAGENTA));
	else if(myId >= MAX_NPC / 4 && myId < (MAX_NPC / 4 * 2))
		myTextName.setFillColor(sf::Color(YELLOW));
	else if ((myId >= (MAX_NPC / 4 * 2) && myId < (MAX_NPC / 4 * 3)))
		myTextName.setFillColor(sf::Color(WHITE));
	else
		myTextName.setFillColor(sf::Color(GREEN));

}
