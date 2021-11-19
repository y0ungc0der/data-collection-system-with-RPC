#include "Crypt.h"

#define error(x) { printf("[ERROR] %s\n", x); return -1; }

HCRYPTPROV hProv; // Дескриптор криптопровайдера.
CHAR *containerName = "MegaSecureContainer"; // Название контейнера ключей.

/* Инициализация клиента для работы с шифрованием, генерация публичного ключа... */
int cryptClientInit(BYTE **pbKeyPublicBlob, DWORD *dwPublicBlobLen, HCRYPTKEY *hKeyPair)
{
	// Создание криптоконтейнера.
	// Имя используемого CSP: MS_DEF_PROV - Microsoft Base Cryptographic Provider
	// Тип провайдера PROV_RSA_FULL: обмен ключами - RSA; шифрование RC2/RC4
	// CRYPT_NEWKEYSET - создает новый контейнер ключей с именем, указанным pszContainer.Если pszContainer имеет значение NULL, создается контейнер ключей с именем по умолчанию.
	if (!CryptAcquireContext(&hProv, containerName, MS_DEF_PROV, PROV_RSA_FULL, CRYPT_NEWKEYSET))
	{
		// Тут ошибка, удаляем и пытаемся снова.
		// CRYPT_DELETEKEYSET - Удалить контейнер ключей
		CryptAcquireContext(&hProv, containerName, MS_DEF_PROV, PROV_RSA_FULL, CRYPT_DELETEKEYSET);
		if (!CryptAcquireContext(&hProv, containerName, MS_DEF_PROV, PROV_RSA_FULL, CRYPT_NEWKEYSET))
			error("Ошибка создания контейнера ключей!");
	}

	// Генерация пары ассиметричных ключей. AT_KEYEXCHANGE - для обмена ключами. CRYPT_EXPORTABLE - ключ можно будет экспортировать. 
	if (!CryptGenKey(hProv, AT_KEYEXCHANGE, CRYPT_EXPORTABLE, hKeyPair))
		error("Ошибка во время CryptGenKey!");

	// Определение длины открытого ключа. PUBLICKEYBLOB - работаем с открытым ключом.
	// Чтобы получить требуемый размер буфера pbKeyPublicBlob, передайте NULL для pbKeyPublicBlob.\
	   Требуемый размер буфера будет помещен в значение, указанное параметром dwPublicBlobLen.
	// Приложения должны использовать фактический размер возвращаемых данных.\
	На входе размеры буфера обычно задаются достаточно большими.\
	На выходе переменная, на которую указывает этот параметр, обновляется, чтобы отразить фактический размер данных, скопированных в буфер.
	if (!CryptExportKey(*hKeyPair, 0, PUBLICKEYBLOB, NULL, NULL, dwPublicBlobLen))
		error("Ошибка BLOB!");

	// Выделяет память для pbKeyPublicBlob
	*pbKeyPublicBlob = (BYTE *)malloc(*dwPublicBlobLen);
	if (*pbKeyPublicBlob == NULL) error("malloc error!");

	// Фактический экспорт публичного ключа в буфер защищенным образом.
	if (!CryptExportKey(*hKeyPair, 0, PUBLICKEYBLOB, NULL, *pbKeyPublicBlob, dwPublicBlobLen))
		error("Открытый ключ не может быть записан в BLOB.");

	return 0;
}

/* Расшифровывает полученный зашифрованный сессионный ключ приватным ключом и присваивает ему дескриптор #hSessionKey. */
int cryptClientImportSessionKey(HCRYPTKEY *hKeyPair, HCRYPTKEY *hSessionKey, BYTE *encyptedSessionKey, DWORD encyptedSessionKeyLen)
{
	CryptImportKey(hProv, encyptedSessionKey, encyptedSessionKeyLen, *hKeyPair, NULL, hSessionKey);
}

DWORD encrypt(BYTE *pbData, DWORD pdwDataLen, DWORD maxBufLen)
{
	DWORD len = pdwDataLen;

	// Функция CryptEncrypt шифрует данные.\
	   Алгоритм, используемый для шифрования данных, обозначается ключом, хранящимся в модуле CSP, и на него ссылается параметр hKey .
	if (!CryptEncrypt(hSessionKey, NULL, TRUE, NULL, pbData, &len, maxBufLen))
	{
		printf("  ошибка при шифровании данных\n");
		return 0;
	}

	return len;
}

// Расшифрование данных.
DWORD decrypt(BYTE *pbData, DWORD pdwDataLen)
{
	DWORD len = pdwDataLen;

	// Функция CryptDecrypt расшифровывает данные, ранее зашифрованные с помощью функции CryptEncrypt.
	if (!CryptDecrypt(hSessionKey, NULL, TRUE, NULL, pbData, &len))
	{
		printf("  ошибка при расшифровке данных\n");
		return 0;
	}

	pbData[len] = '\0';
	return len;
}