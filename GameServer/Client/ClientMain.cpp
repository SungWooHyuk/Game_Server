#include "pch.h"
#include "utils.h"
#include "MapData.h"

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include "protocol.h"

sf::TcpSocket s_socket;
sf::RenderWindow* g_window;
sf::Font g_font;

int g_left_x;
int g_top_y;
int g_myid;

vector<string> sc(10);

bool hpcheck = true;
bool mpcheck = true;
bool expcheck = true;

class OBJECT {
private:
	bool m_showing;
	sf::Sprite m_sprite;
	sf::Sprite m_sprite_attack;

	sf::Text m_name;
	sf::Text m_information[10];
	sf::Text m_level;
	sf::Text m_exp;
	sf::Text m_maxexp;
	sf::Text m_chat;
	sf::Text m_hp;
	sf::Text m_mp;
	sf::Text m_maxhp;
	sf::Text m_maxmp;
	sf::RectangleShape systemBox;
	sf::RectangleShape hpBox;
	sf::RectangleShape monsterhpBox;
	sf::RectangleShape mpBox;
	sf::RectangleShape expBox;
	sf::RectangleShape hpBoxInner;
	sf::RectangleShape mpBoxInner;
	sf::RectangleShape expBoxInner;

	chrono::system_clock::time_point m_mess_end_time;
	chrono::system_clock::time_point m_information_end_time[10];
	chrono::system_clock::time_point m_attack_end_time;
public:
	int id;
	int m_x, m_y;
	int _hp;
	int _mp;
	int _exp;
	int max_hp;
	int max_mp;
	int max_exp;
	int _level;

	int num = 0;
	char name[NAME_SIZE];
	char maxexp[10];
	char exp[10];
	bool characterattack = false;
	bool sc[10] = { false, };
	int sc_num = 0;
	chrono::system_clock::time_point attack_start_time;
	chrono::system_clock::time_point move_start_time;
	chrono::system_clock::time_point information_start_time[6];
	OBJECT(sf::Texture& t, int x, int y, int x2, int y2) {
		m_showing = false;
		m_sprite.setTexture(t);
		m_sprite.setTextureRect(sf::IntRect(x, y, x2, y2));
		set_name("NONAME");
		m_mess_end_time = chrono::system_clock::now();
	}

	OBJECT() {
		m_showing = false;
	}
	void show()
	{
		m_showing = true;
	}
	void hide()
	{
		m_showing = false;
	}

	void a_move(int x, int y) {
		m_sprite.setPosition((float)x, (float)y);
	}

	void a_draw() {
		g_window->draw(m_sprite);
	}

	void move(int x, int y) {
		m_x = x;
		m_y = y;
	}

