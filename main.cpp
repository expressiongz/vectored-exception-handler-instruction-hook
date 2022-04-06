#include <iostream>
#include <windows.h>

auto veh_func(EXCEPTION_POINTERS *p_exception) {
    // check if its a page fault / page guard violation.
    if(p_exception->ExceptionRecord->ExceptionCode == STATUS_GUARD_PAGE_VIOLATION) {
        printf("%s\n", "Triggered page fault. Enabling single stepping mode...");
        // enable single stepping mode by bitwise OR'ing the eflags by 256 / 0x100.
        p_exception->ContextRecord->EFlags |= 0x100;
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    // check if  we are single stepping
    if(p_exception->ExceptionRecord->ExceptionCode == STATUS_SINGLE_STEP) {
        // print out the address of the current instruction we are at.
        printf("[stepper] Hooked instruction -> %p\n", (void*)p_exception->ExceptionRecord->ExceptionAddress);
        p_exception->ContextRecord->EFlags |= 0x100;
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    return EXCEPTION_CONTINUE_SEARCH;
}
int main() {
    DWORD old;
    PVOID veh;
    
    // add our VEH
    veh = AddVectoredExceptionHandler(1, (PVECTORED_EXCEPTION_HANDLER)veh_func);
    if(veh == NULL) { printf("%s", "Failed to get VEH Handle."); return 0; }
    
    // enable page guard on printf (this can be with any function. all we need is to cause a page guard, on literally any function and then call that function.) 
    if(!VirtualProtect((LPVOID)printf, 1, PAGE_EXECUTE_READ | PAGE_GUARD, &old)) { printf("Failed to enable page guard flag with VirtualProtect.\nGLE(get last error) code: %i", GetLastError()); return 0; }

    printf("%s", "This will not output. But the instruction address for this will.\n");
    std::cout << "woo" << "\n";
    return 0;
}
