
#include <stdio.h>
#include <winpr/crt.h>
#include <winpr/file.h>
#include <winpr/path.h>
#include <winpr/handle.h>
#include <winpr/windows.h>
#include <winpr/sysinfo.h>

int TestFileCreateFile(int argc, char* argv[])
{
	HANDLE handle = NULL;
	HRESULT hr = 0;
	DWORD written = 0;
	const char buffer[] = "Some random text\r\njust want it done.";
	char cmp[sizeof(buffer)];
	char sname[8192];
	LPSTR name = NULL;
	int rc = 0;
	SYSTEMTIME systemTime;
	WINPR_UNUSED(argc);
	WINPR_UNUSED(argv);
	GetSystemTime(&systemTime);
	(void)sprintf_s(sname, sizeof(sname),
	                "CreateFile-%04" PRIu16 "%02" PRIu16 "%02" PRIu16 "%02" PRIu16 "%02" PRIu16
	                "%02" PRIu16 "%04" PRIu16,
	                systemTime.wYear, systemTime.wMonth, systemTime.wDay, systemTime.wHour,
	                systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds);
	name = GetKnownSubPath(KNOWN_PATH_TEMP, sname);

	if (!name)
		return -1;

	/* On windows we would need '\\' or '/' as separator.
	 * Single '\' do not work. */
	hr = PathCchConvertStyleA(name, strlen(name), PATH_STYLE_UNIX);

	if (FAILED(hr))
		rc = -1;

	handle = CreateFileA(name, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_NEW,
	                     FILE_ATTRIBUTE_NORMAL, NULL);

	if (!handle)
	{
		free(name);
		return -1;
	}

	if (!winpr_PathFileExists(name))
		rc = -1;

	if (!WriteFile(handle, buffer, sizeof(buffer), &written, NULL))
		rc = -1;

	if (written != sizeof(buffer))
		rc = -1;

	written = SetFilePointer(handle, 5, NULL, FILE_BEGIN);

	if (written != 5)
		rc = -1;

	written = SetFilePointer(handle, 0, NULL, FILE_CURRENT);

	if (written != 5)
		rc = -1;

	written = SetFilePointer(handle, -5, NULL, FILE_CURRENT);

	if (written != 0)
		rc = -1;

	if (!ReadFile(handle, cmp, sizeof(cmp), &written, NULL))
		rc = -1;

	if (written != sizeof(cmp))
		rc = -1;

	if (memcmp(buffer, cmp, sizeof(buffer)) != 0)
		rc = -1;

	if (!CloseHandle(handle))
		rc = -1;

	if (!winpr_DeleteFile(name))
		rc = -1;

	if (winpr_PathFileExists(name))
		rc = -1;

	free(name);
	return rc;
}
