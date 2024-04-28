#define _CRT_SECURE_NO_WARNINGS
#include "pch.h"
#include <iostream>
#include <array>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <thread>
#include <vector> // 템플릿 라이브러리들
#include <mutex>
#include <random>
#include <chrono>
#include <unordered_set>
#include <Windows.h>
#include <concurrent_priority_queue.h>
#include <string>
#include <sqlext.h>
#include <locale>

#include "protocol.h"

extern "C"
{
#include "include\lua.h"
#include "include\lauxlib.h"
#include "include\lualib.h"
}
#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")
//#pragma comment(lib, "lua54.lib")
using namespace std;

constexpr int NAME_LEN = 20;
constexpr int VIEW_RANGE = 7;

bool CheckDataBase(CS_LOGIN_PACKET p, int c_id);
void InitDataBase();
void SaveDB(int x);
SQLHENV henv;
SQLHDBC hdbc;
SQLHSTMT hstmt = 0;
SQLRETURN retcode;

SQLWCHAR szUser_ID[NAME_LEN];

SQLLEN cbLoginID = 0;
SQLLEN cbX, cbY = 0, cbHP = 0, cbEXP = 0, cbLEVEL = 0, cbMp = 0;
SQLLEN cbMaxHp = 0, cbMaxMp = 0, cbMaxExp = 0;

SQLINTEGER  pos_x, pos_y, user_lv, user_hp, user_maxhp;
SQLINTEGER user_mp, user_exp, user_maxexp, user_maxmp;

SQLLEN cb0 = 0, cb1 = 0;
SQLLEN cb2 = 0, cb4 = 0;
SQLLEN cb3 = 0, cb5 = 0;
SQLLEN cb6 = 0, cb7 = 0;
SQLLEN cb8 = 0;

char wmap[2000][2000];
void Attack(int m_id);
void show_error(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode)
{
	SQLSMALLINT iRec = 0;
	SQLINTEGER iError;
	WCHAR wszMessage[1000];
	WCHAR wszState[SQL_SQLSTATE_SIZE + 1];
	if (RetCode == SQL_INVALID_HANDLE) {
		fwprintf(stderr, L"Invalid handle!\n");
		return;
	}
	while (SQLGetDiagRec(hType, hHandle, ++iRec, wszState, &iError, wszMessage,
		(SQLSMALLINT)(sizeof(wszMessage) / sizeof(WCHAR)), (SQLSMALLINT*)NULL) == SQL_SUCCESS) {
		// Hide data truncated..
		if (wcsncmp(wszState, L"01004", 5)) {
			fwprintf(stderr, L"[%5.5s] %s (%d)\n", wszState, wszMessage, iError);
		}
	}
}

enum MAP_TYPE
{
	e_PLAT,
	e_OBSTACLE,
	e_BTOWN,
	e_GTOWN
};

char townmap[20][20]
{
	'3','3','3','3','3','3','3','3','1','1','1','3','3','3','3','3','3','3','3','3',
	'3','3','3','3','3','3','3','3','1','1','1','3','3','3','3','3','3','3','3','3',
	'3','3','2','2','2','2','2','2','1','1','1','2','2','2','2','2','2','2','3','3',
	'3','3','2','2','2','2','2','2','1','1','1','2','2','2','2','2','2','2','3','3',
	'3','3','2','2','2','2','2','2','1','1','1','2','2','2','2','2','2','2','3','3',
	'3','3','2','2','2','2','2','2','1','1','1','2','2','2','2','2','2','2','3','3',
	'3','3','2','2','2','2','2','2','1','1','1','2','2','2','2','2','2','2','3','3',
	'3','3','2','2','2','2','2','2','1','1','1','2','2','2','2','2','2','2','3','3',
	'1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1',
	'1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1',
	'1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1','1',
	'3','3','2','2','2','2','2','2','1','1','1','2','2','2','2','2','2','2','3','3',
	'3','3','2','2','2','2','2','2','1','1','1','2','2','2','2','2','2','2','3','3',
	'3','3','2','2','2','2','2','2','1','1','1','2','2','2','2','2','2','2','3','3',
	'3','3','2','2','2','2','2','2','1','1','1','2','2','2','2','2','2','2','3','3',
	'3','3','2','2','2','2','2','2','1','1','1','2','2','2','2','2','2','2','3','3',
	'3','3','2','2','2','2','2','2','1','1','1','2','2','2','2','2','2','2','3','3',
	'3','3','2','2','2','2','2','2','1','1','1','2','2','2','2','2','2','2','3','3',
	'3','3','3','3','3','3','3','3','1','1','1','3','3','3','3','3','3','3','3','3',
	'3','3','3','3','3','3','3','3','1','1','1','3','3','3','3','3','3','3','3','3'

};

void Init_mapdata()
{
	char data;
	FILE* fp = fopen("mapdata.txt", "rb");

	int cnt = 0;

	while (fscanf(fp, "%c", &data) != EOF)
	{
		switch (data)
		{
		case '0':
			wmap[cnt / 2000][cnt % 2000] = e_PLAT;
			cnt++;
			break;
		case '3':
			wmap[cnt / 2000][cnt % 2000] = e_OBSTACLE;
			cnt++;
			break;
		}
	}

	for (int i = 0; i < 20; ++i)
	{
		for (int j = 0; j < 20; ++j)
		{
			char data = townmap[i][j];
			switch (data)
			{
			case '0':
				wmap[i + 1000][j + 1000] = e_PLAT;
				break;
			case '1':
				wmap[i + 1000][j + 1000] = e_BTOWN;
				break;
			case '2':
				wmap[i + 1000][j + 1000] = e_GTOWN;
				break;
			case '3':
				wmap[i + 1000][j + 1000] = e_OBSTACLE;
				break;
			}
		}
	}
}

