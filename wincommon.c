#include <Windows.h>
#include <stdio.h>

LPTSTR AllocAndPrintf(LPCTSTR fmt, ...) {
#ifndef UNICODE
#error "This function is implemented only for Unicode builds"
#endif
    va_list args;
    va_start(args, fmt);
    size_t length = _vsnwprintf(NULL, 0, fmt, args);
    LPWSTR result = (LPWSTR)malloc((length+1) * sizeof(WCHAR));
    _vsnwprintf(result, length+1, fmt, args);
    va_end(args);
    return result;
}

void ReportLastErrorAndExit(LPTSTR context) {
    DWORD errorCode = GetLastError();
    LPTSTR lpLastErrorMessage;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        errorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpLastErrorMessage,
        0,
        NULL);

    LPTSTR errorMessage = AllocAndPrintf(
                TEXT("%s\nReason: %s"), context, lpLastErrorMessage);
    // TODO: we could have a mode to print it out in console apps.
    MessageBox(NULL, errorMessage, NULL, MB_ICONERROR|MB_OK);
    free(errorMessage);
    LocalFree(lpLastErrorMessage);
    exit(1);
}

const wchar_t* GetWChar(const char* c) {
    const size_t cSize = strlen(c) + 1;
    wchar_t* wc = (wchar_t *)malloc(sizeof(wchar_t)*cSize);
    //mbstowcs(wc, c, cSize);
    size_t result;
    mbstowcs_s(&result, wc, cSize, c, cSize);
    return wc;
}

void ObtainPrivileges(LPCTSTR privilege) {
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;
    BOOL res;
    DWORD error;
    // Obtain required privileges
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        ReportLastErrorAndExit(TEXT("Internal error - OpenProcessTokenFailed"));
    }

    res = LookupPrivilegeValue(NULL, privilege, &tkp.Privileges[0].Luid);
    if (!res) {
        ReportLastErrorAndExit(TEXT("Internal error - LookupPrivilegeValue"));
    }
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

    error = GetLastError();
    if (error != ERROR_SUCCESS) {
        LPTSTR context = AllocAndPrintf(
            TEXT("Couldn't acquire a privilege needed by the program (%s)"), privilege);
        ReportLastErrorAndExit(context);
        free(context);
    }

}
