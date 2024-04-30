#pragma once

#include "utils.h"

struct Stat
{
	int16	hp;
	int16	mp;
	int16	exp;
	int16	maxHp;
	int16	maxMp;
	int16	maxExp;
};

class Player
{
private:

public:

private:
	int16	myId;
	Stat	myStat;
};