enum COMP_TYPE { OP_ACCEPT, OP_RECV, OP_SEND, OP_NPC_MOVE, OP_SLEEP, OP_NPC_ATTACK, OP_HEAL, EV_RANDOM_MOVE, EV_NPC_RESPWAN, OP_SELF_HEAL, OP_RESPAWN, OP_PLAYER_RESPAWN };
struct TIMER_EVENT {
	int obj_id;
	chrono::system_clock::time_point wakeup_time;
	COMP_TYPE event_id;
	int target_id;
	constexpr bool operator < (const TIMER_EVENT& L) const
	{
		return (wakeup_time > L.wakeup_time);
	}
};
concurrency::concurrent_priority_queue<TIMER_EVENT> timer_queue;
mutex timer_lock;

class OVER_EXP {
public:
	WSAOVERLAPPED _over;
	WSABUF _wsabuf;
	char _send_buf[BUF_SIZE];
	COMP_TYPE _comp_type;
	OVER_EXP()
	{
		_wsabuf.len = BUF_SIZE;
		_wsabuf.buf = _send_buf;
		_comp_type = OP_RECV;
		ZeroMemory(&_over, sizeof(_over));
	}
	OVER_EXP(char* packet)
	{
		_wsabuf.len = packet[0];
		_wsabuf.buf = _send_buf;
		ZeroMemory(&_over, sizeof(_over));
		_comp_type = OP_SEND;
		memcpy(_send_buf, packet, packet[0]);
	}
};

enum S_STATE { ST_FREE, ST_ALLOC, ST_INGAME };
class SESSION {
	OVER_EXP _recv_over;

public:
	mutex _s_lock;
	S_STATE _state;
	atomic_bool	_is_active;		// 주위에 플레이어가 있는가?
	int _id;
	SOCKET _socket;
	short	x, y;
	char	_name[NAME_SIZE];
	int level;
	int hp;
	int maxhp;
	int mp;
	int maxmp;
	int exp;
	int maxexp;
	int		_prev_remain;
	unordered_set <int> _view_list;
	mutex	_vl;
	int		last_move_time;
	chrono::system_clock::time_point next_move_time;
	chrono::system_clock::time_point sleep_time;
public:
	SESSION()
	{
		_id = -1;
		_socket = 0;
		x = y = 0;
		_name[0] = 0;
		_state = ST_FREE;
		_prev_remain = 0;
		next_move_time = chrono::system_clock::now() + chrono::seconds(1);
	}

	~SESSION() {}

	void do_recv()
	{
		DWORD recv_flag = 0;
		memset(&_recv_over._over, 0, sizeof(_recv_over._over));
		_recv_over._wsabuf.len = BUF_SIZE - _prev_remain;
		_recv_over._wsabuf.buf = _recv_over._send_buf + _prev_remain;
		WSARecv(_socket, &_recv_over._wsabuf, 1, 0, &recv_flag,
			&_recv_over._over, 0);
	}

	void do_send(void* packet)
	{
		OVER_EXP* sdata = new OVER_EXP{ reinterpret_cast<char*>(packet) };
		WSASend(_socket, &sdata->_wsabuf, 1, 0, 0, &sdata->_over, 0);
	}
	void send_login_info_packet()
	{
		SC_LOGIN_INFO_PACKET p;
		p.id = _id;
		p.size = sizeof(SC_LOGIN_INFO_PACKET);
		p.type = SC_LOGIN_INFO;
		p.x = x;
		p.y = y;
		do_send(&p);
	}

	void send_login_ok_pakcet()
	{
		SC_LOGIN_OK_PACKET p;
		p.id = _id;
		p.size = sizeof(SC_LOGIN_OK_PACKET);
		p.type = SC_LOGIN_OK;
		p.x = x;
		p.y = y;
		p.hp = hp;
		p.mp = mp;
		p.exp = exp;
		p.max_hp = maxhp;
		p.max_mp = maxmp;
		p.max_exp = maxexp;
		p.level = level;
		//strcpy_s(p.name, _name);

		do_send(&p);
	}

	void send_move_packet(int c_id);
	void send_add_player_packet(int c_id);
	void send_chat_packet(int c_id, const char* mess);
	void send_statchange_packet(int c_id);
	void send_remove_player_packet(int c_id)
	{
		_vl.lock();
		if (_view_list.count(c_id))
			_view_list.erase(c_id);
		else {
			_vl.unlock();
			return;
		}
		_vl.unlock();
		SC_REMOVE_OBJECT_PACKET p;
		p.id = c_id;
		p.size = sizeof(p);
		p.type = SC_REMOVE_OBJECT;
		do_send(&p);
	}

	void send_login_fail_packet()
	{
		SC_LOGIN_FAIL_PACKET p;
		p.size = sizeof(SC_LOGIN_FAIL_PACKET);
		p.type = SC_LOGIN_FAIL;

		do_send(&p);
	}
};

HANDLE h_iocp;
array<SESSION, MAX_USER + MAX_NPC> clients;

SOCKET g_s_socket, g_c_socket;
OVER_EXP g_a_over;

bool is_pc(int object_id)
{
	return object_id < MAX_USER;
}

