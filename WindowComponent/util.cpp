#include "windowComponent_64.h"

bool destroyedInd = false;

//Global list of padstrings used for number padding.
wchar_t padStrings[2048][128];
int padStringsIndex = -1;

static const long double FILE_SIZE_KB = 1024.0f;
static const long double FILE_SIZE_MB = FILE_SIZE_KB * FILE_SIZE_KB;
static const long double FILE_SIZE_GB = FILE_SIZE_MB * FILE_SIZE_KB;
static const long double FILE_SIZE_TB = FILE_SIZE_GB * FILE_SIZE_KB;

static const LPWSTR KiloByte = L"KB";
static const LPWSTR  MegaByte = L"MB";
static const LPWSTR GigaByte = L"GB";
static const LPWSTR TerraByte = L"TB";
static const LPWSTR FILE_BYTES = L"(bytes)";

static const LPSTR aKiloByte = "KB";
static const LPSTR aMegaByte = "MB";
static const LPSTR aGigaByte = "GB";
static const LPSTR aTerraByte = "TB";
static const LPSTR aFILE_BYTES = "(bytes)";


//Stamp the diff in memory
void diffMemoryStatus(LPMEMORYSTATUS pMemstatFirst, LPMEMORYSTATUS pMemstatLast, LPWSTR pBufferOut, int pMaxSize)
{
	static wchar_t tmpBuffer[32];
	ZeroMemory(tmpBuffer, sizeof(wchar_t) * 32);

	__int64 initialMem = pMemstatFirst->dwAvailVirtual - pMemstatLast->dwAvailVirtual;
	formatBytes(initialMem, tmpBuffer, pMaxSize);
	swprintf_s(pBufferOut, pMaxSize, L"Diffrerence in Start/End virtual memory: %I64i (%s) \n\0", initialMem, tmpBuffer);

}

//Formats bytes.
void formatBytes(__int64 bytes, LPSTR out, int maxSize)
{
	long double big = 0.0f;
	if (bytes >= FILE_SIZE_TB)
	{
		big = bytes / FILE_SIZE_TB;
		sprintf_s(out, maxSize, "%.2f%s\0", big, aTerraByte);
		return;
	}
	if (bytes >= FILE_SIZE_GB)
	{
		big = bytes / FILE_SIZE_GB;
		sprintf_s(out, maxSize, "%.2f%s\0", big, aGigaByte);
		return;
	}
	if (bytes >= FILE_SIZE_MB)
	{
		big = bytes / FILE_SIZE_MB;
		sprintf_s(out, maxSize, "%.2f%s\0", big, aMegaByte);
		return;
	}
	if (bytes >= FILE_SIZE_KB)
	{
		big = bytes / FILE_SIZE_KB;
		sprintf_s(out, maxSize, "%.2f%s\0", big, aKiloByte);
		return;
	}
	
	sprintf_s(out, maxSize, "%.2f%s\0", (long double) bytes, aFILE_BYTES);
}

//Format bytes.
void formatBytes(__int64 bytes, LPWSTR out, int maxSize)
{
	long double big = 0.0f;
	if (bytes >= FILE_SIZE_TB)
	{
		big = bytes / FILE_SIZE_TB;
		swprintf_s(out, maxSize, L"%.2f%s\0", big, TerraByte);
		return;
	}
	if (bytes >= FILE_SIZE_GB)
	{
		big = bytes / FILE_SIZE_GB;
		swprintf_s(out, maxSize, L"%.2f%s\0", big, GigaByte);
		return;
	}
	if (bytes >= FILE_SIZE_MB)
	{
		big = bytes / FILE_SIZE_MB;
		swprintf_s(out, maxSize, L"%.2f%s\0", big, MegaByte);
		return;
	}
	if (bytes >= FILE_SIZE_KB)
	{
		big = bytes / FILE_SIZE_KB;
		swprintf_s(out, maxSize, L"%.2f%s\0", big, KiloByte);
		return;
	}
	
	swprintf_s(out, maxSize, L"%.2f %s\0", (long double) bytes, FILE_BYTES);
}

//Converts wchar_t to char
void copyWCharToChar(wchar_t *src, char *dest)
{
	//size_t convertedChars = 0;
	size_t size = wcslen(src) + 1;
    wcstombs_s(NULL, dest, size, src, _TRUNCATE);
}