	void CharacterAttack(sf::Texture& t, int x, int y, int x2, int y2)
	{
		m_sprite_attack.setTexture(t);
		m_sprite_attack.setTextureRect(sf::IntRect(x, y, x2, y2));
	}
	void attacktime()
	{
		m_attack_end_time = chrono::system_clock::now() + chrono::milliseconds(500);
	}
	void draw() {
		if (false == m_showing) return;

		float rx = (m_x - g_left_x) * 65.0f + 1;
		float ry = (m_y - g_top_y) * 65.0f + 1;
		m_sprite.setPosition(rx, ry);
		g_window->draw(m_sprite);
		auto size = m_name.getGlobalBounds();

		//monsterhpBox.setOutlineThickness(3.0f);
		//monsterhpBox.setOutlineColor(sf::Color::Green);

		hpBox.setSize(sf::Vector2f(300, 30));
		hpBox.setFillColor(sf::Color(255, 255, 255));
		hpBox.setPosition(sf::Vector2f(1050, 400));
		hpBox.setOutlineThickness(3.0f);
		hpBox.setOutlineColor(sf::Color::Green);
		//hpBox.setScale(1, 1);

		g_window->draw(hpBox);

		mpBox.setSize(sf::Vector2f(300, 30));
		mpBox.setFillColor(sf::Color(255, 255, 255));
		mpBox.setPosition(sf::Vector2f(1050, 500));
		mpBox.setOutlineThickness(3.0f);
		mpBox.setOutlineColor(sf::Color::Green);
		//hpBox.setScale(1, 1);
		g_window->draw(mpBox);

		expBox.setSize(sf::Vector2f(300, 30));
		expBox.setFillColor(sf::Color(255, 255, 255));
		expBox.setPosition(sf::Vector2f(1050, 600));
		expBox.setOutlineThickness(3.0f);
		expBox.setOutlineColor(sf::Color::Green);
		//hpBox.setScale(1, 1);
		g_window->draw(expBox);

		if (m_mess_end_time < chrono::system_clock::now()) {
			m_name.setPosition(rx + 32 - size.width / 2, ry - 10);
			g_window->draw(m_name);
		}
		else {
			m_chat.setPosition(rx + 32 - size.width / 2, ry - 10);
			g_window->draw(m_chat);
		}

		m_level.setPosition(1050, 350);
		g_window->draw(m_level);

		m_hp.setPosition(1050, 450);
		g_window->draw(m_hp);

		m_maxhp.setPosition(1200, 450);
		g_window->draw(m_maxhp);

		m_mp.setPosition(1050, 550);
		g_window->draw(m_mp);

		m_maxmp.setPosition(1200, 550);
		g_window->draw(m_maxmp);

		m_exp.setPosition(1050, 650);
		g_window->draw(m_exp);

		m_maxexp.setPosition(1200, 650);
		g_window->draw(m_maxexp);

		for (int i = 0; i < 10; ++i)
		{
			if (m_information_end_time[i] > chrono::system_clock::now())
			{
				m_information[i].setPosition(1040, 35 * i);
				g_window->draw(m_information[i]);
			}
		}

		if (m_attack_end_time > chrono::system_clock::now())
		{
			m_sprite_attack.setPosition(rx, ry - 65);
			g_window->draw(m_sprite_attack);

			m_sprite_attack.setPosition(rx, ry + 65);
			g_window->draw(m_sprite_attack);

			m_sprite_attack.setPosition(rx + 65, ry);
			g_window->draw(m_sprite_attack);

			m_sprite_attack.setPosition(rx - 65, ry);
			g_window->draw(m_sprite_attack);

		}

	}
	void set_name(const char str[]) {
		m_name.setFont(g_font);
		m_name.setString(str);
		m_name.setStyle(sf::Text::Bold);
		if (id < MAX_USER) m_name.setFillColor(sf::Color(255, 255, 255));
		else if (id > MAX_USER && id < MAX_NPC / 4)
		{
			m_name.setFillColor(sf::Color(255, 0, 255));
		}
		else if (id >= MAX_NPC / 4 && id < (MAX_NPC / 4 * 2))
		{
			m_name.setFillColor(sf::Color(255, 255, 0));
		}
		else if (id >= (MAX_NPC / 4 * 2) && id < (MAX_NPC / 4 * 3))
		{
			m_name.setFillColor(sf::Color(255, 255, 255));
		}
		else
		{
			m_name.setFillColor(sf::Color(0, 255, 0));
		}
	}


	void set_chat(const char str[]) {
		m_chat.setFont(g_font);
		m_chat.setString(str);
		m_chat.setFillColor(sf::Color(255, 255, 255));
		m_chat.setStyle(sf::Text::Bold);
		m_mess_end_time = chrono::system_clock::now() + chrono::seconds(3);
	}

	void set_level(const char str[])
	{
		m_level.setFont(g_font);
		m_level.setString(str);
		m_level.setFillColor(sf::Color(255, 255, 255));
		m_level.setStyle(sf::Text::Bold);
	}

	void set_hp(const char str[])
	{
		m_hp.setFont(g_font);
		m_hp.setString(str);
		m_hp.setFillColor(sf::Color(255, 255, 255));
		m_hp.setStyle(sf::Text::Bold);
	}

	void set_maxhp(const char str[])
	{
		m_maxhp.setFont(g_font);
		m_maxhp.setString(str);
		m_maxhp.setFillColor(sf::Color(255, 255, 255));
		m_maxhp.setStyle(sf::Text::Bold);
	}


	void set_mp(const char str[])
	{
		m_mp.setFont(g_font);
		m_mp.setString(str);
		m_mp.setFillColor(sf::Color(255, 255, 255));
		m_mp.setStyle(sf::Text::Bold);
	}

	void set_maxmp(const char str[])
	{
		m_maxmp.setFont(g_font);
		m_maxmp.setString(str);
		m_maxmp.setFillColor(sf::Color(255, 255, 255));
		m_maxmp.setStyle(sf::Text::Bold);
	}

