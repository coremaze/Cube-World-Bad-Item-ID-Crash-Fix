#include <windows.h>



unsigned int base;
unsigned int invalid_JMP_back;
unsigned int valid_JMP_back;
void __declspec(naked) __declspec(noinline) CheckNullASM(){
    asm("test eax, eax"); //check if sprite is null ptr
    asm("jz 0f");

    asm("mov ecx, eax"); //original code
    asm("mov [esp+0x14], eax");

    asm("jmp [_valid_JMP_back]");

    asm("0:"); //can't dereference in a jz; need to use a jmp
    asm("jmp [_invalid_JMP_back]");

}
void WriteJMP(BYTE* location, BYTE* newFunction){
	DWORD dwOldProtection;
	VirtualProtect(location, 5, PAGE_EXECUTE_READWRITE, &dwOldProtection);
    location[0] = 0xE9; //jmp
    *((DWORD*)(location + 1)) = (DWORD)(( (unsigned INT32)newFunction - (unsigned INT32)location ) - 5);
	VirtualProtect(location, 5, dwOldProtection, &dwOldProtection);
}

extern "C" __declspec(dllexport) bool APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            base = (unsigned int)GetModuleHandle(NULL);
            invalid_JMP_back = base + 0xB2C50;
            valid_JMP_back = base + 0xB22A1;
            WriteJMP((BYTE*)(base + 0xB229B), (BYTE*)&CheckNullASM);
            break;

    }
    return true;
}