//Converts char to wchar_t
void copyCharToWChar(char *src, wchar_t *dest)
{
	//size_t convertedChars = 0;
	size_t size = strlen(src) + 1;
	mbstowcs_s(NULL, dest, size, src, _TRUNCATE);
}

void destroyAllContainers()
{
	HWNDContainer **g = getHWNDS();
	for (int i=0; i<2048; i++)	
		if (g[i] && g[i]->container)
		{
			delete g[i];
			g[i] = NULL;
		}
	free(g);
	g = NULL;

}

//Destroys static data.
void destroyStatic(HWND hwnd)
{
	HWNDContainer **g = getHWNDS();
	for (int i=0; i<2048; i++)	
		if (g[i] && g[i]->container)
			if (g[i]->hwnd == hwnd)
			{
				delete g[i];
				g[i] = NULL;
			}
}

//Formats time in ms.
void formatTimeFromMs(long long timeInMs, LPWSTR out, int size)
{
    float seconds = (float) timeInMs / 1000.0f;
    float secondsRemain = (float)(timeInMs % 1000);

    float minutes = 0.0f;
	if ((long long)seconds % 60 >= 0)
    {
      minutes = (float) seconds / 60.0f;
	  seconds = (float)((long long)seconds % 60);
    }

    float hours = 0.0f;
	if ((long long)minutes % 60 >= 0)
    {
      hours = (float) minutes / 60.0f;
	  minutes = (float)((long long)minutes % 60);
    }

	swprintf_s(out, size, L"%i:%i:%i.%i\0", (long long)hours, (long long)minutes, (long long)seconds, (long long)secondsRemain);

}

//Formats time in ms.
void formatTimeFromMs(long long timeInMs, LPSTR out, int size)
{
    float seconds = (float) timeInMs / 1000.0f;
    float secondsRemain = (float)(timeInMs % 1000);

    float minutes = 0.0f;
	if ((long long)seconds % 60 > 0)
    {
      minutes = (float) seconds / 60.0f;
	  seconds = (float)((long long)seconds % 60);
    }

    float hours = 0.0f;
	if ((long long)minutes % 60 > 0)
    {
      hours = (float) minutes / 60.0f;
	  minutes = (float)((long long)minutes % 60);
    }

	sprintf_s(out, size, "%i:%i:%i.%i\0", (long long)hours, (long long)minutes, (long long)seconds, (long long)secondsRemain);

}

//Pads a string.
LPWSTR padString(LPWSTR strToPad, wchar_t charToPad, int length, bool leftPad)
{
	int len = (int) wcslen(strToPad);
	while (len < length)
	{
		if (leftPad)
		{
			//Shift all characters 1 to the right
			if (len > 0)
			{
				for (int i=len-1; i>=0; i--)
				{
					strToPad[i+1] = strToPad[i];
				}
			}
			//Pad first char
			strToPad[0] = charToPad;
		}
		else
		{
			//Right pad.
			strToPad[len] = charToPad;
		}
		
		len = (int) wcslen(strToPad);
	}
	return strToPad;
}

LPSTR padString(LPSTR strToPad, char charToPad, int length, bool leftPad)
{
	int len = (int) strlen(strToPad);
	while (len < length)
	{
		if (leftPad)
		{
			//Shift all characters 1 to the right
			if (len > 0)
			{
				for (int i=len-1; i>=0; i--)
				{
					strToPad[i+1] = strToPad[i];
				}
			}
			//Pad first char
			strToPad[0] = charToPad;
		}
		else
		{
			//Right pad.
			strToPad[len] = charToPad;
		}
		
		len = (int) strlen(strToPad);
	}
	return strToPad;

}

//Number padding.
LPWSTR padString(int number, wchar_t charToPad, int length, bool leftPad)
{
	padStringsIndex++;
	if (padStringsIndex >= 2048)
		padStringsIndex = 0;

	ZeroMemory(padStrings[padStringsIndex], sizeof(wchar_t) * 128);
	swprintf_s(padStrings[padStringsIndex], 128, L"%i", number);
	padString(padStrings[padStringsIndex], charToPad, length, leftPad);
	return padStrings[padStringsIndex];
}

LPWSTR padString(long number, wchar_t charToPad, int length, bool leftPad)
{
	padStringsIndex++;
	if (padStringsIndex >= 2048)
		padStringsIndex = 0;

	ZeroMemory(padStrings[padStringsIndex], sizeof(wchar_t) * 128);
	swprintf_s(padStrings[padStringsIndex], 128, L"%i", number);
	padString(padStrings[padStringsIndex], charToPad, length, leftPad);
	return padStrings[padStringsIndex];

}

