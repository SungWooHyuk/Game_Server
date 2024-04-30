#pragma once

#include "utils.h"
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

class SFSystem
{
private:
	SFSystem();
	static SFSystem*	s_instance;

public:
	~SFSystem();
	static SFSystem*	GetInstance();

	void InitText();
	void InitBox();

	void SetText(SystemText _st, const char _str[]);
	void SetInnerBoxSize(SystemBox _sb, int _inner, int _maxinner);

private:
	vector<sf::Text>			text;
	Vector<sf::RectangleShape>	box;

public:
	sf::TcpSocket				socket;
	sf::RenderWindow*			window;
	sf::Font					font;

public:
	sf::Texture*				monster;
	sf::Texture*				player;
	sf::Texture*				player_attack;
	sf::Texture*				board;

};

