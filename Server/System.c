#include "Crypt.h"

// Получаем тип и версию ОС
int getOsVersionName(unsigned char *returnB, int size)
{
	// Заполняем структуру нулевыми байтами
	OSVERSIONINFOEX osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	// WinAPI требует указывать размер структуры
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	GetVersionEx((LPOSVERSIONINFOA)&osvi);

	DWORD major = osvi.dwMajorVersion; // идентификатор операционной системы
	DWORD minor = osvi.dwMinorVersion; // идентификатор версии
	DWORD version = major * 100 + minor;

	const char
		*w_8_1 = "Windows 8.1",
		*w_8 = "Windows 8",
		*w_7 = "Windows 7",
		*w_V = "Windows Vista",
		*w_2003 = "Windows 2003",
		*w_XP = "Windows XP",
		*w_2000 = "Windows 2000",
		*w_Me = "Windows Me",
		*w_98 = "Windows 98",
		*w_95 = "Windows 95",
		*_unkn = "unknown";

	unsigned char buf[512];
	memset(buf, 0, sizeof(buf));

	if (version == 603)
	{
		memcpy(buf, w_8_1, strlen(w_8_1));
	}
	else if (version == 602)
	{
		memcpy(buf, w_8, strlen(w_8));
	}
	else if (version == 601)
	{
		memcpy(buf, w_7, strlen(w_7));
	}
	else if (version == 600)
	{
		memcpy(buf, w_V, strlen(w_V));
	}
	else if (version == 502)
	{
		memcpy(buf, w_2003, strlen(w_2003));
	}
	else if (version == 501)
	{
		memcpy(buf, w_XP, strlen(w_XP));
	}
	else if (version == 500)
	{
		memcpy(buf, w_2000, strlen(w_2000));
	}
	else if (version == 490)
	{
		memcpy(buf, w_Me, strlen(w_Me));
	}
	else if (version == 410)
	{
		memcpy(buf, w_98, strlen(w_98));
	}
	else if (version == 400)
	{
		memcpy(buf, w_95, strlen(w_95));
	}
	else
	{
		memcpy(buf, _unkn, strlen(_unkn));
	}

	int len = encrypt(buf, strlen(buf), sizeof(buf));
	memcpy(returnB, buf, len);
	return len;
}

// Получаем текущее время
int getCurrentTime(unsigned char *returnB, int size)
{
	unsigned char buf[50];
	memset(buf, 0, sizeof(buf));

	SYSTEMTIME smu, sm;
	GetSystemTime(&smu); // формат UTC
	//Преобразуем время в формате UTC во времея соответствующее заданному часовому поясу
	SystemTimeToTzSpecificLocalTime(NULL, &smu, &sm); //NULL - функция использует текущий активный часовой пояс

	sprintf(buf, "%02d.%02d.%04d %02d:%02d:%02d\0", sm.wDay, sm.wMonth, sm.wYear, sm.wHour, sm.wMinute, sm.wSecond);
	
	int lenT = encrypt(buf, strlen(buf), sizeof(buf));
	memcpy(returnB, buf, lenT);
	return lenT;
}

// Получаем время, прошедшее с момента запуска ОС
int getTimeSinceStart(unsigned char *returnB, int size)
{
	unsigned char buf[20];
	memset(buf, 0, sizeof(buf));

	int hour, min, sec, msec = GetTickCount();
	hour = msec / (1000 * 60 * 60);
	min = msec / (1000 * 60) - hour * 60;
	sec = (msec / 1000) - (hour * 60 * 60) - min * 60;

	sprintf(buf, "%02d:%02d:%02d\0", hour, min, sec);

	int len = encrypt(buf, strlen(buf), sizeof(buf));
	memcpy(returnB, buf, len);
	return len;
}

