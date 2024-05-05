#include "pch.h"
#include "utils.h"
#include "MapData.h"
#include "SFSystem.h"
#include "Object.h"
#include "Client.h"
#include "Player.h"

#include "protocol.h"

sf::TcpSocket s_socket;
sf::RenderWindow* g_window;
sf::Font g_font;

unordered_map <int, Client> players;
Player avatar;

uint16	g_left_x;
uint16	g_top_y;
uint16	g_myid;

//class OBJECT {
//private:
//	
//	sf::Text m_information[10];
//	sf::Text m_name;
//	sf::Text m_chat;
//
//	chrono::system_clock::time_point m_mess_end_time;
//public:
//
//	char name[NAME_SIZE];
//
//	void set_name(const char str[]) {
//		m_name.setFont(g_font);
//		m_name.setString(str);
//		m_name.setStyle(sf::Text::Bold);
//		if (id < MAX_USER) m_name.setFillColor(sf::Color(255, 255, 255));
//		else if (id > MAX_USER && id < MAX_NPC / 4)
//		{
//			m_name.setFillColor(sf::Color(255, 0, 255));
//		}
//		else if (id >= MAX_NPC / 4 && id < (MAX_NPC / 4 * 2))
//		{
//			m_name.setFillColor(sf::Color(255, 255, 0));
//		}
//		else if (id >= (MAX_NPC / 4 * 2) && id < (MAX_NPC / 4 * 3))
//		{
//			m_name.setFillColor(sf::Color(255, 255, 255));
//		}
//		else
//		{
//			m_name.setFillColor(sf::Color(0, 255, 0));
//		}
//	}
//};

