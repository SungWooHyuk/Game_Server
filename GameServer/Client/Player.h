#pragma once
#include "Client.h"
#include "Utils.h"
#include "SFSystem.h"

class Player : public Client
{
private:
	using Client::Sprite;

public:
	Player(sf::Texture& _attackT, sf::Texture& _playerT, int _x, int _y, int _x2, int _y2, STAT _st, POS _pos, TP _tp, int16 _id, const char* _name);
	Player() {};
	~Player() {};

	TP		GetTP() const { return myTp; };
	void	SetAttackTime() { 
		myTp.attackEndTime = chrono::system_clock::now() + chrono::milliseconds(500);
		myTp.attackTime = chrono::system_clock::now() + chrono::milliseconds(1000);
	};

	POS		GetPos() const { return myPos; };
	void	SetStat(STAT _st);
	STAT	GetStat() const { return myStat; };

	void	Draw();
private:
	STAT		myStat;
	TP			myTp;
private:
	
	sf::Sprite	attackSprite;
};