// Получаем информацию об используемой памяти
int getMemoryInfo(unsigned char *returnB, int size)
{
	unsigned char buf[800];
	memset(buf, 0, sizeof(buf));
	/*
	 static MEMORYSTATUS stat;
	stat.dwLength = sizeof(stat);
	GlobalMemoryStatus(&stat);

	sprintf(buf, "Memory load: %u %%\nTotal physical memory: %u mb\nAvailable physical memory: %u mb\nTotal memory for programs: %u mb\nAvailable memory for programs: %u mb\nTotal virual memory: %u mb\nAvailable virtual memory: %u mb\0",
		stat.dwMemoryLoad, stat.dwTotalPhys / 0x100000, stat.dwAvailPhys / 0x100000, stat.dwTotalPageFile / 0x100000, stat.dwAvailPageFile / 0x100000, stat.dwTotalVirtual / 0x100000, stat.dwAvailVirtual / 0x100000);
	*/
	static MEMORYSTATUSEX stat;
	stat.dwLength = sizeof(stat);
	GlobalMemoryStatusEx(&stat);

	sprintf(buf, "Memory load: %u %%\nTotal physical memory: %lld mb\nAvailable physical memory: %lld mb\nTotal memory for programs: %lld mb\nAvailable memory for programs: %lld mb\nTotal virual memory: %lld mb\nAvailable virtual memory: %lld mb\0",
		stat.dwMemoryLoad, stat.ullTotalPhys / 0x100000, stat.ullAvailPhys / 0x100000, stat.ullTotalPageFile / 0x100000, stat.ullAvailPageFile / 0x100000, stat.ullTotalVirtual / 0x100000, stat.ullAvailVirtual / 0x100000);
	
	int len = encrypt(buf, strlen(buf), sizeof(buf));
	memcpy(returnB, buf, len);
	return len;
}

// Получаем информацию о дисках
int getDrivesList(unsigned char *returnB, int size)
{
	unsigned char buf[250];
	memset(buf, 0, sizeof(buf));

	// Заполняем буфер списком существующих дисков в системе вида А:/\0B:/\0C:/\0\0
	DWORD a = GetLogicalDriveStringsA(sizeof(buf), buf);
	//for (DWORD i = 0; i < a; i++)
	//printf("%c", buf[i]);

	unsigned char result[4096];
	memset(result, 0, sizeof(result));
	char *ptrResult = result;

	for (char *s = buf; *s != '\0'; s += 4)
	{
		switch (s[2] = GetDriveTypeA(s))
		{
		case 0:
			sprintf(ptrResult, "%c:%c - %s\n", s[0], s[2] + 0x30, "unknown type");
			break;
		case 1:
			sprintf(ptrResult, "%c:%c - %s\n", s[0], s[2] + 0x30, "root path is invalid");
			break;
		case 2:
			sprintf(ptrResult, "%c:%c - %s\n", s[0], s[2] + 0x30, "removable media");
			break;
		case 3:
			sprintf(ptrResult, "%c:%c - %s\n", s[0], s[2] + 0x30, "fixed media");
			break;
		case 4:
			sprintf(ptrResult, "%c:%c - %s\n", s[0], s[2] + 0x30, "remote drive");
			break;
		case 5:
			sprintf(ptrResult, "%c:%c - %s\n", s[0], s[2] + 0x30, "CD-ROM drive");
			break;
		case 6:
			sprintf(ptrResult, "%c:%c - %s\n", s[0], s[2] + 0x30, "RAM disk");
			break;
		}

		ptrResult += strlen(ptrResult);
		s[3] = ';';
	}

	int len = encrypt(result, strlen(result), sizeof(result));
	memcpy(returnB, result, len);
	return len;
}

