#pragma once

#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aclapi.h>

HCRYPTKEY hSessionKey;

int cryptClientInit(BYTE **pbKeyPublicBlob, DWORD *dwPublicBlobLen, HCRYPTKEY *hKeyPair);
int cryptClientImportSessionKey(HCRYPTKEY *hKeyPair, HCRYPTKEY *hSessionKey, BYTE *encyptedSessionKey, DWORD encyptedSessionKeyLen);

DWORD encrypt(BYTE *pbData, DWORD pdwDataLen, DWORD maxBufLen);
DWORD decrypt(BYTE *pbData, DWORD pdwDataLen);