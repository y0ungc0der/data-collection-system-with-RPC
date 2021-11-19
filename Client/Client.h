#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <wincrypt.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <locale.h>
#include "time.h"
#include "Crypt.h"
#include "../Interface/Interface_h.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Rpcrt4.lib")

#pragma warning(disable : 4996) // Отключаем предупреждения и ошибки об устаревших и небезопасных функциях.