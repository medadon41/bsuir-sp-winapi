#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <ctime>

#define MAX_THREADS 5
#define BUF_SIZE 255
#define N 1000

DWORD WINAPI MyThreadFunction(LPVOID lpParam);
void ErrorHandler(LPCTSTR lpszFunction);

// Sample custom data structure for threads to use.
// This is passed by void pointer so it can be any data type
// that can be passed using a single void pointer (LPVOID).
typedef struct MyData {
    int val1;
    int val2;
} MYDATA, * PMYDATA;


int _tmain()
{

    PMYDATA pDataArray[MAX_THREADS];
    DWORD   dwThreadIdArray[MAX_THREADS];
    HANDLE  hThreadArray[MAX_THREADS];

    // Create MAX_THREADS worker threads.

    hThreadArray[0] = CreateThread(
        NULL,                   // default security attributes
        0,                      // use default stack size  
        MyThreadFunction,       // thread function name
        NULL,                   // argument to thread function 
        0,                      // use default creation flags 
        &dwThreadIdArray[0]);   // returns the thread identifier  

    SetThreadPriority(hThreadArray[0], THREAD_PRIORITY_LOWEST);

    if (hThreadArray[0] == NULL)
    {
        ErrorHandler(TEXT("CreateThread"));
        ExitProcess(3);
    }

    hThreadArray[1] = CreateThread(
        NULL,                   // default security attributes
        0,                      // use default stack size  
        MyThreadFunction,       // thread function name
        NULL,                   // argument to thread function 
        0,                      // use default creation flags 
        &dwThreadIdArray[1]);   // returns the thread identifier  
    // Wait until all threads have terminated.

    SetThreadPriority(hThreadArray[1], THREAD_PRIORITY_NORMAL);

    hThreadArray[2] = CreateThread(
        NULL,                   // default security attributes
        0,                      // use default stack size  
        MyThreadFunction,       // thread function name
        NULL,                   // argument to thread function 
        0,                      // use default creation flags 
        &dwThreadIdArray[2]);   // returns the thread identifier  
    // Wait until all threads have terminated.

    SetThreadPriority(hThreadArray[2], THREAD_PRIORITY_HIGHEST);

    hThreadArray[3] = CreateThread(
        NULL,                   // default security attributes
        0,                      // use default stack size  
        MyThreadFunction,       // thread function name
        NULL,                   // argument to thread function 
        0,                      // use default creation flags 
        &dwThreadIdArray[3]);   // returns the thread identifier  
    // Wait until all threads have terminated.

    SetThreadPriority(hThreadArray[3], THREAD_PRIORITY_BELOW_NORMAL);

    hThreadArray[4] = CreateThread(
        NULL,                   // default security attributes
        0,                      // use default stack size  
        MyThreadFunction,       // thread function name
        NULL,                   // argument to thread function 
        0,                      // use default creation flags 
        &dwThreadIdArray[4]);   // returns the thread identifier  
    // Wait until all threads have terminated.

    SetThreadPriority(hThreadArray[4], THREAD_PRIORITY_BELOW_NORMAL);

    WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, INFINITE);


    for (int i = 0; i < MAX_THREADS; i++)
    {
        CloseHandle(hThreadArray[i]);
        
    }

    return 0;
}


DWORD WINAPI MyThreadFunction(LPVOID lpParam)
{
    HANDLE hStdout;
    PMYDATA pDataArray;

    TCHAR msgBuf[BUF_SIZE];
    size_t cchStringSize;
    DWORD dwChars;

    time_t lt;
    lt = time(NULL);

    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdout == INVALID_HANDLE_VALUE)
        return 1;

    StringCchPrintf(msgBuf, BUF_SIZE, TEXT("Thread #%d has been created with the priority level of %d\n"),
        GetCurrentThreadId(), GetThreadPriority(GetCurrentThread()));
    StringCchLength(msgBuf, BUF_SIZE, &cchStringSize);
    WriteConsole(hStdout, msgBuf, (DWORD)cchStringSize, &dwChars, NULL);

    Sleep(15000);
    int** A = (int**)malloc(N * sizeof(int*));
    int** B = (int**)malloc(N * sizeof(int*));
    int** C = (int**)malloc(N * sizeof(int*));
    for (int i = 0; i < N; i++)
    {
        A[i] = (int*)malloc(N * sizeof(int));
        B[i] = (int*)malloc(N * sizeof(int));
        C[i] = (int*)malloc(N * sizeof(int));
    }

    srand(time(NULL));
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
        {
            A[i][j] = rand() % 10;
            B[i][j] = rand() % 10;
        }

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
        {
            //Sleep(1);
            C[i][j] = 0;
            for (int k = 0; k < N; k++)
                C[i][j] += A[i][k] * B[k][j];
        }

    for (int i = 0; i < N; i++)
    {
        free(A[i]);
        free(B[i]);
        free(C[i]);
    }
    free(A);
    free(B);
    free(C);
    // Make sure there is a console to receive output results. 


    // Cast the parameter to the correct data type.
    // The pointer is known to be valid because 
    // it was checked for NULL before the thread was created.

    pDataArray = (PMYDATA)lpParam;

    // Print the parameter values using thread-safe functions.

    StringCchPrintf(msgBuf, BUF_SIZE, TEXT("Thread #%d (priority level %d) finished work and worked for %d seconds.\n"),
        GetCurrentThreadId(), GetThreadPriority(GetCurrentThread()), time(NULL)-lt);
    StringCchLength(msgBuf, BUF_SIZE, &cchStringSize);
    WriteConsole(hStdout, msgBuf, (DWORD)cchStringSize, &dwChars, NULL);

    return 0;
}



void ErrorHandler(LPCTSTR lpszFunction)
{
    // Retrieve the system error message for the last-error code.

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,
        0, NULL);

    // Display the error message.

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
    StringCchPrintf((LPTSTR)lpDisplayBuf,
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"),
        lpszFunction, dw, lpMsgBuf);
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

    // Free error-handling buffer allocations.

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
}