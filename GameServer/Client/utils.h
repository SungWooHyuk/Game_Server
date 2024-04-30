/*------------------------
	Related Windows Size
--------------------------*/
#ifndef UTILS_H
#define UTILS_H

constexpr uint16 SCREEN_WIDTH = 16;
constexpr uint16 SCREEN_HEIGHT = 16;

constexpr uint16 TILE_WIDTH = 65;

constexpr uint16 WINDOW_WIDTH = SCREEN_WIDTH * TILE_WIDTH;   // size of window
constexpr uint16 WINDOW_HEIGHT = SCREEN_HEIGHT * TILE_WIDTH;

constexpr uint16 TOWN_SIZE = 20;
constexpr uint16 W_WIDTH = 2000;
constexpr uint16 W_HEIGHT = 2000;

uint16	g_left_x;
uint16	g_top_y;
uint16	g_myid;

enum SystemText
{
	HP,
	MAXHP,
	MP,
	MAXMP,
	EXP,
	MAXEXP,
	LEVEL,
	NAME,
	CHAT
};

enum SystemBox
{
	HPBOX,
	MPBOX,
	EXPBOX,
	HPINNERBOX,
	MPINNERBOX,
	EXPINNERBOX
};
#define	MAPDATA				MapData::GetInstance()
#define	SFSYSTEM			SFSystem::GetInstance()

#define	BOX					300, 30

#define WHITE				255, 255, 255
#define	RED					255, 0, 0
#define	BLUE				0, 0, 255
#define	MAGENTA				255, 0, 255

#define	OUTLINETHICK		3.f

#define INFORMATION_X		1050
#define	INFORMATION_HP_Y	400
#define	INFORMATION_MP_Y	500
#define	INFORMATION_EXP_Y	600


#endif 