// Получаем информацию о свободном месте на локальных дисках
int getLogicalDrivesMemoryInfo(unsigned char *returnB, int size)
{
	unsigned char buf[250];
	memset(buf, 0, sizeof(buf));

	// Заполняем буфер списком существующих дисков в системе вида А:/\0B:/\0C:/\0\0
	DWORD a = GetLogicalDriveStringsA(sizeof(buf), buf);

	unsigned char result[4096];
	memset(result, 0, sizeof(result));
	char *ptrResult = result;

	for (char *s = buf; *s != '\0'; s += 4)
	{
		if (GetDriveTypeA(s) == 3)
		{
			DWORD s1, b, f, c;
			GetDiskFreeSpaceA(s, &s1, &b, &f, &c);
			// s1 - количество секторов на кластер
			// b - количество байтов на сектор
			// f - общее количество свободных кластеров на диске, доступных пользователю
			// c - общее количество кластеров на диске, доступных пользователю
			double freeSpace = (double)f * (double)s1 * (double)b / 1024.0 / 1024.0 / 1024.0;
			sprintf(ptrResult, "%c: %.1f Gb left\n", s[0], freeSpace);
			ptrResult += strlen(ptrResult);
		}
	}

	int len = encrypt(result, strlen(result), sizeof(result));
	memcpy(returnB, result, len);
	return len;
}

// Получаем информацию о владельце указанного объекта
int getOwner(char *path, char setype, unsigned char *returnB, int size) // path - путь к объекту, SE_OBJECT_TYPE - тип объекта (файл\ключ реестра)
{
	SE_OBJECT_TYPE se;
	se = (setype == 'r') ? SE_REGISTRY_KEY : SE_FILE_OBJECT;

	// Извлекаем копию дескриптора безопасности для объекта с указанным именем
	PACL pDACL;
	PSECURITY_DESCRIPTOR pSD;
	PSID psidOwner;
	//								   получить список DACL		   получить владельца объекта  для записи SID    для записи DACL	
	if (GetNamedSecurityInfo(path, se, DACL_SECURITY_INFORMATION | OWNER_SECURITY_INFORMATION, &psidOwner, NULL, &pDACL, NULL, &pSD) != ERROR_SUCCESS)
	{
		printf("  Get security information error!\n");
		return 0;
	}

#define length 24
	DWORD len = length;
	CHAR user[length], domain[length];
	memset(user, 0, length);
	memset(domain, 0, length);
	SID_NAME_USE sid_nu; //тип учетной записи

	unsigned char buf[250];
	memset(buf, 0, sizeof(buf));

	char *resPtr = buf;
	char *ownerSID;
	// Преобразуем SID владельца в строковой вид
	if (!ConvertSidToStringSidA(psidOwner, &ownerSID))
	{
		printf(" Can't to determine owner's SID");
		return 0;
	}

	// Ищем имя домена или пользователя по их SID
	if (LookupAccountSid(NULL, psidOwner, user, &len, domain, &len, &sid_nu))
	{
		sprintf(resPtr, "Owner: %s\\%s (SID: %s)\n", domain, user, ownerSID);
		resPtr += strlen(resPtr);
	}

	int lenB = encrypt(buf, strlen(buf), sizeof(buf));
	memcpy(returnB, buf, lenB);
	return lenB;
}

/*
	Путь к ключу реестра задается, например, так: CURRENT_USER\Control Panel\Colors
	Indicates a registry key. A registry key object can be in the local registry, such as CLASSES_ROOT\SomePath or in a remote registry, such as \\ComputerName\CLASSES_ROOT\SomePath.
	The names of registry keys must use the following literal strings to identify the predefined registry keys: "CLASSES_ROOT", "CURRENT_USER", "MACHINE", and "USERS".
*/