LPWSTR padString(double number, wchar_t charToPad, int length, bool leftPad)
{
	padStringsIndex++;
	if (padStringsIndex >= 2048)
		padStringsIndex = 0;

	ZeroMemory(padStrings[padStringsIndex], sizeof(wchar_t) * 128);
	swprintf_s(padStrings[padStringsIndex], 128, L"%f", number);
	padString(padStrings[padStringsIndex], charToPad, length, leftPad);
	return padStrings[padStringsIndex];

}

LPWSTR padString(float number, wchar_t charToPad, int length, bool leftPad)
{
	padStringsIndex++;
	if (padStringsIndex >= 2048)
		padStringsIndex = 0;

	ZeroMemory(padStrings[padStringsIndex], sizeof(wchar_t) * 128);
	swprintf_s(padStrings[padStringsIndex], 128, L"%f", number);
	padString(padStrings[padStringsIndex], charToPad, length, leftPad);
	return padStrings[padStringsIndex];

}

LPWSTR padString(__int64 number, wchar_t charToPad, int length, bool leftPad)
{
	padStringsIndex++;
	if (padStringsIndex >= 2048)
		padStringsIndex = 0;

	ZeroMemory(padStrings[padStringsIndex], sizeof(wchar_t) * 128);
	swprintf_s(padStrings[padStringsIndex], 128, L"%I64d", number);
	padString(padStrings[padStringsIndex], charToPad, length, leftPad);
	return padStrings[padStringsIndex];

}

//Get last error, and format message into string.
void getLastErrorMsg(DWORD le, LPWSTR msgBuffer)
{
	//Format msg.
	ZeroMemory(msgBuffer, sizeof(wchar_t) * 2048);
	FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, le, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), msgBuffer, 2048, NULL );

	int len = (int) wcslen(msgBuffer);
	msgBuffer[len] = L'\n';
}

//Logs the last error.
void logLastError(AlCLogger *vLogger, LPWSTR msgBuffer, bool showMsg)
{
	//Get the last error code.
	DWORD le = GetLastError();

	if (le == 0)
		return;

	getLastErrorMsg(le, msgBuffer);

	if (vLogger != NULL)
		vLogger->debug(msgBuffer);

	if (showMsg)
		MessageBoxW(NULL, msgBuffer, L"Error Occurred", MB_OK);
}

//Checks if a file exists.
bool fileexists(LPWSTR fileName)
{

	bool ret = true;

	wifstream file;
	file.open(fileName, ios::in | ios::binary);
	if (file.fail())
	{
		ret = false;
	}
	else
	{
		file.close();
	}

	return ret;
}

//Checks if a file exists.
bool fileexists(LPSTR fileName)
{

	bool ret = true;

	ifstream file;
	file.open(fileName, ios::in | ios::binary);
	if (file.fail())
	{
		ret = false;
	}
	else
	{
		file.close();
	}

	return ret;
}

/**
	Determines the file size.
**/
INT64 determineFileSize(LPWSTR fileName)
{
	INT64 len = 0;

	FILE *file = NULL;
	if (_wfopen_s(&file, fileName, L"rb") != 0)
		goto exit;

	if (_fseeki64(file, 0, SEEK_SET) != 0)
		goto exit;
	__int64 begin = _ftelli64(file);

	if (_fseeki64(file, 0, SEEK_END) != 0)
		goto exit;
	__int64 end = _ftelli64(file);

	len = end - begin;
	
exit:
	if (file)
		fclose(file);

	return len;

}


/**
	Determines the file size.
**/
INT64 determineFileSize(LPSTR fileName)
{
	INT64 len = 0;

	FILE *file = NULL;
	if (fopen_s(&file, fileName, "rb") != 0)
		goto exit;

	if (_fseeki64(file, 0, SEEK_SET) != 0)
		goto exit;
	__int64 begin = _ftelli64(file);

	if (_fseeki64(file, 0, SEEK_END) != 0)
		goto exit;
	__int64 end = _ftelli64(file);

	len = end - begin;

exit:
	if (file)
		fclose(file);

	return len;

}



/** Seeds Random. */
void seedRandom()
{
	srand((unsigned)time(0));
}

/** Generate a random float between 2 floats (incl). */
float random(float min, float max)
{
	return min + (float)rand() / ( (float)RAND_MAX / ( max-min ) );
}