bool is_npc(int object_id)
{
	return !is_pc(object_id);
}

bool can_see(int from, int to)
{
	if (abs(clients[from].x - clients[to].x) > VIEW_RANGE) return false;
	return abs(clients[from].y - clients[to].y) <= VIEW_RANGE;
}

void SESSION::send_move_packet(int c_id)
{
	SC_MOVE_OBJECT_PACKET p;
	p.id = c_id;
	p.size = sizeof(SC_MOVE_OBJECT_PACKET);
	p.type = SC_MOVE_OBJECT;
	p.x = clients[c_id].x;
	p.y = clients[c_id].y;
	p.move_time = clients[c_id].last_move_time;
	do_send(&p);
}

void SESSION::send_add_player_packet(int c_id)
{
	SC_ADD_OBJECT_PACKET add_packet;
	add_packet.id = c_id;
	strcpy_s(add_packet.name, clients[c_id]._name);
	add_packet.size = sizeof(add_packet);
	add_packet.type = SC_ADD_OBJECT;
	add_packet.x = clients[c_id].x;
	add_packet.y = clients[c_id].y;
	_vl.lock();
	_view_list.insert(c_id);
	_vl.unlock();
	do_send(&add_packet);
}

void SESSION::send_statchange_packet(int c_id)
{
	SC_STAT_CHANGEL_PACKET cg;
	cg.id = c_id;
	cg.size = sizeof(cg);
	cg.type = SC_STAT_CHANGE;
	cg.level = clients[c_id].level;
	cg.exp = clients[c_id].exp;
	cg.max_exp = clients[c_id].maxexp;
	cg.hp = clients[c_id].hp;
	cg.max_hp = clients[c_id].maxhp;
	cg.mp = clients[c_id].mp;
	cg.max_mp = clients[c_id].maxmp;
	do_send(&cg);
}
void SESSION::send_chat_packet(int p_id, const char* mess)
{
	SC_CHAT_PACKET packet;
	packet.id = p_id;
	packet.size = sizeof(packet);
	packet.type = SC_CHAT;
	strcpy_s(packet.mess, mess);
	do_send(&packet);
}

//void timerset(int m_id, int time, COMP_TYPE ET)
//{
//	timer_lock.lock();
//	TIMER_EVENT ev;
//	ev.event_id = ET;
//	ev.obj_id = m_id;
//	ev.wakeup_time = chrono::system_clock::now() + chrono::milliseconds(time);
//	timer_queue.push(ev);
//	timer_lock.unlock();
//}

int get_new_client_id()
{
	for (int i = 0; i < MAX_USER; ++i) {
		lock_guard <mutex> ll{ clients[i]._s_lock };
		if (clients[i]._state == ST_FREE) {
			return i;
		}
	}
	return -1;
}

void WakeUpNPC(int npc_id)
{
	if (is_pc(npc_id)) {
		cout << "ERROR" << endl;
	}
	if (clients[npc_id]._is_active) return; // 이미깨있다? return
	bool old_state = false;
	if (false == atomic_compare_exchange_strong(&clients[npc_id]._is_active, &old_state, true)) return; // 한쓰레드만 께워야해서 CAS 사용 / 실패하면 return 성공했으면 깨운사람이 책임지고 이동이벤트 만들기
	TIMER_EVENT ev{ npc_id, chrono::system_clock::now(), EV_RANDOM_MOVE, 0 };  //깨우는게 중첩이 된다는건 이벤트가 겹겹이 쌓인다. 한번깨웠으면 또 꺠우면안된다.
	timer_queue.push(ev);
}