	// char buff[256];
	// int x = 10;
	// sprintf(buff, "%d", x);
	void set_exp(const char str[])
	{
		m_exp.setFont(g_font);
		m_exp.setString(str);
		m_exp.setFillColor(sf::Color(255, 255, 255));
		m_exp.setStyle(sf::Text::Bold);
	}

	void set_maxexp(const char str[])
	{
		m_maxexp.setFont(g_font);
		m_maxexp.setString(str);
		m_maxexp.setFillColor(sf::Color(255, 255, 255));
		m_maxexp.setStyle(sf::Text::Bold);
	}

	void system_chat(const char str[])
	{

		for (int i = 0; i < 10; ++i)
		{
			m_information[i].setFont(g_font);
			m_information[i].setFillColor(sf::Color(255, 255, 255));
			m_information[i].setStyle(sf::Text::Bold);
			m_information[i].setScale(sf::Vector2f(0.5f, 0.5f));
		}

		/*for (int i = 9; i > 0; --i)
		{
			if (m_information[i].getString() != "NULL")
			{
				m_information[i + 1].setString(m_information[i].getString());
				m_information_end_time[i+1] = m_information_end_time[i];
			}
			else if (i == 0)
			{
				m_information[i].setString(str);
				m_information_end_time[0] = chrono::system_clock::now() + chrono::seconds(5);
			}
			else
				continue;
		}*/
		m_information[9].setString(m_information[8].getString());
		m_information[8].setString(m_information[7].getString());
		m_information[7].setString(m_information[6].getString());
		m_information[6].setString(m_information[5].getString());
		m_information[5].setString(m_information[4].getString());
		m_information[4].setString(m_information[3].getString());
		m_information[3].setString(m_information[2].getString());
		m_information[2].setString(m_information[1].getString());
		m_information[1].setString(m_information[0].getString());
		m_information[0].setString(str);

		m_information_end_time[9] = m_information_end_time[8];
		m_information_end_time[8] = m_information_end_time[7];
		m_information_end_time[7] = m_information_end_time[6];
		m_information_end_time[6] = m_information_end_time[5];
		m_information_end_time[5] = m_information_end_time[4];
		m_information_end_time[4] = m_information_end_time[3];
		m_information_end_time[3] = m_information_end_time[2];
		m_information_end_time[2] = m_information_end_time[1];
		m_information_end_time[1] = m_information_end_time[0];
		m_information_end_time[0] = chrono::system_clock::now() + chrono::seconds(5);


	}
};

OBJECT avatar;
OBJECT attack;
unordered_map <int, OBJECT> players;

OBJECT plat_tile;
OBJECT obstacle_tile;
OBJECT Btown_tile;
OBJECT Gtown_tile;

sf::Texture* board;
sf::Texture* monster;
sf::Texture* player;
sf::Texture* player_attack;

void client_initialize()
{
	board = new sf::Texture;
	player = new sf::Texture;
	monster = new sf::Texture;
	player_attack = new sf::Texture;

	board->loadFromFile("./Image/maps.bmp");
	player->loadFromFile("./Image/Player.png");
	monster->loadFromFile("./Image/Monster.png");
	player_attack->loadFromFile("./Image/player_Attack1.png");

	if (false == g_font.loadFromFile("cour.ttf")) {
		cout << "Font Loading Error!\n";
		//exit(-1);
	}
	attack = OBJECT(*player_attack, 0, 0, 65, 65);
	plat_tile = OBJECT{ *board, 130, 0, TILE_WIDTH, TILE_WIDTH };
	obstacle_tile = OBJECT{ *board, 195, 0, TILE_WIDTH, TILE_WIDTH };
	Btown_tile = OBJECT{ *board, 65, 0, TILE_WIDTH, TILE_WIDTH };
	Gtown_tile = OBJECT{ *board, 0, 0, TILE_WIDTH, TILE_WIDTH };

	avatar = OBJECT(*player, 0, 0, 65, 65);
	avatar.CharacterAttack(*player_attack, 0, 0, 65, 65);

	avatar.move(4, 4);
}

void client_finish()
{
	players.clear();
	delete board;
	delete player;
	delete monster;
	delete player_attack;
}

