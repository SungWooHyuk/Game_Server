#pragma once
#define NOMINMAX

#include "Types.h"
#include "CoreMacro.h"
#include "CoreTLS.h"
#include "CoreGlobal.h"
#include "Container.h"

#include <unordered_map>
#include <chrono>
#include <string>
#include <random>
#include <vector>
#include <fstream>
#include <windows.h>
#include <iostream>

using namespace std;

#include <winsock2.h>
#include <windows.h>
#include <mswsock.h>
#include <ws2tcpip.h>

//#pragma comment(lib, "ws2_32.lib")

#include "Lock.h"
#include "ObjectPool.h"
#include "TypeCast.h"
#include "Memory.h"
#include "SendBuffer.h"
#include "Session.h"
#include "LockQueue.h"
#include "ConsoleLog.h"