void process_packet(int c_id, char* packet)
{
	switch (packet[1]) {
	case CS_LOGIN: {
		CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(packet);

		//if(p->name) 아이디 체크해서 DUMMY가 들어가있으면 CheckDataBase말고 
		//그냥 흩뿌려서 돌아
		if (CheckDataBase(*p, c_id))
		{
			lock_guard<mutex> ll{ clients[c_id]._s_lock };
			clients[c_id]._state = ST_INGAME;
			clients[c_id].send_login_ok_pakcet();
			string temp = clients[c_id]._name;
			string ttemp = "";

			for (int i = 0; i < 5; ++i)
			{
				ttemp += temp[i];
			}
			if (ttemp == "DUMMY")
			{

			}
			else
			{
				if (clients[c_id].hp < clients[c_id].maxhp)
				{
					TIMER_EVENT ev{ c_id, chrono::system_clock::now() + 5s, OP_SELF_HEAL, 0 };
					timer_queue.push(ev);
				}
			}


			for (auto& pl : clients) {

				//lock_guard<mutex> ll(pl._s_lock); //여기 check
				//pl._s_lock.lock();
				if (ST_INGAME != pl._state) {
					//pl._s_lock.unlock();
					continue;
				}

				if (pl._id == c_id) {
					//pl._s_lock.unlock();
					continue;
				}
				if (false == can_see(c_id, pl._id)) {
					//pl._s_lock.unlock();
					continue;
				}



				if (is_pc(pl._id)) pl.send_add_player_packet(c_id); // 새로 접속한 플레이어 주위에 있는게 pc가 아니면 npc니까 wakeupnpc로 깨워준다.
				else {
					string temp = clients[c_id]._name;
					string ttemp = "";
					for (int i = 0; i < 5; ++i)
					{
						ttemp += temp[i];
					}
					if (ttemp == "DUMMY")
						break;
					else
						WakeUpNPC(pl._id);
				}
				clients[c_id].send_add_player_packet(pl._id);
				//pl._s_lock.unlock();
			}

		}
		else
		{
			clients[c_id].send_login_fail_packet();
		}

		break;
	}
	case CS_MOVE: {
		CS_MOVE_PACKET* p = reinterpret_cast<CS_MOVE_PACKET*>(packet);
		clients[c_id].last_move_time = p->move_time;
		short x = clients[c_id].x;
		short y = clients[c_id].y;
		switch (p->direction) {
		case 0: if (y > 0 && wmap[y - 1][x] != MAP_TYPE::e_OBSTACLE) y--; break;
		case 1: if (y < (W_HEIGHT - 1) && wmap[y + 1][x] != MAP_TYPE::e_OBSTACLE) y++; break;
		case 2: if (x > 0 && wmap[y][x - 1] != MAP_TYPE::e_OBSTACLE) x--; break;
		case 3:if (x < (W_WIDTH - 1) && wmap[y][x + 1] != MAP_TYPE::e_OBSTACLE) x++; break;
		}
		clients[c_id].x = x;
		clients[c_id].y = y;


		unordered_set<int> near_list;
		clients[c_id]._vl.lock();
		unordered_set<int> old_vlist = clients[c_id]._view_list;
		clients[c_id]._vl.unlock();
		for (auto& cl : clients) {
			if (cl._state != ST_INGAME) continue;
			if (cl._id == c_id) continue;
			if (can_see(c_id, cl._id)) {
				near_list.insert(cl._id);


				if (cl._id > MAX_USER)
				{
					if (clients[c_id].x == clients[cl._id].x &&
						clients[c_id].y == clients[cl._id].y)
					{
						clients[c_id].hp -= clients[cl._id].level * 5;

						char mess[50];
						sprintf_s(mess, "%s %d Damage ", clients[c_id]._name, clients[cl._id].level * 5);
						clients[c_id].send_chat_packet(c_id, mess);


						for (int i = 0; i < MAX_USER; ++i)
						{
							if (i == c_id)
								continue;
							if (clients[i]._state == ST_INGAME)
							{
								unordered_set <int> vl = clients[c_id]._view_list;
								for (auto other_id : vl)
								{
									clients[i].send_chat_packet(c_id, mess);
									break;
								}
							}
						}

						clients[c_id].send_statchange_packet(c_id);
						if (clients[c_id].hp <= 0)
						{
							clients[c_id]._state = ST_FREE;
							TIMER_EVENT ev{ c_id, chrono::system_clock::now(), OP_RESPAWN, 0 };
							timer_queue.push(ev);
						}

					}
				}


			}
		}

		clients[c_id].send_move_packet(c_id);

		for (auto& pl : near_list) {
			auto& cpl = clients[pl];
			if (is_pc(pl)) {

				cpl._vl.lock();
				if (clients[pl]._view_list.count(c_id)) {
					cpl._vl.unlock();
					clients[pl].send_move_packet(c_id); // pc면 패킷을 보내서 내 이동을 알리고
				}
				else {
					cpl._vl.unlock();
					clients[pl].send_add_player_packet(c_id);
				}
			}
			else {
				string temp = clients[c_id]._name;
				string ttemp = "";
				for (int i = 0; i < 5; ++i)
				{
					ttemp += temp[i];
				}
				if (ttemp == "DUMMY")
					break;
				else
					WakeUpNPC(pl); // 아니면 깨운다.
			}

			if (old_vlist.count(pl) == 0)
				clients[c_id].send_add_player_packet(pl);
		}

		for (auto& pl : old_vlist) {
			if (0 == near_list.count(pl)) {
				clients[c_id].send_remove_player_packet(pl);
				if (is_pc(pl))
					clients[pl].send_remove_player_packet(c_id);
			}
		}

		break;
	}
	case CS_ATTACK:
		CS_ATTACK_PACKET* p = reinterpret_cast<CS_ATTACK_PACKET*>(packet);
		Attack(c_id);
		break;
	}
}

void disconnect(int c_id)
{
	SaveDB(c_id);
	clients[c_id]._vl.lock();
	unordered_set <int> vl = clients[c_id]._view_list;
	clients[c_id]._vl.unlock();
	for (auto& p_id : vl) {
		if (is_npc(p_id)) continue;
		auto& pl = clients[p_id];
		{
			lock_guard<mutex> ll(pl._s_lock);
			if (ST_INGAME != pl._state) continue;
		}
		if (pl._id == c_id) continue;
		pl.send_remove_player_packet(c_id);
	}
	closesocket(clients[c_id]._socket);

	lock_guard<mutex> ll(clients[c_id]._s_lock);
	clients[c_id]._state = ST_FREE;
}