void ProcessPacket(char* ptr)
{
	static bool first_time = true;
	switch (ptr[1])
	{
	case SC_LOGIN_OK:
	{
		SC_LOGIN_OK_PACKET* packet = reinterpret_cast<SC_LOGIN_OK_PACKET*>(ptr);
		g_myid = packet->id;
		avatar.id = g_myid;
		avatar.m_x = packet->x;
		avatar.m_y = packet->y;
		avatar._exp = packet->exp;
		avatar._hp = packet->hp;
		avatar._mp = packet->mp;
		avatar.max_exp = packet->max_exp;
		avatar.max_hp = packet->max_hp;
		avatar.max_mp = packet->max_mp;
		avatar._level = packet->level;

		avatar.move(packet->x, packet->y);
		g_left_x = packet->x - SCREEN_WIDTH / 2;
		g_top_y = packet->y - SCREEN_HEIGHT / 2;

		avatar.show();
		break;
	}
	case SC_LOGIN_FAIL:
	{
		exit(-1);
		break;
	}
	case SC_ADD_OBJECT:
	{
		SC_ADD_OBJECT_PACKET* my_packet = reinterpret_cast<SC_ADD_OBJECT_PACKET*>(ptr);
		int id = my_packet->id;

		if (id == g_myid) {
			avatar.move(my_packet->x, my_packet->y);
			g_left_x = my_packet->x - SCREEN_WIDTH / 2;
			g_top_y = my_packet->y - SCREEN_HEIGHT / 2;

			avatar.show();
		}
		else if (id < MAX_USER) {
			players[id] = OBJECT{ *player, 0, 0, 65, 65 };
			players[id].id = id;
			players[id].move(my_packet->x, my_packet->y);
			players[id].set_name(my_packet->name);
			players[id].show();
		}
		else {
			if (id > MAX_USER && id < MAX_NPC / 4) {
				players[id] = OBJECT{ *monster, 0, 0, 65, 65 };
				players[id].id = id;
				players[id].move(my_packet->x, my_packet->y);
				players[id].set_name("LV1");
				players[id].show();
			}
			else if (id >= MAX_NPC / 4 && id < (MAX_NPC / 4 * 2))
			{
				players[id] = OBJECT{ *monster, 455, 0, 65, 65 };
				players[id].id = id;
				players[id].move(my_packet->x, my_packet->y);
				players[id].set_name("LV2");
				players[id].show();
			}
			else if (id >= (MAX_NPC / 4 * 2) && id < (MAX_NPC / 4 * 3))
			{
				players[id] = OBJECT{ *monster, 0, 260, 65, 65 };
				players[id].id = id;
				players[id].move(my_packet->x, my_packet->y);
				players[id].set_name("LV3");
				players[id].show();
			}
			else
			{
				players[id] = OBJECT{ *monster, 260, 260, 65, 65 };
				players[id].id = id;
				players[id].move(my_packet->x, my_packet->y);
				players[id].set_name("LV4");
				players[id].show();
			}
		}
		break;
	}
	case SC_MOVE_OBJECT:
	{
		SC_MOVE_OBJECT_PACKET* my_packet = reinterpret_cast<SC_MOVE_OBJECT_PACKET*>(ptr);
		int other_id = my_packet->id;
		if (other_id == g_myid) {
			avatar.move(my_packet->x, my_packet->y);
			g_left_x = my_packet->x - SCREEN_WIDTH / 2;
			g_top_y = my_packet->y - SCREEN_HEIGHT / 2;
		}
		else {
			players[other_id].move(my_packet->x, my_packet->y);
		}
		break;
	}

	case SC_REMOVE_OBJECT:
	{
		SC_REMOVE_OBJECT_PACKET* my_packet = reinterpret_cast<SC_REMOVE_OBJECT_PACKET*>(ptr);
		int other_id = my_packet->id;
		if (other_id == g_myid) {
			avatar.hide();
		}
		else {
			players.erase(other_id);
		}
		break;
	}
	case SC_CHAT:
	{
		SC_CHAT_PACKET* my_packet = reinterpret_cast<SC_CHAT_PACKET*>(ptr);
		int other_id = my_packet->id;

		avatar.system_chat(my_packet->mess);

		break;
	}
	case SC_STAT_CHANGE:
	{
		SC_STAT_CHANGEL_PACKET* pc = reinterpret_cast<SC_STAT_CHANGEL_PACKET*>(ptr);
		int other_id = pc->id;

		if (other_id == g_myid) {
			avatar._hp = pc->hp;
			avatar.max_hp = pc->max_hp;
			avatar._mp = pc->mp;
			avatar.max_mp = pc->max_mp;
			avatar._exp = pc->exp;
			avatar.max_exp = pc->max_exp;
			avatar._level = pc->level;
		}
		else
		{
		}
		break;
	}

	default:
		printf("Unknown PACKET type [%d]\n", ptr[1]);
	}
}

