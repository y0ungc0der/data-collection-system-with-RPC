#pragma once
#include "System.h"

BYTE clientPKey[512];
DWORD clientPKeyLen;
HCRYPTKEY hSKey; // Дескриптор сеансового ключа.

int cryptServerInit();
int cryptServerGenSessionKey(HCRYPTKEY *hSessionKey, BYTE *publicClientKey, DWORD publicClientKeyLen, BYTE **encryptedSessionKey, DWORD *encryptedSessionKeyLen);

DWORD encrypt(BYTE *pbData, DWORD pdwDataLen, DWORD maxBufLen);
DWORD decrypt(BYTE *pbData, DWORD pdwDataLen);
