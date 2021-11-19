#include "Client.h"

void rpcFunc(char *, char *);

unsigned char gBuf[1048576];
int logflag = 0;

// Функция распределения памяти для RPC.
void __RPC_FAR * __RPC_API midl_user_allocate(size_t len)
{
	return(malloc(len));
}

// Функция освобождения памяти для RPC.
void __RPC_API midl_user_free(void __RPC_FAR * ptr)
{
	free(ptr);
}

char *scanStr(char *line)
{
	int ch;
	line[0] = '\0';

	for (int index = 0; ((ch = getchar()) != '\n') && (ch != EOF); index++)
	{
		line[index] = (char)ch;
		line[index + 1] = '\0';
	}

	return line;
}

void _logIn()
{
	unsigned char userName[256], password[256];
	int res;

	while (1)
	{
		memset(userName, 0, sizeof(userName));
		memset(password, 0, sizeof(password));

		printf("Введите имя пользователя: ");
		scanStr(userName);

		printf("Введите пароль: ");
		scanStr(password);

		int userNlen = encrypt(userName, strlen(userName), sizeof(userName));
		int passlen = encrypt(password, strlen(password), sizeof(password));

		res = logIn(userName, userNlen, password, passlen);
		if (res == 0) break;
		else if (res == 1) printf("Неверный логин или пароль!\n");
		else if (res == 2) printf("Неизвестная ошибка!\n");
	}
}

int _logOut()
{
	if (logOut() == 1)
	{
		printf("Успешный выход!\n");
		logflag = 0;
		return 1;
	}

	else printf("Ошибка при выходе!\n");
	return 0;
}

char * menu()
{
	static char result[256];
	char tmp[256];
	while (1)
	{
		memset(result, 0, sizeof(result));

		printf("Доступные команды:\n");
		printf(" 0 - OS info\n");
		printf(" 1 - current time\n");
		printf(" 2 - time since OS launch\n");
		printf(" 3 - memory info\n");
		printf(" 4 - disks info\n");
		printf(" 5 - free disk space\n");
		printf(" 6 - access rigths\n");
		printf(" 7 - get object's owner\n");
		printf(" ! - log out\n");
		printf("Выбор: ");
		int choice = scanStr(result)[0];
		if (choice != '!' && (choice < '0' || choice > '7'))
		{
			printf("Неизвестная команда :(\n");
			system("pause & cls");
			continue;
		}

		if (choice == '6' || choice == '7')
		{
			printf(" Введите путь к файлу или ключу реестра: ");
			scanStr(result + 2);

			printf(" Введите тип объекта (f или r): ");
			choice = scanStr(tmp)[0];
			if (choice != 'f' && choice != 'r')
			{
				printf("Ошибка!\n");
				system("pause & cls");
				continue;
			}
			*(result + 1) = choice;
		}

		return result;
	}
}

void operations()
{
	char *type;
	while (1)
	{
		system("cls");
		type = menu();

		printf("\n");

		memset(gBuf, sizeof(gBuf), 0);
		int ret;
		switch (type[0])
		{
		case '0':
			ret = getOsVersionName(gBuf, sizeof(gBuf));
			decrypt(gBuf, ret);
			printf(gBuf);
			break;
		case '1':
			ret = getCurrentTime(gBuf, sizeof(gBuf));
			decrypt(gBuf, ret);
			printf(gBuf);
			break;
		case '2':
			ret = getTimeSinceStart(gBuf, sizeof(gBuf));
			decrypt(gBuf, ret);
			printf(gBuf);
			break;
		case '3':
			ret = getMemoryInfo(gBuf, sizeof(gBuf));
			decrypt(gBuf, ret);
			printf(gBuf);
			break;
		case '4':
			ret = getDrivesList(gBuf, sizeof(gBuf));
			decrypt(gBuf, ret);
			printf(gBuf);
			break;
		case '5':
			ret = getLogicalDrivesMemoryInfo(gBuf, sizeof(gBuf));
			decrypt(gBuf, ret);
			printf(gBuf);
			break;
		case '6':
			ret = getAccessRights(type + 2, *(type + 1), gBuf, sizeof(gBuf));
			decrypt(gBuf, ret);
			printf(gBuf);
			break;
		case '7':
			ret = getOwner(type + 2, *(type + 1), gBuf, sizeof(gBuf));
			decrypt(gBuf, ret);
			printf(gBuf);
			break;
		case '!':
			_logOut();
			return;
		default:
			printf("Ошибка!\n");
		}

		printf("\n");
		system("pause");
	}

}

