#include "Server.h"

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

// Функция обратного вызова.
RPC_STATUS CALLBACK SecurityCallback(RPC_IF_HANDLE hInterface, void* pBindingHandle)
{
	return RPC_S_OK;
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

int logIn(unsigned char *username, unsigned userNlen, unsigned char *password, unsigned passlen)
{
	decrypt(username, userNlen);
	decrypt(password, passlen);

	handle_t handle;
	// LOGON32_LOGON_INTERACTIVE - Этот тип входа предназначен для пользователей, которые будут в интерактивном режиме использовать компьютер.
	// LOGON32_PROVIDER_DEFAULT - Стандартный поставщик входа в систему.
	if (LogonUser((const char *)username, NULL, (const char *)password, LOGON32_LOGON_INTERACTIVE, LOGON32_PROVIDER_DEFAULT, &handle) == 0) return 1;
	else if (ImpersonateLoggedOnUser(handle) == 0) return 2;
	else
	{
		printf("Пользователь '%s' успешно вошел в систему.\n", username);
		return 0;
	}
}

int logOut()
{
	if (RevertToSelf() != 0) // Завершает имперсонацию клиентского приложения.
	{
		printf("Пользователь успешно вышел из системы.\n");
		return 1;
	}
	else
	{
		printf("ERROR\n");
		return 0; // Ошибка.
	}
}

void setPKeyToServer(unsigned char *pbKeyBlob, unsigned long dwPublicBlobLen)
{
	// Сохраняем публичный ключ.
	memset(clientPKey, 0, sizeof(clientPKey));

	memcpy(clientPKey, pbKeyBlob, dwPublicBlobLen);
	clientPKeyLen = dwPublicBlobLen;

	printf("  открытый ключ от клиента, длины %d получен.\n", clientPKeyLen);
}

void recvEncrypSKeyFromServer(unsigned char *encyptedSKey, unsigned len, unsigned long *encyptedSKeyLen)
{
	BYTE *encryptedSessionKey;
	DWORD encryptedSessionKeyLen;

	// Генерация сеансового ключа.
	if (cryptServerGenSessionKey(&hSKey, clientPKey, clientPKeyLen, &encryptedSessionKey, &encryptedSessionKeyLen))
	{
		printf("Ошибка при генерации сеансового ключа");
		return;
	}
	else
		printf("  сеансовый ключ клиента cгенерирован\n");

	memcpy(encyptedSKey, encryptedSessionKey, encryptedSessionKeyLen);
	*encyptedSKeyLen = encryptedSessionKeyLen;
}

int main()
{
	setlocale(LC_ALL, "RUS");
	RPC_STATUS status;

	// Инициализация сервера для работы с шифрованием.
	cryptServerInit();

	unsigned char port[8];
	printf("Введите порт: ");
	scanStr(port);

	// Идентифицирует пункт назначения (endpoint) сервера и последовательность сетевых протоколов.
	status = RpcServerUseProtseqEp(
		(RPC_CSTR)("ncacn_ip_tcp"),       // Идентификатор последовательности протокола для регистрации.
		RPC_C_PROTSEQ_MAX_REQS_DEFAULT,   // Длина очереди невыполненных заданий для последовательности протокола ncacn_ip_tcp.
		(RPC_CSTR)(port),                 // Указатель на информацию об адресе конечной точки.
		NULL);

	if (status)
		exit(status);

	// Регистрирует интерфейс.
	status = RpcServerRegisterIf2(
		InterfaceRPC_v1_0_s_ifspec,           // MIDL-генерируемая структура, указывающая интерфейс для регистрации.
		NULL,                                 // Регистрирует IfSpec с нулевым UUID.
		NULL,                                 // Использовать сгенерированный MIDL EPV по умолчанию.
		RPC_IF_ALLOW_CALLBACKS_WITH_NO_AUTH,  // Принудительное использование безопасного обратного вызова.
		RPC_C_LISTEN_MAX_CALLS_DEFAULT,       // Максимальное количество одновременных запросов удаленных вызовов процедур.
		(unsigned)-1,                         // Бесконечный максимальный размер входящих блоков данных.
		SecurityCallback);                    // Функция обратного вызова.

	if (status)
		exit(status);

	if (status)
		exit(status);

	printf("RPC сервер запущен!\n");

	// Инструктирует сервер начать прослушивание запросов клиента.
	status = RpcServerListen(
		1,                              // Минимальное количество потоков вызовов, которое должно быть создано и поддерживаться на данном сервере.
		RPC_C_LISTEN_MAX_CALLS_DEFAULT, // Рекомендуемое максимальное количество одновременных удаленных вызовов процедур, которые может выполнить сервер. 
		FALSE);                         // Не должен возвращаться, пока функция RpcMgmtStopServerListening не была вызвана, и все удаленные вызовы не завершены.

	if (status) exit(status);

	return 0;
}