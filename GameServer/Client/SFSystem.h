#pragma once

#include "utils.h"
#include "queue"
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
	void SetChat(const char* _mess);
	void SetInnerBoxSize(SystemBox _sb, int _inner, int _maxinner);
	void Draw();
	

private:
	vector<sf::Text>			text;
	vector<sf::RectangleShape>	box;
	vector<sf::Text>			systemChat;

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

