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

#define	MAPDATA		MapData::GetInstance()
#endif // UTILS_H