void Attack(int m_id)
{
	clients[m_id]._vl.lock();
	unordered_set <int> vl = clients[m_id]._view_list;
	clients[m_id]._vl.unlock();

	for (auto p_id : vl)
	{
		if (is_pc(p_id))continue;

		if ((clients[p_id].x == clients[m_id].x && clients[p_id].y == clients[m_id].y - 1) ||
			(clients[p_id].x == clients[m_id].x && clients[p_id].y == clients[m_id].y + 1) ||
			(clients[p_id].x == clients[m_id].x - 1 && clients[p_id].y == clients[m_id].y) ||
			(clients[p_id].x == clients[m_id].x + 1 && clients[p_id].y == clients[m_id].y))
		{
			clients[p_id]._vl.lock();
			clients[p_id].hp -= (clients[m_id].level * 3);
			clients[p_id]._vl.unlock();

			if (clients[p_id].hp <= 0)
			{
				clients[p_id]._state = ST_FREE;

				clients[p_id].sleep_time = chrono::system_clock::now();

				clients[m_id].exp += clients[p_id].exp;

				if (clients[m_id].exp >= (clients[m_id].level * 100))
				{
					clients[m_id].level++;
					clients[m_id].exp = 0;
					clients[m_id].maxexp *= 2;
					clients[m_id].maxhp *= 2;
					clients[m_id].maxmp *= 2;
					clients[m_id].hp = clients[m_id].maxhp;
					clients[m_id].mp = clients[m_id].maxmp;

					char mess[50];
					sprintf_s(mess, "Player LV %d -> %d UP", clients[m_id].level - 1, clients[m_id].level);
					clients[m_id].send_chat_packet(m_id, mess);
					for (int i = 0; i < MAX_USER; ++i)
					{
						if (i == m_id)
							continue;
						if (clients[i]._state == ST_INGAME)
						{
							unordered_set <int> vl = clients[m_id]._view_list;
							for (auto other_id : vl)
							{
								clients[i].send_chat_packet(m_id, mess);
								break;
							}

						}
					}
				}

				clients[m_id].send_statchange_packet(m_id);

				char mess[50];
				sprintf_s(mess, "%s Catch LV%d Monster + %d ", clients[m_id]._name, clients[p_id].level, clients[p_id].exp);
				clients[m_id].send_chat_packet(m_id, mess);
				for (int i = 0; i < MAX_USER; ++i)
				{
					if (i == m_id)
						continue;
					if (clients[i]._state == ST_INGAME)
					{
						unordered_set <int> vl = clients[m_id]._view_list;
						for (auto other_id : vl)
						{
							clients[i].send_chat_packet(m_id, mess);
							break;
						}

					}
				}

				clients[m_id].send_remove_player_packet(p_id);

				for (int i = 0; i < MAX_USER; ++i)
				{
					if (clients[i]._state == ST_INGAME)
					{
						unordered_set <int> vl = clients[i]._view_list;
						for (auto monster_id : vl)
						{
							if (monster_id == p_id) {
								clients[i].send_remove_player_packet(p_id);
								break;
							}
						}

					}

				}
			}
			else if (clients[p_id].hp > 0) {
				char mess[50];
				sprintf_s(mess, "%s Damaged to %d -> LV%d Mon", clients[m_id]._name, clients[m_id].level * 3, clients[p_id].level);
				clients[m_id].send_chat_packet(m_id, mess);
				for (int i = 0; i < MAX_USER; ++i)
				{
					if (i == m_id)
						continue;

					if (clients[i]._state == ST_INGAME)
					{
						unordered_set <int> vl = clients[m_id]._view_list;
						for (auto other_id : vl)
						{
							clients[i].send_chat_packet(m_id, mess);
							break;
						}

					}
				}
			}
		}
	}
}
void do_npc_random_move(int npc_id)
{
	SESSION& npc = clients[npc_id];
	unordered_set<int> old_vl;
	for (auto& obj : clients) {
		if (ST_INGAME != obj._state) continue;
		if (true == is_npc(obj._id)) continue;
		if (true == can_see(npc._id, obj._id))
			old_vl.insert(obj._id);
	}

	int x = npc.x;
	int y = npc.y;

	if (npc_id > MAX_USER && npc_id < MAX_NPC / 4)
	{

	}
	else if (npc_id >= MAX_NPC / 4 && npc_id < (MAX_NPC / 4 * 2))
	{
		switch (rand() % 4) {
		case 0: if (x < (W_WIDTH - 1) && wmap[y][x + 1] == MAP_TYPE::e_PLAT) x++; break;
		case 1: if (x > 0 && wmap[y][x - 1] == MAP_TYPE::e_PLAT) x--; break;
		case 2: if (y < (W_HEIGHT - 1) && wmap[y + 1][x] == MAP_TYPE::e_PLAT) y++; break;
		case 3:if (y > 0 && wmap[y - 1][x] == MAP_TYPE::e_PLAT) y--; break;
		}
	}
	else if (npc_id >= (MAX_NPC / 4 * 2) && npc_id < (MAX_NPC / 4 * 3))
	{

	}
	else
	{
		switch (rand() % 4) {
		case 0: if (x < (W_WIDTH - 1) && wmap[y][x + 1] == MAP_TYPE::e_PLAT) x++; break;
		case 1: if (x > 0 && wmap[y][x - 1] == MAP_TYPE::e_PLAT) x--; break;
		case 2: if (y < (W_HEIGHT - 1) && wmap[y + 1][x] == MAP_TYPE::e_PLAT) y++; break;
		case 3:if (y > 0 && wmap[y - 1][x] == MAP_TYPE::e_PLAT) y--; break;
		}
	}

	npc.x = x;
	npc.y = y;

	unordered_set<int> new_vl;
	for (auto& obj : clients) {
		if (ST_INGAME != obj._state) continue;
		if (true == is_npc(obj._id)) continue;
		if (true == can_see(npc._id, obj._id)) {
			new_vl.insert(obj._id);
		}
	}

	for (auto pl : new_vl) {
		if (0 == old_vl.count(pl)) {
			// 플레이어의 시야에 등장
			clients[pl].send_add_player_packet(npc._id);
		}
		else {
			// 플레이어가 계속 보고 있음.
			clients[pl].send_move_packet(npc._id);
			if (npc._id > MAX_USER)
			{
				if (clients[pl].x == clients[npc._id].x &&
					clients[pl].y == clients[npc._id].y)
				{
					clients[pl].hp -= clients[npc._id].level * 5;
					clients[pl].send_statchange_packet(pl);

					char mess[50];
					sprintf_s(mess, "%s %d Damage ", clients[pl]._name, clients[npc._id].level * 5);
					clients[pl].send_chat_packet(pl, mess);
					for (int i = 0; i < MAX_USER; ++i)
					{
						if (i == pl)
							continue;
						if (clients[i]._state == ST_INGAME)
						{
							unordered_set <int> vl = clients[pl]._view_list;
							for (auto other_id : vl)
							{
								clients[i].send_chat_packet(pl, mess);
								break;
							}
						}
					}

					if (clients[pl].hp <= 0)
					{
						clients[pl]._state = ST_FREE;
						TIMER_EVENT ev{ pl, chrono::system_clock::now(), OP_RESPAWN, 0 };
						timer_queue.push(ev);
					}
				}
			}
		}
	}
	///vvcxxccxvvdsvdvds
	for (auto pl : old_vl) {
		if (0 == new_vl.count(pl)) {
			clients[pl]._vl.lock();
			if (0 != clients[pl]._view_list.count(npc._id)) {
				clients[pl]._vl.unlock();
				clients[pl].send_remove_player_packet(npc._id);
			}
			else {
				clients[pl]._vl.unlock();
			}
		}
	}
}

