#include <iostream>
#include <windows.h>

auto veh_func(EXCEPTION_POINTERS *p_exception) {
    if(p_exception->ExceptionRecord->ExceptionCode == STATUS_GUARD_PAGE_VIOLATION) {
        printf("%s\n", "Triggered page fault. Enabling single stepping mode...");
        p_exception->ContextRecord->EFlags |= 0x100;
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    if(p_exception->ExceptionRecord->ExceptionCode == STATUS_SINGLE_STEP) {
        printf("[stepper] Hooked instruction -> %p\n", (void*)p_exception->ExceptionRecord->ExceptionAddress);
        p_exception->ContextRecord->EFlags |= 0x100;
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    return EXCEPTION_CONTINUE_SEARCH;
}
int main() {
    DWORD old;
    auto veh = AddVectoredExceptionHandler(1, (PVECTORED_EXCEPTION_HANDLER)veh_func);
    VirtualProtect((LPVOID)printf, 1, PAGE_EXECUTE_READ | PAGE_GUARD, &old);
    printf("%s", "enable single stepping.");
    if(veh == NULL) {
        printf("%s", "Failed to add VEH.");
        return 0;
    }
    return 0;
}
