#include <windows.h>
#include <ctime>
#include <strsafe.h>

#define MAX_THREADS 1000
#define BUF_SIZE 255

typedef struct MyData {
    HANDLE hFileMapping;
    DWORD dwFileSize;
    DWORD offset;
    DWORD symbolsToRead;
} MYDATA, * PMYDATA;


DWORD WINAPI MyThreadFunction(LPVOID lpParam)
{
    HANDLE hStdout;
    PMYDATA pDataArray;

    TCHAR msgBuf[BUF_SIZE];
    size_t cchStringSize;
    DWORD dwChars;

    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdout == INVALID_HANDLE_VALUE)
        return 1;

    pDataArray = (PMYDATA)lpParam;

    StringCchPrintf(msgBuf, BUF_SIZE, TEXT("Thread #%d started! Offset: %d\n"),
        GetCurrentThreadId(), pDataArray->offset);
    StringCchLength(msgBuf, BUF_SIZE, &cchStringSize);
    WriteConsole(hStdout, msgBuf, (DWORD)cchStringSize, &dwChars, NULL);

    LPVOID lpMapAddress = MapViewOfFile(pDataArray->hFileMapping,
        FILE_MAP_READ,
        0,
        0,
        pDataArray->dwFileSize);

    if (lpMapAddress == NULL) {
        StringCchPrintf(msgBuf, BUF_SIZE, TEXT("\nThread #%d. Failed to map file. Error: %d\n"),
            GetCurrentThreadId(), GetLastError());
        StringCchLength(msgBuf, BUF_SIZE, &cchStringSize);
        WriteConsole(hStdout, msgBuf, (DWORD)cchStringSize, &dwChars, NULL);
    }


    char output[9];
    _itoa(((char*)lpMapAddress)[pDataArray->offset], output, 2);

    char a = ((char*)lpMapAddress)[pDataArray->offset];


    StringCchPrintf(msgBuf, BUF_SIZE, TEXT("Thread #%d. Read character: %c. In the binary code: %s \n"),
        GetCurrentThreadId(), a, output);
    StringCchLength(msgBuf, BUF_SIZE, &cchStringSize);
    WriteConsole(hStdout, msgBuf, (DWORD)cchStringSize, &dwChars, NULL);

    UnmapViewOfFile(lpMapAddress);

    return 0;
}

void _tmain(int argc, TCHAR* argv[])
{
    PMYDATA pDataArray[MAX_THREADS];
    DWORD   dwThreadIdArray[MAX_THREADS];
    HANDLE  hThreadArray[MAX_THREADS];


    HANDLE hFile;

    hFile = CreateFile(TEXT("slovaThreading.txt"), 
        GENERIC_READ,             
        0,                        
        NULL,                     
        OPEN_EXISTING,            
        FILE_ATTRIBUTE_NORMAL,    
        NULL);                    

    if (hFile == INVALID_HANDLE_VALUE)
    {
        printf("Could not open slovaThreading.txt.");
        return;
    }

    DWORD dwFileSize = GetFileSize(hFile, nullptr);

    HANDLE mappingFile = CreateFileMapping(hFile,
        NULL,
        PAGE_READONLY,
        0,
        dwFileSize,
        NULL);

    if (mappingFile == NULL) {
        printf("Could not map file slovaThreading.txt");
        return;
    }

    CloseHandle(hFile);

    for (int i = 0; i < MAX_THREADS; i++)
    {
        Sleep(10);
        pDataArray[i] = (PMYDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
            sizeof(MYDATA));

        if (pDataArray[i] == NULL)
        {
            ExitProcess(2);
        }

        pDataArray[i]->hFileMapping = mappingFile;
        pDataArray[i]->dwFileSize = dwFileSize;
        pDataArray[i]->offset = i;
        pDataArray[i]->symbolsToRead = (dwFileSize / MAX_THREADS);


        hThreadArray[i] = CreateThread(
            NULL,                 
            0,                   
            MyThreadFunction,      
            pDataArray[i],        
            0,                     
            &dwThreadIdArray[i]);



        if (hThreadArray[i] == NULL)
        {
            ExitProcess(3);
        }
    } 

    WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, INFINITE);
    
    CloseHandle(mappingFile);
}
