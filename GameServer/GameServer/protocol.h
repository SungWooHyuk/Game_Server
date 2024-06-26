constexpr int PORT_NUM = 4000;
constexpr int BUF_SIZE = 200;
constexpr int NAME_SIZE = 20;
constexpr int CHAT_SIZE = 100;

constexpr int MAX_USER = 10000;
constexpr int MAX_NPC = 100000;

constexpr uint16 SCREEN_WIDTH = 16;
constexpr uint16 SCREEN_HEIGHT = 16;

constexpr uint16 TILE_WIDTH = 65;

constexpr uint16 WINDOW_WIDTH = SCREEN_WIDTH * TILE_WIDTH;   // size of window
constexpr uint16 WINDOW_HEIGHT = SCREEN_HEIGHT * TILE_WIDTH;

constexpr uint16 TOWN_SIZE = 20;
constexpr uint16 W_WIDTH = 2000;
constexpr uint16 W_HEIGHT = 2000;

// Packet ID
constexpr char CS_LOGIN = 0;
constexpr char CS_MOVE = 1;
constexpr char CS_CHAT = 2;				// 주위만
constexpr char CS_ATTACK = 3;			// 4 방향 공격
constexpr char CS_TELEPORT = 4;			// RANDOM한 위치로 Teleport, Stress Test할 때 Hot Spot현상을 피하기 위해 구현 
// 스트레스 테스트 할 때 랜덤한위치로 날리는 텔레포트
constexpr char CS_LOGOUT = 5;			// 클라이언트에서 정상적으로 접속을 종료하는 패킷

constexpr char SC_LOGIN_INFO = 2;
constexpr char SC_ADD_OBJECT = 3;
constexpr char SC_REMOVE_OBJECT = 4;
constexpr char SC_MOVE_OBJECT = 5;
constexpr char SC_CHAT = 6;
constexpr char SC_LOGIN_OK = 7; // 성공
constexpr char SC_LOGIN_FAIL = 8; // 실패
constexpr char SC_STAT_CHANGE = 9; // 레벨이 오르거나 HP가 깎일때를위해서

//enum MAP_TYPE
//{
//	e_PLAT,
//	e_OBSTACLE,
//	e_BTOWN,
//	e_GTOWN
//};
//
//struct MAP
//{
//	MAP_TYPE type; // 평지랑 장애물 구분하기 위함.
//};

#pragma pack (push, 1)
struct CS_LOGIN_PACKET {
	unsigned char size;
	char	type;
	char	name[NAME_SIZE];
};

struct CS_MOVE_PACKET {
	unsigned char size;
	char	type;
	char	direction;  // 0 : UP, 1 : DOWN, 2 : LEFT, 3 : RIGHT
	unsigned	move_time;
};

struct CS_CHAT_PACKET {
	unsigned char size;
	char	type;
	char	mess[CHAT_SIZE];
};

struct CS_TELEPORT_PACKET {
	unsigned char size;
	char	type;
};

struct CS_ATTACK_PACKET {
	unsigned char size;
	char	type;
};

struct CS_LOGOUT_PACKET {
	unsigned char size;
	char	type;
};

struct SC_LOGIN_INFO_PACKET {
	unsigned char size;
	char	type;
	int		id;
	int		hp;
	int		max_hp;
	int		exp;
	int		level;
	short	x, y;
};

struct SC_ADD_OBJECT_PACKET {
	unsigned char size;
	char	type;
	int		id;
	short	x, y;
	char	name[NAME_SIZE];
};

struct SC_REMOVE_OBJECT_PACKET {
	unsigned char size;
	char	type;
	int		id;
};

struct SC_MOVE_OBJECT_PACKET {
	unsigned char size;
	char	type;
	int		id;
	short	x, y;
	unsigned int move_time;
};

struct SC_CHAT_PACKET {
	unsigned char size;
	char	type;
	int		id;
	char	mess[CHAT_SIZE];
};

struct SC_LOGIN_OK_PACKET {
	unsigned char size;
	char	type;
	int		id;
	int		hp;
	int		max_hp;
	int		exp;
	int		max_exp;
	int		mp;
	int		max_mp;
	int		level;
	short	x, y;
	char	name[20];
};

struct SC_LOGIN_FAIL_PACKET {
	unsigned char size;
	char	type;

};

struct SC_STAT_CHANGEL_PACKET {
	unsigned char size;
	char	type;
	int		hp;
	int		max_hp;
	int		exp;
	int		max_exp;
	int		mp;
	int		max_mp;
	int		level;
	int		id;
};

#pragma pack (pop)