#include <Windows.h>
#include "Log.h"

BOOL APIENTRY DllMain(HANDLE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {

    switch(fdwReason) {

        case DLL_PROCESS_ATTACH:
            LOG("Attach process. DLL Module Handle: %08X, Reason: %08X, Reserved: %p", hinstDLL, fdwReason, lpvReserved);
            break;

        case DLL_PROCESS_DETACH:
            LOG("Detach process. DLL Module Handle: %08X, Reason: %08X, Reserved: %p", hinstDLL, fdwReason, lpvReserved);
            break;

        case DLL_THREAD_ATTACH:
            LOG("Attach thread. DLL Module Handle: %08X, Reason: %08X, Reserved: %p", hinstDLL, fdwReason, lpvReserved);
            break;

        case DLL_THREAD_DETACH:
            LOG("Detach thread. DLL Module Handle: %08X, Reason: %08X, Reserved: %p", hinstDLL, fdwReason, lpvReserved);
            break;
    }

    return TRUE;
}
