#include "Crypt.h"

#define error(x) { printf("[ERROR] %s\n", x); return -1; }

HCRYPTPROV hProv; // Дескриптор криптопровайдера.
CHAR *containerName = "MegaSecureContainer"; // Название контейнера ключей.

/* Инициализация сервера для работы с шифрованием. */
int cryptServerInit()
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

	return 0;
}

/* Генерация сеансового ключа. */
int cryptServerGenSessionKey(HCRYPTKEY *hSessionKey, BYTE *publicClientKey, DWORD publicClientKeyLen, BYTE **encryptedSessionKey, DWORD *encryptedSessionKeyLen)
{

	// CALG_RC4	- Алгоритм потокового шифрования RC4
	if (!CryptGenKey(hProv, CALG_RC4, CRYPT_EXPORTABLE, hSessionKey))
		error("Ошибка во время CryptGenKey!");

	// Кладем полученый открытый ключ в криптоконтейнер, получаем его дескриптор.
	HCRYPTKEY hKeyPublic;
	CryptImportKey(hProv, publicClientKey, publicClientKeyLen, NULL, NULL, &hKeyPublic);

	// Шифруем сессионый ключ открытым ключом для отправки клиенту.
	{
		// Определяем длину шифрованного ключа.
		// SIMPLEBLOB -	Используется для транспортировки ключей сеанса.
		// hExpKey -Дескриптор открытого ключа. Данные ключа в экспортирующемся ключе BLOB зашифрованы с использованием этого ключа.
		if (!CryptExportKey(*hSessionKey, hKeyPublic, SIMPLEBLOB, NULL, NULL, encryptedSessionKeyLen))
			error("Ошибка BLOB!");

		// Выделяем память и получаем шифрованный сессионный ключ.
		*encryptedSessionKey = (BYTE *)malloc(*encryptedSessionKeyLen);
		if (*encryptedSessionKey == NULL) error("malloc error!");
		if (!CryptExportKey(*hSessionKey, hKeyPublic, SIMPLEBLOB, NULL, *encryptedSessionKey, encryptedSessionKeyLen))
			error("	Сеансоый ключ  не может быть записан в BLOB!");
	}

	return 0;
}

// Шифруем данные сессионным ключом.
DWORD encrypt(BYTE *pbData, DWORD pdwDataLen, DWORD maxBufLen)
{
	DWORD len = pdwDataLen;
	// Функция CryptEncrypt шифрует данные.\
	   Алгоритм, используемый для шифрования данных, обозначается ключом, хранящимся в модуле CSP, и на него ссылается параметр hKey .
	if (!CryptEncrypt(hSKey, NULL, TRUE, NULL, pbData, &len, maxBufLen))
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
	if (!CryptDecrypt(hSKey, NULL, TRUE, NULL, pbData, &len))
	{
		printf("  ошибка при расшифровке данных\n");
		printf("  %s\n", pbData);
		return 0;
	}

	pbData[len] = '\0';
	return len;
}