#include <iostream>
#include <Windows.h>

long __stdcall handler(EXCEPTION_POINTERS* exception) {

    if (exception->ExceptionRecord->ExceptionCode == STATUS_GUARD_PAGE_VIOLATION) {
        std::cout << "[stepper] Triggered page fault. Enabled single step\n";
        exception->ContextRecord->EFlags |= 0x100;
        return EXCEPTION_CONTINUE_EXECUTION;
    }

	if (exception->ExceptionRecord->ExceptionCode == STATUS_SINGLE_STEP) {
        std::printf("[stepper] Hooked instruction -> 0x%p\n", exception->ExceptionRecord->ExceptionAddress);
        exception->ContextRecord->EFlags |= 0x100;
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

int main() {
    AddVectoredExceptionHandler(true, &handler);
    DWORD vp_old_protection{ 0 };
    VirtualProtect(&printf, 1, PAGE_EXECUTE_READ | PAGE_GUARD, &vp_old_protection);
    std::cout << "ty kind virtualprotect";
    return 0;
}