void ProcessPacket(char* ptr)
{
	static bool first_time = true;
	switch (ptr[1])
	{
	case SC_LOGIN_OK:
	{
		SC_LOGIN_OK_PACKET* packet = reinterpret_cast<SC_LOGIN_OK_PACKET*>(ptr);

		STAT st;
		POS ps;
		TP tp;

		st.exp = packet->exp;
		st.hp = packet->hp;
		st.level = packet->level;
		st.maxExp = packet->max_exp;
		st.maxHp = packet->max_hp;
		st.maxMp = packet->max_mp;
		st.mp = packet->mp;
		ps.posx = packet->x;
		ps.posy = packet->y;
		tp.attackEndTime = chrono::system_clock::now();
		tp.attackTime = chrono::system_clock::now();
		tp.moveTime = chrono::system_clock::now();

		Player avatar(*SFSYSTEM->player_attack, *SFSYSTEM->player, 0, 0, 65, 65, st, ps, tp, packet->id, packet->name);

		
		g_left_x = packet->x - SCREEN_WIDTH / 2;
		g_top_y = packet->y - SCREEN_HEIGHT / 2;

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

		POS ps;
		ps.posx = my_packet->x;
		ps.posy = my_packet->y;

		if (id == g_myid) {
			avatar.Move(my_packet->x, my_packet->y);
			g_left_x = my_packet->x - SCREEN_WIDTH / 2;
			g_top_y = my_packet->y - SCREEN_HEIGHT / 2;
		}
		else if (id < MAX_USER) {
			players[id] = Client{ *SFSYSTEM->player, ps, 0, 0, 65, 65, id, my_packet->name};
		}
		else {
			if (id > MAX_USER && id < MAX_NPC / 4) 
				players[id] = Client{ *SFSYSTEM->monster, ps, 0, 0, 65, 65, id, "LV_1"};
			else if (id >= MAX_NPC / 4 && id < (MAX_NPC / 4 * 2))
				players[id] = Client{ *SFSYSTEM->monster, ps, 0, 0, 65, 65, id, "LV_2" };
			else if (id >= (MAX_NPC / 4 * 2) && id < (MAX_NPC / 4 * 3))
				players[id] = Client{ *SFSYSTEM->monster, ps, 0, 0, 65, 65, id, "LV_3" };
			else
				players[id] = Client{ *SFSYSTEM->monster, ps, 0, 0, 65, 65, id, "LV_4" };
		}
		break;
	}
	case SC_MOVE_OBJECT:
	{
		SC_MOVE_OBJECT_PACKET* my_packet = reinterpret_cast<SC_MOVE_OBJECT_PACKET*>(ptr);
		int other_id = my_packet->id;
		if (other_id == g_myid) {
			avatar.Move(my_packet->x, my_packet->y);
			g_left_x = my_packet->x - SCREEN_WIDTH / 2;
			g_top_y = my_packet->y - SCREEN_HEIGHT / 2;
		}
		else {
			players[other_id].Move(my_packet->x, my_packet->y);
		}
		break;
	}

	case SC_REMOVE_OBJECT:
	{
		SC_REMOVE_OBJECT_PACKET* my_packet = reinterpret_cast<SC_REMOVE_OBJECT_PACKET*>(ptr);
		int other_id = my_packet->id;
		if (other_id == g_myid) {
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

		SFSYSTEM->SetChat(my_packet->mess);

		break;
	}
	case SC_STAT_CHANGE:
	{
		SC_STAT_CHANGEL_PACKET* pc = reinterpret_cast<SC_STAT_CHANGEL_PACKET*>(ptr);
		int other_id = pc->id;

		STAT st;
		st.hp = pc->hp;
		st.maxHp = pc->max_hp;
		st.mp = pc->mp;
		st.maxMp = pc->max_mp;
		st.exp = pc->exp;
		st.maxExp = pc->max_exp;
		st.level = pc->level;

		if (other_id == g_myid) {
			avatar.SetStat(st);
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

	
	//for (auto& pl : players) pl.second.Draw();
	
	char buf[100];
	sprintf_s(buf, "(%d, %d)", avatar.GetPos().posx, avatar.GetPos().posy);

	SFSYSTEM->SetText(SystemText::PLAYERPOS, buf);
	SFSYSTEM->SetInnerBoxSize(SystemBox::HPINNERBOX, avatar.GetStat().hp, avatar.GetStat().maxHp);
	SFSYSTEM->SetInnerBoxSize(SystemBox::MPINNERBOX, avatar.GetStat().mp, avatar.GetStat().maxMp);
	SFSYSTEM->SetInnerBoxSize(SystemBox::EXPINNERBOX, avatar.GetStat().exp, avatar.GetStat().maxExp);

	string hp = to_string(avatar.GetStat().hp);
	string _hp = "HP : ";
	_hp += hp;

	SFSYSTEM->SetText(SystemText::HP, _hp.c_str());

	string maxhp = to_string(avatar.GetStat().maxHp);
	string _maxhp = " / ";
	_maxhp += maxhp;
	
	SFSYSTEM->SetText(SystemText::MAXHP, _maxhp.c_str());

	string mp = to_string(avatar.GetStat().mp);
	string _mp = "MP : ";
	_mp += mp;
	SFSYSTEM->SetText(SystemText::MP, _mp.c_str());

	string maxmp = to_string(avatar.GetStat().maxMp);
	string _maxmp = " / ";
	_maxmp += maxmp;
	SFSYSTEM->SetText(SystemText::MAXMP, _maxmp.c_str());

	string lv = to_string(avatar.GetStat().level);
	string _lv = "LV : ";
	_lv += lv;
	SFSYSTEM->SetText(SystemText::LEVEL, _lv.c_str());

	string exp = to_string(avatar.GetStat().exp);
	string _exp = "EXP : ";
	_exp += exp;
	SFSYSTEM->SetText(SystemText::EXP, exp.c_str());

	string maxexp = to_string(avatar.GetStat().maxExp);
	string _maxexp = " / ";
	_maxexp += maxexp;
	SFSYSTEM->SetText(SystemText::MAXEXP, maxexp.c_str());

	
	SFSYSTEM->Draw(); 
	OBJECT->TileDraw();

	avatar.Draw();
	for (auto& a : players)
	{
		a.second.Draw();
	}

}

void send_packet(void* packet)
{
	unsigned char* p = reinterpret_cast<unsigned char*>(packet);
	size_t sent = 0;
	s_socket.send(packet, p[0], sent);
}




int main()
{
	MapData* mapData = MapData::GetInstance();
	SFSystem* sfml = SFSystem::GetInstance();
	Object* object = Object::GetInstance();
	
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

	CS_LOGIN_PACKET p;
	p.size = sizeof(p);
	p.type = CS_LOGIN;
	strcpy_s(p.name, userid.c_str());

	send_packet(&p);
	
	sf::RenderWindow window(sf::VideoMode(1365, WINDOW_HEIGHT), "2D CLIENT");
	SFSYSTEM->window = &window;

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
					if (avatar.GetTP().moveTime <= chrono::system_clock::now())
					{
						direction = 2;
						avatar.GetTP().moveTime = chrono::system_clock::now() + chrono::milliseconds(500);
					}
					break;
				case sf::Keyboard::Right:
					if (avatar.GetTP().moveTime <= chrono::system_clock::now())
					{
						direction = 3;
						avatar.GetTP().moveTime = chrono::system_clock::now() + chrono::milliseconds(500);
					}
					break;
				case sf::Keyboard::Up:
					if (avatar.GetTP().moveTime <= chrono::system_clock::now())
					{
						direction = 0;
						avatar.GetTP().moveTime = chrono::system_clock::now() + chrono::milliseconds(500);
					}
					break;
				case sf::Keyboard::Down:
					if (avatar.GetTP().moveTime <= chrono::system_clock::now())
					{
						direction = 1;
						avatar.GetTP().moveTime = chrono::system_clock::now() + chrono::milliseconds(500);
					}
					break;
				case sf::Keyboard::Escape:
					window.close();
					break;
				case sf::Keyboard::Space:
					if (avatar.GetTP().moveTime <= chrono::system_clock::now())
					{
						avatar.SetAttackTime();
						CS_ATTACK_PACKET p;
						p.size = sizeof(p);
						p.type = CS_ATTACK;
						send_packet(&p);

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
	
	return 0;
}