#include <Windows.h>
#include "wincommon.h"

void reboot_init() {
    ObtainPrivileges(SE_SHUTDOWN_NAME);
}

void do_reboot() {
    int res = ExitWindowsEx(EWX_REBOOT, 0);
    if (res == 0) {
        ReportLastErrorAndExit(TEXT("Could not reboot the system"));
    }
}