void worker_thread(HANDLE h_iocp)
{
	while (true) {
		DWORD num_bytes;
		ULONG_PTR key;
		WSAOVERLAPPED* over = nullptr;
		BOOL ret = GetQueuedCompletionStatus(h_iocp, &num_bytes, &key, &over, INFINITE);
		OVER_EXP* ex_over = reinterpret_cast<OVER_EXP*>(over);
		if (FALSE == ret) {
			if (ex_over->_comp_type == OP_ACCEPT) cout << "Accept Error";
			else {
				cout << "GQCS Error on client[" << key << "]\n";
				disconnect(static_cast<int>(key));
				if (ex_over->_comp_type == OP_SEND) delete ex_over;
				continue;
			}
		}

		if ((0 == num_bytes) && ((ex_over->_comp_type == OP_RECV) || (ex_over->_comp_type == OP_SEND))) {
			disconnect(static_cast<int>(key));
			if (ex_over->_comp_type == OP_SEND) delete ex_over;
			continue;
		}

		switch (ex_over->_comp_type) {
		case OP_ACCEPT: {
			int client_id = get_new_client_id();
			if (client_id != -1) {
				{
					lock_guard<mutex> ll(clients[client_id]._s_lock);
					clients[client_id]._state = ST_ALLOC;
				}
				clients[client_id].x = 0;
				clients[client_id].y = 0;
				clients[client_id]._id = client_id;
				clients[client_id]._name[0] = 0;
				clients[client_id]._prev_remain = 0;
				clients[client_id]._socket = g_c_socket;
				CreateIoCompletionPort(reinterpret_cast<HANDLE>(g_c_socket),
					h_iocp, client_id, 0);
				clients[client_id].do_recv();
				g_c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			}
			else {
				cout << "Max user exceeded.\n";
			}
			ZeroMemory(&g_a_over._over, sizeof(g_a_over._over));
			int addr_size = sizeof(SOCKADDR_IN);
			AcceptEx(g_s_socket, g_c_socket, g_a_over._send_buf, 0, addr_size + 16, addr_size + 16, 0, &g_a_over._over);
			break;
		}
		case OP_RECV: {
			int remain_data = num_bytes + clients[key]._prev_remain;
			char* p = ex_over->_send_buf;
			while (remain_data > 0) {
				int packet_size = p[0];
				if (packet_size <= remain_data) {
					process_packet(static_cast<int>(key), p);
					p = p + packet_size;
					remain_data = remain_data - packet_size;
				}
				else break;
			}
			clients[key]._prev_remain = remain_data;
			if (remain_data > 0) {
				memcpy(ex_over->_send_buf, p, remain_data);
			}
			clients[key].do_recv();
			break;
		}
		case OP_SEND:
			delete ex_over;
			break;
		case OP_NPC_MOVE:
		{
			bool keep_alive = false;
			for (int j = 0; j < MAX_USER; ++j)
				if (can_see(static_cast<int>(key), j)) { // 주위에있다고 무조건 keep_alive는 아니죠????
					if (clients[j]._state != ST_INGAME) continue;
					keep_alive = true; // 나를 지켜보고 있는 플레이어가 있으면 keep_alive
					break;
				}
			if (true == keep_alive) {
				do_npc_random_move(static_cast<int>(key)); // 주위에 지켜보고있는애들이 있을경우에만 random_move
				TIMER_EVENT ev{ key, chrono::system_clock::now() + 1s, EV_RANDOM_MOVE, 0 }; // 1초후에 다시 이동
				timer_queue.push(ev);
			}
			else {
				clients[key]._is_active = false;
			}
			delete ex_over;
			break;
		}
		case OP_NPC_ATTACK: {


			break;
		}
		case OP_HEAL: {

			if (clients[key].maxhp != clients[key].hp)
				clients[key].hp += clients[key].maxhp / 10;

			if (clients[key].hp >= clients[key].maxhp)
				clients[key].hp = clients[key].maxhp;
			else if (clients[key].hp < clients[key].maxhp)
			{
				TIMER_EVENT ev{ key, chrono::system_clock::now() + 5s, OP_SELF_HEAL, 0 }; // 1초후에 다시 이동
				timer_queue.push(ev);
			}

			clients[key].send_statchange_packet(key);
			break;
		}
		case OP_PLAYER_RESPAWN: {
			clients[key].hp = clients[key].maxhp;
			clients[key].x = 1010;
			clients[key].y = 1010;
			clients[key].exp /= 2;
			clients[key]._state = ST_INGAME;

			clients[key].send_statchange_packet(key);
			char mess[50];
			sprintf_s(mess, "%s Player RESPAWN!!!!!", clients[key]._name);
			clients[key].send_chat_packet(key, mess);

			for (int i = 0; i < MAX_USER; ++i)
			{
				if (i == key)
					continue;
				if (clients[i]._state == ST_INGAME)
				{
					unordered_set <int> vl = clients[key]._view_list;
					for (auto other_id : vl)
					{
						clients[i].send_chat_packet(key, mess);
						break;
					}

				}
			}

			break;
		}
		}
	}
}

