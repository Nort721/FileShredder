#include <stdio.h>
#include <Windows.h>

LRESULT
CALLBACK
WindowProc(
    _In_ HWND hwnd, 
    _In_ UINT uMsg, 
    _In_ WPARAM wParam, 
    _In_ LPARAM lParam
    )
{
    if (uMsg == WM_CLOSE) {
        DestroyWindow(hwnd);
    }
    else if (uMsg == WM_DESTROY) {
        PostQuitMessage(0);
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

VOID
SendWindowsNotification(
    _In_ CONST PCHAR title, 
    _In_ CONST PCHAR message
    )
{
    // Register window class
    CONST PCHAR CLASS_NAME = "NotificationWindowClass";
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandleA(NULL);
    wc.lpszClassName = CLASS_NAME;
    RegisterClassA(&wc);

    // Create a hidden window for receiving messages
    HWND hwnd = CreateWindowExA(0, CLASS_NAME, "NotificationWindow", 0,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, GetModuleHandleA(NULL), NULL);

    // Initialize NOTIFYICONDATAA structure
    NOTIFYICONDATAA nid = { 0 };
    nid.cbSize = sizeof(NOTIFYICONDATAA);
    nid.hWnd = hwnd;                    // Associate notification with window handle
    nid.uID = 1001;                     // Unique ID for the icon
    nid.uFlags = NIF_INFO | NIF_MESSAGE | NIF_ICON;
    nid.uCallbackMessage = WM_APP + 1;  // Message ID for notification events

    // Load the icon from the executable
    nid.hIcon = LoadIconA(GetModuleHandleA(NULL), MAKEINTRESOURCEA(101));

    // Set notification title and message
    strcpy_s(nid.szInfoTitle, sizeof(nid.szInfoTitle), title);  // Set notification title
    strcpy_s(nid.szInfo, sizeof(nid.szInfo), message);          // Set notification message

    nid.dwInfoFlags = NIIF_INFO;

    // Display notification
    Shell_NotifyIconA(NIM_ADD, &nid);

    // Clean up: Remove icon and unregister window class
    Shell_NotifyIconA(NIM_DELETE, &nid);
    DestroyWindow(hwnd);
    UnregisterClassA(CLASS_NAME, GetModuleHandle(NULL));
}

LPVOID
GenerateRandomData(
    _In_ DWORD dwSize
    )
{
	HCRYPTPROV hCryptProv;
	BYTE* pbRndData = (BYTE*)malloc(dwSize);

    if (!pbRndData) {
        perror("Failed to allocate buffer.\n");
        return NULL;
    }

	if (CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
	{
		if (CryptGenRandom(hCryptProv, dwSize, pbRndData))
		{
			CryptReleaseContext(hCryptProv, 0);
			return (LPCVOID)pbRndData;
		}
		CryptReleaseContext(hCryptProv, 0);
	}

	free(pbRndData);
	return NULL;
}

DWORD 
OverrideFile(
    _In_ PCHAR pFileName
    )
{
	DWORD dwBytesWritten = 0;
	HANDLE hFile = CreateFileA(pFileName, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		perror("Failed to open file.\n");
		return dwBytesWritten;
	}

	DWORD dwSize = GetFileSize(hFile, NULL);

	LPCVOID lpRndData = GenerateRandomData(dwSize);
	if (lpRndData == NULL)
	{
		perror("Failed to generate random data.\n");
		CloseHandle(hFile);
		return 0;
	}

	WriteFile(hFile, lpRndData, dwSize, &dwBytesWritten, NULL);
    free(lpRndData);

	CloseHandle(hFile);

	return dwBytesWritten;
}

BOOL
HardDelete(
    _In_ PCHAR pFileName
    )
{
	DWORD dwWritten = OverrideFile(pFileName);
	BOOL res = DeleteFileA(pFileName);
	return dwWritten > 0 && res;
}

int APIENTRY WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd
    )
{
	if (__argc != 2)
	{
        SendWindowsNotification("FileShredder - Usage", "Drag and drop a file onto FileShredder.exe to shred it.");
		return 0;
	}

    if (MessageBoxA(NULL, "This action is irreversable, Are you sure you want to permanetly erase this file.", "Are you sure?", MB_OKCANCEL | MB_ICONINFORMATION) != IDOK)
        return 0;

    if (HardDelete(__argv[1])) {
        SendWindowsNotification("File shredded successfully.", "The file has been securely overwritten and permanently erased.");
    }
    else {
        SendWindowsNotification("Failed to shred file.", "File Shredder encountered an error while attempting to shred the file. ensure the file is not being used by another process.");
    }

	return 0;
}