#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>

char* GetRequestString(char msg[], char userName[]);

DWORD main(int argc, char* argv[])
{
    LPCTSTR  lpszMailslotName = TEXT("\\\\.\\mailslot\\lab5_mailslot");
    HANDLE hMailslot;
    char   szMailslotName[256];
    char   szBuf[1024];
    char   userName[50];
    DWORD  cbWritten;

    printf("Enter your user name (up 50 symbols): ");
    BOOL wrongInput = TRUE;
    while (wrongInput) {
        char buff[4096];
        gets_s(buff);
        if (strlen(buff) < 50) {
            wrongInput = FALSE;
            strcpy(userName, buff);
            break;
        }
        fprintf(stdout, "\nWrong input, try again: ");
    }

    hMailslot = CreateFile(
        lpszMailslotName, GENERIC_WRITE,
        FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

    if (hMailslot == INVALID_HANDLE_VALUE)
    {
        fprintf(stdout, "CreateFile: Error %ld\n",
            GetLastError());
        _getch();
        return 0;
    }

    fprintf(stdout, "\nConnected. Type 'exit' to terminate\n");

    while (1)
    {
        printf("%s>", userName);

        wrongInput = TRUE;
        while (wrongInput) {
            char buff[4096];
            gets_s(buff);
            if (strlen(buff) < 1024) {
                wrongInput = FALSE;
                strcpy(szBuf, buff);
                break; 
            }
            fprintf(stdout, "\nWrong input, try again: ");
        }

        char request[1050];
        strcpy(request, GetRequestString(szBuf, userName));

        if (!WriteFile(hMailslot, request, strlen(request) + 1,
            &cbWritten, NULL))
            break;

        if (!strcmp(szBuf, "exit"))
            break;
    }

    CloseHandle(hMailslot);
    return 0;
}

char* GetRequestString(char msg[], char userName[]) {
    char result[1050];
    strcpy(result, userName);
    strcat(result, ": ");
    strcat(result, msg);
    return result;
}