void InitializeNPC()
{
	cout << "NPC initialize begin.\n";
	for (int i = MAX_USER; i < MAX_USER + MAX_NPC; ++i) {

		while (true)
		{
			int x = rand() % 2000;
			int y = rand() % 2000;

			if (wmap[y][x] == MAP_TYPE::e_PLAT)
			{
				clients[i].x = x;
				clients[i].y = y;
				break;
			}
		}

		if (i > MAX_USER && i < MAX_NPC / 4)
		{
			clients[i].level = 1;
			clients[i].hp = 10;
			clients[i].exp = 2;
		}
		else if (i >= MAX_NPC / 4 && i < (MAX_NPC / 4 * 2))
		{
			clients[i].level = 2;
			clients[i].hp = 20;
			clients[i].exp = 8;
		}
		else if (i >= (MAX_NPC / 4 * 2) && i < (MAX_NPC / 4 * 3))
		{
			clients[i].level = 3;
			clients[i].hp = 30;
			clients[i].exp = 18;
		}
		else
		{
			clients[i].level = 4;
			clients[i].hp = 40;
			clients[i].exp = 32;
		}

		clients[i]._id = i;
		sprintf_s(clients[i]._name, "NPC%d", i);
		clients[i]._state = ST_INGAME;
	}

	cout << "NPC initialize set.\n";
}

void InitDataBase() {

	setlocale(LC_ALL, "korean");


	// Allocate environment handle  
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

	// Set the ODBC version environment attribute  
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);

		// Allocate connection handle  
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

			// Set login timeout to 5 seconds  
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);

				// Connect to data source  
				retcode = SQLConnect(hdbc, (SQLWCHAR*)L"2022FALLODBC", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);

				// Allocate statement handle  
				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
					retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

					printf("ODBC Connect OK \n");
				}
			}
		}
	}

}

bool CheckDataBase(CS_LOGIN_PACKET p, int c_id)
{
	string temp = p.name;
	string ttemp = "";
	for (int i = 0; i < 5; ++i)
	{
		ttemp += temp[i];
	}

	if (ttemp == "DUMMY")
	{
		strcpy_s(clients[c_id]._name, p.name);
		//clients[c_id].x = rand() % 2000;
		//clients[c_id].y = rand() % 2000;
		while (true)
		{
			clients[c_id].hp = 5000;
			if (c_id < 20)
			{
				clients[c_id].x = 1010;
				clients[c_id].y = 1010;
				break;
			}
			else
			{
				clients[c_id].x = rand() % 2000;
				clients[c_id].y = rand() % 2000;
			}

			if (wmap[clients[c_id].y][clients[c_id].x] == MAP_TYPE::e_PLAT)
			{
				break;
			}
		}


		return true;
	}

	wstring wtemp = L"";
	wtemp.assign(temp.begin(), temp.end());

	wstring proc = L"EXEC select_user_id ";
	proc += wtemp;

	setlocale(LC_ALL, "korean");

	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	retcode = SQLExecDirect(hstmt, (SQLWCHAR*)proc.c_str(), SQL_NTS);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLBindCol(hstmt, 1, SQL_C_WCHAR, szUser_ID, NAME_LEN, &cbLoginID);
		retcode = SQLBindCol(hstmt, 2, SQL_C_LONG, &pos_x, 100, &cb0);
		retcode = SQLBindCol(hstmt, 4, SQL_C_LONG, &user_lv, 100, &cb2);
		retcode = SQLBindCol(hstmt, 3, SQL_C_LONG, &pos_y, 100, &cb1);
		retcode = SQLBindCol(hstmt, 5, SQL_C_LONG, &user_hp, 100, &cb3);
		retcode = SQLBindCol(hstmt, 6, SQL_C_LONG, &user_maxhp, 100, &cb4);
		retcode = SQLBindCol(hstmt, 7, SQL_C_LONG, &user_mp, 100, &cb5);
		retcode = SQLBindCol(hstmt, 8, SQL_C_LONG, &user_maxmp, 100, &cb6);
		retcode = SQLBindCol(hstmt, 9, SQL_C_LONG, &user_exp, 100, &cb7);
		retcode = SQLBindCol(hstmt, 10, SQL_C_LONG, &user_maxexp, 100, &cb8);

		for (int i = 0; ; i++) {
			retcode = SQLFetch(hstmt);
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO) {
					//HandleDiagnosticRecord(hstmt, SQL_HANDLE_STMT, retcode);
				}
				//검색 완료( 존재 )
				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
				{
					printf("ID: %S  ", szUser_ID);
					printf("X: %d  ", pos_x);
					printf("Y: %d  ", pos_y);
					printf("lv: %d  ", user_lv);
					printf("hp: %d  ", user_hp);
					printf("maxhp: %d  ", user_maxhp);
					printf("mp: %d  ", user_mp);
					printf("maxmp: %d  ", user_maxmp);
					printf("exp: %d  ", user_exp);
					printf("maxexp: %d  ", user_maxexp);

					printf("로그인 성공.\n");

					clients[c_id].x = pos_x;
					clients[c_id].y = pos_y;
					clients[c_id].level = user_lv;
					clients[c_id].exp = user_exp;
					clients[c_id].hp = user_hp;
					clients[c_id].mp = user_mp;
					clients[c_id].maxexp = user_maxexp;
					clients[c_id].maxhp = user_maxhp;
					clients[c_id].maxmp = user_maxmp;

					strcpy_s(clients[c_id]._name, p.name);

					return true;


				}
				else {
					return false;
				}
			}
			else {
				return false;
			}
		}
	}
	else {
		return false;
	}
}