/* Организуем шифрование соединения сеансовым ключем. */
void initEncryptConnect()
{
	// Обеспечиваем шифрование.
	HCRYPTKEY hKeyPair;
	BYTE *pbKeyPublicBlob;
	DWORD dwPublicBlobLen;
	BYTE encyptedSessionKey[1024];
	DWORD encyptedSessionKeyLen;
	{
		// Генерируем пару ключей публичный/приватный, отправляем публичный ключ на сервер.
		if (cryptClientInit(&pbKeyPublicBlob, &dwPublicBlobLen, &hKeyPair))
		{
			printf("Ошибка при инициализации пары ключей клиента!");
			return;
		}
		else
			printf("Пара ключей создана.\n");
		setPKeyToServer(pbKeyPublicBlob, dwPublicBlobLen);

		free(pbKeyPublicBlob);
		pbKeyPublicBlob = NULL;

		// Принимаем шифрованный сессионный ключ, расшифровываем его с помощью приватного ключа.
		memset(encyptedSessionKey, 0, sizeof(encyptedSessionKey));
		recvEncrypSKeyFromServer(encyptedSessionKey, sizeof(encyptedSessionKey), &encyptedSessionKeyLen);

		cryptClientImportSessionKey(&hKeyPair, &hSessionKey, encyptedSessionKey, encyptedSessionKeyLen);
		memset(encyptedSessionKey, 0, sizeof(encyptedSessionKey));

		printf("Сессионный ключ успешно получен.\n");
	}

	printf("Зашифрованное соединение установлено!\n");
	system("pause");
	system("cls");
}


int main()
{
	setlocale(LC_ALL, "RUS");
	char host[32], portStr[8];

	printf("Введите IP: ");
	scanStr(host);
	printf("Введите порт: ");
	scanStr(portStr);

	printf("\nЗапуск клиента...\n");
	rpcFunc(host, portStr);

	return 0;
}

void rpcFunc(char *ip, char *port)
{
	RPC_STATUS status;
	unsigned char *szStringBinding = NULL;

	// Функция создает дескриптор привязки строки.
	status = RpcStringBindingCompose(
		(RPC_CSTR)"4b4d847a-d14c-4e87-93cf-b47af3a83474", // UUID для привязки.
		(unsigned char *) "ncacn_ip_tcp", // Строковое представление последовательности протокола.
		(unsigned char *)ip,              // Строковое представление сетевого адреса.
		(unsigned char *)port,            // Строковое представление конечной точки.
		NULL,                             // Строковое представление параметров сети (связана с последовательностью протокола). 
		&szStringBinding);

	if (status)
		exit(status);

	// Возвращает дескриптор привязки из строкового представления дескриптора привязки.
	status = RpcBindingFromStringBinding(
		szStringBinding, // Указатель на строковое представление дескриптора привязки.
		&hBinding);      // Поместит результат в дескриптор связывания, определенный в файле IDL.

	if (status)
		exit(status);

	RpcTryExcept
	{
		// Вызывает функцию RPC. Дескриптор связывания hBinding используется неявно.
		while (1)
		{
			// Сообщает, прослушивает ли сервер удаленные вызовы процедур.
			status = RpcMgmtIsServerListening(hBinding);
			if (status)
				if (status == RPC_S_NOT_LISTENING)
				{
					printf("RPC сервер не отвечает\n");
					main();
				}
			else
			{
				if (logflag == 1) operations();
				else
				{
					initEncryptConnect();
					_logIn();
					logflag = 1;
				}
			}
		}

	} RpcExcept(1)
	{
		printf("Превышено время ожидания/количество исключений %u\n", RpcExceptionCode());
		system("pause");
	}
	RpcEndExcept

		// Функция освобождает строку символов.
		status = RpcStringFree(&szStringBinding);
	if (status) exit(status);

	// Функция освобождает ресурсы дескриптора привязки и отключается от сервера.
	status = RpcBindingFree(&hBinding);
	if (status) exit(status);
}