void process_data(char* net_buf, size_t io_byte)
{
	char* ptr = net_buf;
	static size_t in_packet_size = 0;
	static size_t saved_packet_size = 0;
	static char packet_buffer[BUF_SIZE];

	while (0 != io_byte) {
		if (0 == in_packet_size) in_packet_size = ptr[0];
		if (io_byte + saved_packet_size >= in_packet_size) {
			memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
			ProcessPacket(packet_buffer);
			ptr += in_packet_size - saved_packet_size;
			io_byte -= in_packet_size - saved_packet_size;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else {
			memcpy(packet_buffer + saved_packet_size, ptr, io_byte);
			saved_packet_size += io_byte;
			io_byte = 0;
		}
	}
}

void client_main()
{
	char net_buf[BUF_SIZE];
	size_t	received;

	auto recv_result = s_socket.receive(net_buf, BUF_SIZE, received);
	if (recv_result == sf::Socket::Error)
	{
		wcout << L"Recv 에러!";
		//exit(-1);
	}
	if (recv_result == sf::Socket::Disconnected) {
		wcout << L"Disconnected\n";
		//exit(-1);
	}
	if (recv_result != sf::Socket::NotReady)
		if (received > 0) process_data(net_buf, received);

	for (int i = 0; i < SCREEN_WIDTH; ++i)
		for (int j = 0; j < SCREEN_HEIGHT; ++j)
		{
			int tile_x = i + g_left_x;
			int tile_y = j + g_top_y;
			if ((tile_x < 0) || (tile_y < 0)) continue;
			
			if (MAPDATA->GetTile(tile_y,tile_x) == MAPDATA->MAP_TYPE::e_PLAT) {
				plat_tile.a_move(TILE_WIDTH * i, TILE_WIDTH * j);
				plat_tile.a_draw();
			}
			else if (MAPDATA->GetTile(tile_y, tile_x) == MAPDATA->MAP_TYPE::e_OBSTACLE)
			{
				obstacle_tile.a_move(TILE_WIDTH * i, TILE_WIDTH * j);
				obstacle_tile.a_draw();
			}
			else if (MAPDATA->GetTile(tile_y, tile_x) == MAPDATA->MAP_TYPE::e_BTOWN)
			{
				Btown_tile.a_move(TILE_WIDTH * i, TILE_WIDTH * j);
				Btown_tile.a_draw();
			}
			else if (MAPDATA->GetTile(tile_y, tile_x) == MAPDATA->MAP_TYPE::e_GTOWN)
			{
				Gtown_tile.a_move(TILE_WIDTH * i, TILE_WIDTH * j);
				Gtown_tile.a_draw();
			}
		}
	avatar.draw();
	for (auto& pl : players) pl.second.draw();
	sf::Text text;
	text.setFont(g_font);
	text.setFillColor(sf::Color(255, 255, 255));
	text.setStyle(sf::Text::Bold);
	char buf[100];
	sprintf_s(buf, "(%d, %d)", avatar.m_x, avatar.m_y);
	text.setString(buf);
	g_window->draw(text);

	string hp = to_string(avatar._hp);
	string _hp = "HP : ";
	_hp += hp;
	avatar.set_hp(_hp.c_str());

	string maxhp = to_string(avatar.max_hp);
	string _maxhp = " / ";
	_maxhp += maxhp;
	avatar.set_maxhp(_maxhp.c_str());

	string mp = to_string(avatar._mp);
	string _mp = "MP : ";
	_mp += mp;
	avatar.set_mp(_mp.c_str());

	string maxmp = to_string(avatar.max_mp);
	string _maxmp = " / ";
	_maxmp += maxmp;
	avatar.set_maxmp(_maxmp.c_str());

	string lv = to_string(avatar._level);
	string a = "LV : ";
	a += lv;
	avatar.set_level(a.c_str());

	string maxexp = to_string(avatar.max_exp);
	string _maxexp = " / ";
	_maxexp += maxexp;
	avatar.set_maxexp(_maxexp.c_str());

	string exp = to_string(avatar._exp);
	string _exp = "EXP : ";
	_exp += exp;
	avatar.set_exp(_exp.c_str());
	//avatar.set_exp(teemp);
	//avatar.set_maxexp("300");

	sf::RectangleShape hpBoxInner;
	sf::RectangleShape mpBoxInner;
	sf::RectangleShape expBoxInner;


	hpBoxInner.setSize(sf::Vector2f(((100.f * avatar._hp / avatar.max_hp) * 3), 30));
	hpBoxInner.setFillColor(sf::Color(255, 0, 0));
	//hpBoxInner.setPosition(hpBox.getPosition());
	hpBoxInner.setPosition(1050, 400);
	g_window->draw(hpBoxInner);

	mpBoxInner.setSize(sf::Vector2f(((100.f * avatar._mp / avatar.max_mp) * 3), 30));
	mpBoxInner.setFillColor(sf::Color(0, 0, 255));
	mpBoxInner.setPosition(1050, 500);
	//mpBoxInner.setPosition(mpBox.getPosition());

	expBoxInner.setSize(sf::Vector2f(((100.f * avatar._exp / avatar.max_exp) * 3), 30));
	expBoxInner.setFillColor(sf::Color(255, 0, 255));
	expBoxInner.setPosition(1050, 600);
	//expBoxInner.setPosition(expBox.getPosition());


	g_window->draw(mpBoxInner);
	g_window->draw(expBoxInner);

}

void send_packet(void* packet)
{
	unsigned char* p = reinterpret_cast<unsigned char*>(packet);
	size_t sent = 0;
	s_socket.send(packet, p[0], sent);
}

int main()
{
	string userid;
	cout << " User ID를 입력하시오 : ";
	cin >> userid;

	wcout.imbue(locale("korean"));
	sf::Socket::Status status = s_socket.connect("127.0.0.1", PORT_NUM);
	s_socket.setBlocking(false);

	if (status != sf::Socket::Done) {
		wcout << L"서버와 연결할 수 없습니다.\n";
		//exit(-1);
	}

	MapData* mapData = MapData::GetInstance();
	
	client_initialize();
	CS_LOGIN_PACKET p;
	p.size = sizeof(p);
	p.type = CS_LOGIN;
	strcpy_s(p.name, userid.c_str());

	//string player_name{ "P" };
	//player_name += to_string(GetCurrentProcessId());
	//strcpy_s(p.name, player_name.c_str());
	send_packet(&p);
	avatar.set_name(p.name);

	sf::RenderWindow window(sf::VideoMode(1365, WINDOW_HEIGHT), "2D CLIENT");
	g_window = &window;

	while (window.isOpen())
	{

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed) {
				int direction = -1;
				switch (event.key.code) {
				case sf::Keyboard::Left:
					if (avatar.move_start_time <= chrono::system_clock::now())
					{
						direction = 2;
						avatar.move_start_time = chrono::system_clock::now() + chrono::milliseconds(500);
					}
					break;
				case sf::Keyboard::Right:
					if (avatar.move_start_time <= chrono::system_clock::now())
					{
						direction = 3;
						avatar.move_start_time = chrono::system_clock::now() + chrono::milliseconds(500);
					}
					break;
				case sf::Keyboard::Up:
					if (avatar.move_start_time <= chrono::system_clock::now())
					{
						direction = 0;
						avatar.move_start_time = chrono::system_clock::now() + chrono::milliseconds(500);
					}
					break;
				case sf::Keyboard::Down:
					if (avatar.move_start_time <= chrono::system_clock::now())
					{
						direction = 1;
						avatar.move_start_time = chrono::system_clock::now() + chrono::milliseconds(500);
					}
					break;
				case sf::Keyboard::Escape:
					window.close();
					break;
				case sf::Keyboard::Space:
					if (avatar.attack_start_time <= chrono::system_clock::now())
					{
						avatar.attacktime();
						CS_ATTACK_PACKET p;
						p.size = sizeof(p);
						p.type = CS_ATTACK;
						send_packet(&p);
						avatar.attack_start_time = chrono::system_clock::now() + chrono::seconds(1);

					}
					break;
				}
				if (-1 != direction) {
					CS_MOVE_PACKET p;
					p.size = sizeof(p);
					p.type = CS_MOVE;
					p.direction = direction;
					send_packet(&p);
				}

			}
		}

		window.clear();
		client_main();
		window.display();
	}
	client_finish();

	return 0;
}