void SaveDB(int c_id)
{
	string temp(clients[c_id]._name);
	wstring wtemp;
	wtemp.assign(temp.begin(), temp.end());

	wstring proc = L"EXECUTE select_user_pos ";
	proc += wtemp;
	proc += L", ";
	proc += to_wstring(clients[c_id].y);
	proc += L", ";
	proc += to_wstring(clients[c_id].level);
	proc += L", ";
	proc += to_wstring(clients[c_id].hp);
	proc += L", ";
	proc += to_wstring(clients[c_id].maxhp);
	proc += L", ";
	proc += to_wstring(clients[c_id].mp);
	proc += L", ";
	proc += to_wstring(clients[c_id].maxmp);
	proc += L", ";
	proc += to_wstring(clients[c_id].exp);
	proc += L", ";
	proc += to_wstring(clients[c_id].maxexp);
	proc += L", ";
	proc += to_wstring(clients[c_id].x);

	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	retcode = SQLExecDirect(hstmt, (SQLWCHAR*)proc.c_str(), SQL_NTS);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		cout << "POS 저장 완료" << endl;
	}
	else {
		cout << "저장 실패" << endl;
	}
}

void do_timer()
{
	while (true) {
		TIMER_EVENT ev;
		auto current_time = chrono::system_clock::now();
		if (true == timer_queue.try_pop(ev)) { // 있나없나 본다. timer_queue에서 뽑아서 있으면 꺼낸거 
			if (ev.wakeup_time > current_time) { // 시간을 봐서 실행할시간이 되었는가?? 작으면 아직 실행할 시간이 아니기에
				timer_queue.push(ev);		// 최적화 필요 // 다시 집어넣고
				// timer_queue에 다시 넣지 않고 처리해야 한다.
				this_thread::sleep_for(1ms);  // 실행시간이 아직 안됐으므로 잠시 대기
				continue; // 그냥 continue하면 헛돌기때문에 바로 실행하기보단 살짝 기다렸다가 continue해준다.
			}
			switch (ev.event_id) {
			case EV_RANDOM_MOVE: {
				OVER_EXP* ov = new OVER_EXP;
				ov->_comp_type = OP_NPC_MOVE; // 누가? 아래에 ev.obj_id
				PostQueuedCompletionStatus(h_iocp, 1, ev.obj_id, &ov->_over); // 실행을하고 된거면 바로 다시 돌고
				break;
			}
			case OP_SELF_HEAL: {
				OVER_EXP* oov = new OVER_EXP;
				oov->_comp_type = OP_HEAL;
				PostQueuedCompletionStatus(h_iocp, 1, ev.obj_id, &oov->_over); // 실행을하고 된거면 바로 다시 돌고
				break;
			}
			case OP_RESPAWN: {
				OVER_EXP* oov = new OVER_EXP;
				oov->_comp_type = OP_PLAYER_RESPAWN;
				PostQueuedCompletionStatus(h_iocp, 1, ev.obj_id, &oov->_over); // 실행을하고 된거면 바로 다시 돌고
				break;
			}
			}
			continue;	// 즉시 다음 작업 꺼내기
		}
		else this_thread::sleep_for(1ms); // timer_queue가 비어 있으니 잠시 기다렸다가 다시 시작 계속 돌면 cpu낭비가 될 수 있으니 잠시 기다렸다가.
	}
}

int main()
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);

	InitDataBase();
	Init_mapdata();
	InitializeNPC();

	g_s_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT_NUM);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(g_s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	listen(g_s_socket, SOMAXCONN);
	SOCKADDR_IN cl_addr;
	int addr_size = sizeof(cl_addr);



	h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(g_s_socket), h_iocp, 9999, 0);
	g_c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	g_a_over._comp_type = OP_ACCEPT;
	AcceptEx(g_s_socket, g_c_socket, g_a_over._send_buf, 0, addr_size + 16, addr_size + 16, 0, &g_a_over._over);

	vector <thread> worker_threads;
	int num_threads = std::thread::hardware_concurrency();
	for (int i = 0; i < num_threads; ++i)
		worker_threads.emplace_back(worker_thread, h_iocp);
	thread timer_thread{ do_timer };
	timer_thread.join();
	for (auto& th : worker_threads)
		th.join();
	closesocket(g_s_socket);
	WSACleanup();
}
