#include <iostream>
#include <windows.h>

long __stdcall veh_func(EXCEPTION_POINTERS* p_exception) {
    // check if its a page fault / page guard violation.
    if (p_exception->ExceptionRecord->ExceptionCode == STATUS_GUARD_PAGE_VIOLATION) {
        std::cout << "Triggered page fault. Enabling single stepping mode...\n";
        // enable single stepping mode by bitwise OR'ing the eflags by 256 / 0x100.
        p_exception->ContextRecord->EFlags |= 0x100;
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    // check if  we are single stepping
    if (p_exception->ExceptionRecord->ExceptionCode == STATUS_SINGLE_STEP) {
        // print out the address of the current instruction we are at.
        std::cout << "[stepper] Hooked instruction -> " << p_exception->ExceptionRecord->ExceptionAddress << "\n";
        p_exception->ContextRecord->EFlags |= 0x100;
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    return EXCEPTION_CONTINUE_SEARCH;
}
int main() {
    DWORD old; // buffer var for virtualprot
    // add our VEH
    const auto* veh = AddVectoredExceptionHandler(1, veh_func);
    if (veh == NULL) { 
        std::cout << "Couldn't add vectored exception handler\n";
        return -1;
    }

    // enable page guard on printf (this can be with any function. all we need is to cause a page guard, on literally any function and then call that function.) 
    if (!VirtualProtect(static_cast<void*>(printf), 1, PAGE_EXECUTE_READWRITE | PAGE_GUARD, &old)) { 
        std::cout << "Failed to enable page guard flag with VirtualProtect.\nGet last error code:  " << GetLastError() << "\n";
        return -1;
    }
    std::cout << "woo" << "\n";
    return 0;
}
