#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <ctime>

int main()
{
    HANDLE hMailslot;
    BOOL   fReturnCode;
    DWORD  cbMessages;
    DWORD  cbMsgNumber;
    LPCTSTR  lpszMailslotName = TEXT("\\\\.\\mailslot\\lab5_mailslot");
    char   szBuf[512];
    DWORD  cbRead;

    time_t now = time(0);

    hMailslot = CreateMailslot(
        lpszMailslotName, 0,
        MAILSLOT_WAIT_FOREVER, NULL);

    if (hMailslot == INVALID_HANDLE_VALUE)
    {
        fprintf(stdout, "CreateMailslot: Error %ld\n",
            GetLastError());
        _getch();
        return 0;
    }

    fprintf(stdout, "Mailslot created\n");

    while (1)
    {
        tm* ltm = localtime(&now);

        fReturnCode = GetMailslotInfo(
            hMailslot, NULL, &cbMessages,
            &cbMsgNumber, NULL);

        if (!fReturnCode)
        {
            fprintf(stdout, "GetMailslotInfo: Error %ld\n",
                GetLastError());
            _getch();
            break;
        }

        if (cbMsgNumber != 0)
        {
            if (ReadFile(hMailslot, szBuf, 512, &cbRead, NULL))
            {
                printf("%s - (%d:%d)\n", szBuf, ltm->tm_hour, ltm->tm_min);

                if (!strcmp(szBuf, "exit"))
                    break;
            }
            else
            {
                fprintf(stdout, "ReadFile: Error %ld\n",
                    GetLastError());
                _getch();
                break;
            }
        }
        Sleep(500);
    }

    CloseHandle(hMailslot);
    return 0;
}