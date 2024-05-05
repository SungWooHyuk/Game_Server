#pragma once

#include "utils.h"
#include "SFSystem.h"

class Client
{
private:

public:
	Client() {};
	Client(sf::Texture& _t, POS _pos, int _x, int _y, int _x2, int _y2, int _id, const char* _name);
	~Client();

	virtual void	Move(int _x, int _y);
	virtual void	Draw();

protected:
	int16		myId;
	string		myName;
	POS			myPos;

protected:
	sf::Sprite	Sprite;
};