// Получаем информацию о правах доступа для указанного объекта
int getAccessRights(char *path, char setype, unsigned char *returnB, int sizeR) // path - путь к объекту, SE_OBJECT_TYPE - тип объекта (файл\ключ реестра)
{
	SE_OBJECT_TYPE se;
	se = (setype == 'r') ? SE_REGISTRY_KEY : SE_FILE_OBJECT;

	// Извлекаем копию дескриптора безопасности для объекта с указанным именем
	PACL pDACL;
	PSECURITY_DESCRIPTOR pSD;
	//								   получить список DACL		   получить владельца объекта			   для записи DACL	
	if (GetNamedSecurityInfo(path, se, DACL_SECURITY_INFORMATION | OWNER_SECURITY_INFORMATION, NULL, NULL, &pDACL, NULL, &pSD) != ERROR_SUCCESS)
	{
		printf("  Get security information error!\n");
		return 0;
	}

	// Извлекаем информацию о списке контроля доступа (ACL)
	ACL_SIZE_INFORMATION aclInfo; // буфер для получения запрошенной информации
	//														 функция заполнит буфер, структурой ACL_SIZE_INFORMATION
	if (!GetAclInformation(pDACL, &aclInfo, sizeof(aclInfo), AclSizeInformation)) 
	{
		printf("  Can't get ACL info!\n");
		return 0;
	}

#define length 24
	DWORD len = length;
	CHAR user[length], domain[length];
	memset(user, 0, length);
	memset(domain, 0, length);
	SID_NAME_USE sid_nu; // тип учетной записи
	ACCESS_MASK mask; // определяет права доступа, предоставленные этим ACE
#define maxACE 12 // макс. кол-во отправляемых ACE (сейчас зависит от размеров буфера)
	void *ace;
	PSID *pSID;

	unsigned char buf[262144];
	memset(buf, 0, sizeof(buf));

	setlocale(LC_ALL, "Rus");

	unsigned char *resPtr = buf;
	unsigned short aceCount = 0;

	unsigned char maskinf[16384];
	memset(maskinf, 0, sizeof(maskinf));
	char *maskPtr = maskinf;

	// Перебор ACL-записей
	for (DWORD i = 0; i < aclInfo.AceCount; i++)
	{
		if (aceCount >= maxACE) break;

		// Получить текущую ACE-запись
		if (GetAce(pDACL, i, &ace))
		{
			unsigned char *type = ((ACCESS_ALLOWED_ACE *)ace)->Header.AceType == ACCESS_ALLOWED_ACE_TYPE ? "Allow" : "Deny"; // тип записи

			pSID = (PSID) &((ACCESS_ALLOWED_ACE *)ace)->SidStart; // SID пользователя или группы, на которые распространятся запись

			unsigned char *ownerSID;
			// Преобразуем SID владельца в строковой вид
			if (!ConvertSidToStringSidA(pSID, &ownerSID))
			{
				printf("  Can't to determine owner's SID");
				return 0;
			}

			mask = ((ACCESS_ALLOWED_ACE *)ace)->Mask; // маска доступа к данному объекту

			// Поиск имени домена и пользователя\группы по их SID
			if (LookupAccountSid(NULL, pSID, user, &len, domain, &len, &sid_nu))
			{
				aceCount++;
				sprintf(maskPtr, "Access mask bitmap [31 <- 0] :: ");  maskPtr += strlen(maskPtr);
				for (int i = mask, j = 31; j >= 0; j--)
				{
					sprintf(maskPtr, "%d%s", (i >> j) & 1, j == 0 ? "\n" : "");
					maskPtr += strlen(maskPtr);
				}

				int bit;
				(bit = (mask & DELETE) != 0 ? 1 : 0);
				sprintf(maskPtr, " [bit %.2d] :: %d -- %s\n", (int)log2f(DELETE), bit, "DELETE");	 	 	 	    maskPtr += strlen(maskPtr);
				(bit = (mask & READ_CONTROL) != 0 ? 1 : 0);
				sprintf(maskPtr, " [bit %.2d] :: %d -- %s\n", (int)log2f(READ_CONTROL), bit, "READ_CONTROL");       maskPtr += strlen(maskPtr);
				(bit = (mask & WRITE_DAC) != 0 ? 1 : 0);
				sprintf(maskPtr, " [bit %.2d] :: %d -- %s\n", (int)log2f(WRITE_DAC), bit, "WRITE_DAC");	 	        maskPtr += strlen(maskPtr);
				(bit = (mask & WRITE_OWNER) != 0 ? 1 : 0);
				sprintf(maskPtr, " [bit %.2d] :: %d -- %s\n", (int)log2f(WRITE_OWNER), bit, "WRITE_OWNER");	        maskPtr += strlen(maskPtr);
				(bit = (mask & SYNCHRONIZE) != 0 ? 1 : 0);
				sprintf(maskPtr, " [bit %.2d] :: %d -- %s\n ****\n", (int)log2f(SYNCHRONIZE), bit, "SYNCHRONIZE");  maskPtr += strlen(maskPtr);

				(bit = (mask & ACCESS_SYSTEM_SECURITY) != 0 ? 1 : 0);
				sprintf(maskPtr, " [bit %.2d] :: %d -- %s\n", (int)log2f(ACCESS_SYSTEM_SECURITY), bit, "ACCESS_SYSTEM_SECURITY");	 maskPtr += strlen(maskPtr);
				(bit = (mask & MAXIMUM_ALLOWED) != 0 ? 1 : 0);
				sprintf(maskPtr, " [bit %.2d] :: %d -- %s\n ****\n", (int)log2f(MAXIMUM_ALLOWED), bit, "MAXIMUM_ALLOWED");	 	     maskPtr += strlen(maskPtr);

				(bit = (mask & GENERIC_ALL) != 0 ? 1 : 0);
				sprintf(maskPtr, " [bit %.2d] :: %d -- %s\n", (int)log2f(GENERIC_ALL), bit, "GENERIC_ALL");	 	   	 maskPtr += strlen(maskPtr);
				(bit = (mask & GENERIC_EXECUTE) != 0 ? 1 : 0);
				sprintf(maskPtr, " [bit %.2d] :: %d -- %s\n", (int)log2f(GENERIC_EXECUTE), bit, "GENERIC_EXECUTE");  maskPtr += strlen(maskPtr);
				(bit = (mask & GENERIC_WRITE) != 0 ? 1 : 0);
				sprintf(maskPtr, " [bit %.2d] :: %d -- %s\n", (int)log2f(GENERIC_WRITE), bit, "GENERIC_WRITE");	   	 maskPtr += strlen(maskPtr);
				(bit = (mask & GENERIC_READ) != 0 ? 1 : 0);
				sprintf(maskPtr, " [bit %.2d] :: %d -- %s\n ****\n", (int)log2f(GENERIC_READ), bit, "GENERIC_READ"); maskPtr += strlen(maskPtr);

				(bit = (mask & STANDARD_RIGHTS_ALL) == STANDARD_RIGHTS_ALL ? 1 : 0);
				sprintf(maskPtr, " %d -- %s\n", bit, "STANDARD_RIGHTS_ALL");                      maskPtr += strlen(maskPtr);
				(bit = (mask & STANDARD_RIGHTS_EXECUTE) == STANDARD_RIGHTS_EXECUTE ? 1 : 0);
				sprintf(maskPtr, " %d -- %s\n", bit, "STANDARD_RIGHTS_EXECUTE");                  maskPtr += strlen(maskPtr);
				(bit = (mask & STANDARD_RIGHTS_READ) == STANDARD_RIGHTS_READ ? 1 : 0);
				sprintf(maskPtr, " %d -- %s\n", bit, "STANDARD_RIGHTS_READ");                     maskPtr += strlen(maskPtr);
				(bit = (mask & STANDARD_RIGHTS_REQUIRED) == STANDARD_RIGHTS_REQUIRED ? 1 : 0);
				sprintf(maskPtr, " %d -- %s\n", bit, "STANDARD_RIGHTS_REQUIRED");                 maskPtr += strlen(maskPtr);
				(bit = (mask & STANDARD_RIGHTS_WRITE) == STANDARD_RIGHTS_WRITE ? 1 : 0);
				sprintf(maskPtr, " %d -- %s\n", bit, "STANDARD_RIGHTS_WRITE");                    maskPtr += strlen(maskPtr);

				sprintf(resPtr, " ## ACE №%d ## \n Domain: \"%s\" \n User: \"%s\" \n Type: \"%s\"  \n SID: %s \n %s \n", aceCount, domain, user, type, ownerSID, maskinf);
				memset(maskinf, 0, sizeof(maskinf));
				maskPtr = maskinf;
				resPtr += strlen(resPtr);
			}

		}
	}

	int lenB = encrypt(buf, strlen(buf), sizeof(buf));
	memcpy(returnB, buf, lenB);
	return lenB;
}