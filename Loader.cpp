#define WIN32_LEAN_AND_MEAN
#undef UNICODE

#include <windows.h>
#include <iostream>
#include <string>
#include <tlhelp32.h>
#include <psapi.h>
#include <memoryapi.h>

#pragma comment(lib, "kernel32.lib")

const char* KnownNames[] = {
	"th12.exe",
	"touhou12.exe",
	"touhou.exe",
	"th.exe"
};

int main(void) {
	char DllPath[MAX_PATH + 1] = "C:\\Users\\brat_volk\\Desktop\\cpp\\MemExperiments\\Release\\MemExperiments.dll";
	PROCESSENTRY32 pe32 = { NULL };

	/*HANDLE hToken;
	LUID luid;
	TOKEN_PRIVILEGES tkp;

	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid);

	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = luid;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL);
	CloseHandle(hToken);*/

	HANDLE ProcHandle;
	HANDLE hSnapshot;
	LPVOID AllocMemAdress;
	LPVOID BaseMemAddress;
	SIZE_T BytesWritten;
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot) {
		pe32.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hSnapshot, &pe32)) {
			do {
				for (int i = 0; i < strlen(pe32.szExeFile); i++) {
					pe32.szExeFile[i] = tolower(pe32.szExeFile[i]);
				}
				for (int i = 0; i < sizeof(KnownNames) / sizeof(KnownNames[0]); i++) {
					if (!strcmp(pe32.szExeFile, KnownNames[i])) {
						std::cout << "Touhou UFO process found: \"" << KnownNames[i] << "\", PID:" << pe32.th32ProcessID << std::endl;
						goto FoundPid;
					}
				}
			} while (Process32Next(hSnapshot, &pe32));
			std::cout << "Couldn't find Touhou, quitting...";
			exit(0);
		}
		CloseHandle(hSnapshot);
	}
FoundPid:
	ProcHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
	AllocMemAdress = VirtualAllocEx(ProcHandle, NULL, strlen(DllPath) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	WriteProcessMemory(ProcHandle, AllocMemAdress, DllPath, strlen(DllPath) + 1, &BytesWritten);
	BaseMemAddress = (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
	std::cout << "LoadLibraryA Address: 0x" << BaseMemAddress << std::endl;
	HANDLE ThreadHandle = CreateRemoteThread(ProcHandle,NULL,NULL,(LPTHREAD_START_ROUTINE)LoadLibraryA, (LPDWORD)AllocMemAdress,NULL,NULL);
	WaitForSingleObject(ThreadHandle, INFINITE);
	CloseHandle(ThreadHandle);
	VirtualFreeEx(ProcHandle,DllPath,0,MEM_RELEASE);
	CloseHandle(ProcHandle);
	std::cout << "Remote Thread exited succesfuly." << std::endl;
}

std::string CtoS(const char *p) {
	std::string s = p;
	std::cout << s;
	return s;
}