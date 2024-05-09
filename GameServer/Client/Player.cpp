#include "pch.h"
#include "Player.h"
#include "Client.h"
#include "utils.h"

Player::Player(sf::Texture& _attackT, sf::Texture& _playerT, int _x, int _y, int _x2, int _y2, STAT _st, POS _pos, TP _tp, int16 _id, const char* _name)
{
	Sprite.setTexture(_playerT);
	Sprite.setTextureRect(sf::IntRect(_x, _y, _x2, _y2));

	attackSprite.setTexture(_attackT);
	attackSprite.setTextureRect(sf::IntRect(_x, _y, _x2, _y2));

	myId = _id;
	myName = _name;
	myStat = _st;
	myPos = _pos;
	myTp = _tp;

	Move(myPos.posx, myPos.posy);
}

void Player::SetStat(STAT _st)
{
	myStat.hp = _st.hp;
	myStat.maxHp = _st.maxHp;
	myStat.mp = _st.mp;
	myStat.maxMp = _st.maxMp;
	myStat.exp = _st.exp;
	myStat.maxExp = _st.maxExp;
	myStat.level = _st.level;
}

void Player::Draw()
{
	float fX = (myPos.posx - g_left_x) * 65.f + 1;
	float fY = (myPos.posy - g_top_y) * 65.f + 1;

	Sprite.setPosition(fX, fY);
	SFSYSTEM->window->draw(Sprite);

	if (myTp.attackEndTime > chrono::system_clock::now())
	{
		attackSprite.setPosition(fX, fY - 65);
		SFSYSTEM->window->draw(attackSprite);
		attackSprite.setPosition(fX, fY + 65);
		SFSYSTEM->window->draw(attackSprite);
		attackSprite.setPosition(fX + 65, fY);
		SFSYSTEM->window->draw(attackSprite);
		attackSprite.setPosition(fX - 65, fY);
		SFSYSTEM->window->draw(attackSprite);
	}
}
