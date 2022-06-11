#include <iostream>
#include <Windows.h>

long __stdcall veh_func(EXCEPTION_POINTERS* p_exception) {

    if (p_exception->ExceptionRecord->ExceptionCode == STATUS_GUARD_PAGE_VIOLATION) {
        std::cout << "[stepper] Triggered page fault. Enabling single stepping mode...\n";
        p_exception->ContextRecord->EFlags |= 0x100;
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    else if (p_exception->ExceptionRecord->ExceptionCode == STATUS_SINGLE_STEP) {
        std::cout << "[stepper] Hooked instruction -> " << p_exception->ExceptionRecord->ExceptionAddress << "\n";
        p_exception->ContextRecord->EFlags |= 0x100;
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    return EXCEPTION_CONTINUE_SEARCH;
}
int main() {
    if (const auto* veh = AddVectoredExceptionHandler(true, veh_func); veh == nullptr) {
        std::cout << "Couldn't add vectored exception handler\n";
        return -1;
    }
    DWORD old;
    std::cout << "ty kind luxgz\n";
    VirtualProtect(&printf, 1, PAGE_EXECUTE_READWRITE | PAGE_GUARD, &old);
    std::cout << "ty kind virtualprotect";
    return 0;
}