/**
	Gets the time in milliseconds.
**/
__int64 milliseconds_now() {
	LARGE_INTEGER s_frequency;
	LARGE_INTEGER now;

	QueryPerformanceFrequency(&s_frequency);
	QueryPerformanceCounter(&now);
	return (1000LL * now.QuadPart) / s_frequency.QuadPart;
}


/**
Increments an IP address.
**/
char *increment_address(char* address_string)
{

	// convert the input IP address to an integer
	u_long address = inet_addr(address_string);

	// add one to the value (making sure to get the correct byte orders)
	address = ntohl(address);
	address += 1;
	address = htonl(address);

	// pack the address into the struct inet_ntoa expects
	struct in_addr address_struct;
	address_struct.s_addr = address;

	// convert back to a string
	return inet_ntoa(address_struct);
}


/**
	Checks if a string is numeric.
**/
bool isnumeric(char* valtocheck)
{
	bool retval = false;

	size_t l = strlen(valtocheck);


	if (l > 0)
	{
		retval = true;
		for (size_t i = 0; i < l; i++)
		{
			if (!std::isdigit(valtocheck[i]))
			{
				retval = false;
				break; //for
			} //if
		} //for
	} //if

	return retval;
}



/**
	Get total physical memory.
**/
unsigned long long DLLEXPORT getTotalPhysMemory()
{
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	unsigned long long totalPhysMem = memInfo.ullTotalPhys;

	return totalPhysMem;
}


/**
	Get process physical memory.
**/
unsigned long long  DLLEXPORT getProcessPhysicalMemory()
{
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(PROCESS_MEMORY_COUNTERS));
	unsigned long long physMemUsedByMe = pmc.PagefileUsage;

	return physMemUsedByMe;
}






/**
	CalculateCPULoad.
**/
float CalculateCPULoad(unsigned long long idleTicks, unsigned long long totalTicks)
{
	static unsigned long long _previousTotalTicks = 0;
	static unsigned long long _previousIdleTicks = 0;

	unsigned long long totalTicksSinceLastTime = totalTicks - _previousTotalTicks;
	unsigned long long idleTicksSinceLastTime = idleTicks - _previousIdleTicks;

	float ret = 1.0f - ((totalTicksSinceLastTime > 0) ? ((float)idleTicksSinceLastTime) / totalTicksSinceLastTime : 0);

	_previousTotalTicks = totalTicks;
	_previousIdleTicks = idleTicks;
	return ret;
}


/** 
	Returns 1.0f for "CPU fully pinned", 0.0f for "CPU idle", or somewhere in between
	You'll need to call this at regular intervals, since it measures the load between
	the previous call and the current one.  Returns -1.0 on error.
**/
float GetCPULoad()
{
	FILETIME idleTime, kernelTime, userTime;
	return GetSystemTimes(&idleTime, &kernelTime, &userTime) ? CalculateCPULoad(FileTimeToInt64(idleTime), FileTimeToInt64(kernelTime) + FileTimeToInt64(userTime)) : -1.0f;
}

/**
	Does stuff.
**/
unsigned long long FileTimeToInt64(const FILETIME & ft) 
{ 
		return (((unsigned long long)(ft.dwHighDateTime)) << 32) | ((unsigned long long)ft.dwLowDateTime); 
}



/**
	Binary to long! (32 bit)
**/
long bintolong32(char* str)
{
	char *ptr;
	long parsed = strtol(str, &ptr, 2);
	return parsed;
}

/**
	Binary to long! (64 bit)
**/
long long bintolong64(char* str)
{
	char *ptr;
	long long parsed = strtoll(str, &ptr, 2);
	return parsed;
}


/**
	Splits a full path into filename and path.
**/
void splitFileNameAndPath(char* fullPath, char* pathOut, char* fileNameOut)
{
	int len = strlen(fullPath);
	int find = -1;
	for (int i = len; i >= 0; i--)
	{
		find = i;
		if ( fullPath[i] == '\\' || fullPath[i] == '/' )
		{
			break; //for
		} //if
	} //for

	if (find < 0)
	{
		// No path.
		strcpy(fileNameOut, fullPath);
	} //if
	else
	{
		
		// copy path
		for (int i = 0; i < find; i++)
		{
			pathOut[i] = fullPath[i];
		} //for

		//copy filename
		char *ptr = fullPath;
		ptr += find + 1;
		strcpy(fileNameOut, ptr);


	} //else
	

}
