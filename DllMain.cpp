#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#ifdef UNICODE
	#undef UNICODE
#endif UNICODE
#include <iostream>
#include <string.h>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <thread>


void WriteToMem(HANDLE Handle, UINT_PTR Address, int Value);

struct AddressKeyCombo {
	UINT_PTR Address;
	int KeyPress;
	int NewValue;
	char Message[150];
};

AddressKeyCombo PointerTable[] = {
	{0x004B0C98,VK_LCONTROL,8,"HP Maxed out!"},				//HP
	{0x004B0C48,VK_INSERT,500,"Power Maxed out!"},			//POWER
	{0x004B30EC,VK_RCONTROL,69420,"Funny score set!"},		//SCORE
	{0x004B0CA0,VK_RSHIFT,8,"Spells Maxed out!"}			//SPELLS

};

/*
BOOL WINAPI DllMain() {
	PROCESSENTRY32 pe32 = { NULL };

	HANDLE Handle;
	Handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
	if (!Handle) return 0;
	else {
		for (;;) {
			for (int i = 0; i < (sizeof(PointerTable) / sizeof(PointerTable[0])); i++) {
				if (GetAsyncKeyState(PointerTable[i].KeyPress) == -32767) { //If Left-Control is pressed
					WriteToMem(Handle, PointerTable[i].Address, PointerTable[i].NewValue);
					std::cout << PointerTable[i].Message << std::endl;
				}
			}
		}
	}
}*/

int MainFun() {
	int Array[10] = { 0 };
	AllocConsole();
	for (;;) {
		for (int i = 0; i < (sizeof(PointerTable) / sizeof(PointerTable[0])); i++) {
			if (GetAsyncKeyState(PointerTable[i].KeyPress) == -32767) { //If Left-Control is pressed
				WriteToMem(GetCurrentProcess(), PointerTable[i].Address, PointerTable[i].NewValue);
				std::cout << PointerTable[i].Message << std::endl;
			}
		}
	}
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{

		MessageBoxA(NULL,"Process Attached Succesfuly", "DllAttach", MB_ICONINFORMATION);
		std::thread t1(MainFun);
		t1.detach();

		return TRUE;
	}

	return FALSE;
}


void WriteToMem(HANDLE Handle, UINT_PTR Address, int Value) {
	DWORD Old, New;
	VirtualProtectEx(Handle, reinterpret_cast<void*>(Address), sizeof(Value), PAGE_EXECUTE_READWRITE, &Old);
	WriteProcessMemory(Handle, reinterpret_cast<void*>(Address), &Value, sizeof(Value), nullptr);
	VirtualProtectEx(Handle, reinterpret_cast<void*>(Address), sizeof(Value), Old, &New);
}