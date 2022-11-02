#include <windows.h>
#include <iostream>

// объ€вление функций
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
ATOM RegMyWindowClass(HINSTANCE, LPCTSTR);
STARTUPINFO si;
PROCESS_INFORMATION pi;
std::wstring ApplicationName(L"C:\\Users\\mddn\\source\\repos\\SP_Lab2\\x64\\Debug\\SP_Lab2.exe");
LPWSTR lpwAppName = &ApplicationName[0];

#define MYDISPLAY 1
typedef struct tagGEN
{
    DWORD procId;
} GEN;
COPYDATASTRUCT MyCDS;
PCOPYDATASTRUCT pMyCDS;
GEN Gen;

////////////////////////////////////////////////////////////////////////// 
// функци€ вхождений программы WinMain
int APIENTRY WinMain(HINSTANCE hInstance,
    HINSTANCE         hPrevInstance,
    LPSTR             lpCmdLine,
    int               nCmdShow)
{
    // им€ будущего класса
    LPCTSTR lpzClass = TEXT("My Window Class!");

    // регистраци€ класса
    if (!RegMyWindowClass(hInstance, lpzClass))
        return 1;

    // вычисление координат центра экрана
    RECT screen_rect;
    GetWindowRect(GetDesktopWindow(), &screen_rect); // разрешение экрана
    int x = screen_rect.right / 2 - 150;
    int y = screen_rect.bottom / 2 - 75;

    int currentProcId = GetCurrentProcessId();
    TCHAR buf[300];
    _swprintf(buf, TEXT("Dialog Window. Process ID: %d"), currentProcId);
    // создание диалогового окна
    HWND hWnd = CreateWindow(lpzClass, buf,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE, x, y, 300, 150, NULL, NULL,
        hInstance, NULL);


    HWND hButton = CreateWindow(
        L"BUTTON",
        L"Terminate",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        50, 50, 200, 30, hWnd, reinterpret_cast<HMENU>(41), nullptr, nullptr, nullptr
    );

    // если окно не создано, описатель будет равен 0
    if (!hWnd) return 2;

    // цикл сообщений приложени€
    MSG msg = { 0 };    // структура сообщени€
    int iGetOk = 0;   // переменна€ состо€ни€
    while ((iGetOk = GetMessage(&msg, NULL, 0, 0)) != 0) // цикл сообщений
    {
        if (iGetOk == -1) return 3;  // если GetMessage вернул ошибку - выход
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;  // возвращаем код завершени€ программы
}

////////////////////////////////////////////////////////////////////////// 
// функци€ регистрации класса окон
ATOM RegMyWindowClass(HINSTANCE hInst, LPCTSTR lpzClassName)
{
    WNDCLASS wcWindowClass = { 0 };
    // адрес ф-ции обработки сообщений
    wcWindowClass.lpfnWndProc = (WNDPROC)WndProc;
    // стиль окна
    wcWindowClass.style = CS_HREDRAW | CS_VREDRAW;
    // дискриптор экземпл€ра приложени€
    wcWindowClass.hInstance = hInst;
    // название класса
    wcWindowClass.lpszClassName = lpzClassName;
    // загрузка курсора
    wcWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    // загрузка цвета окон
    wcWindowClass.hbrBackground = (HBRUSH)COLOR_APPWORKSPACE;
    return RegisterClass(&wcWindowClass); // регистраци€ класса
}

////////////////////////////////////////////////////////////////////////// 
// функци€ обработки сообщений
LRESULT CALLBACK WndProc(
    HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // выборка и обработка сообщений
    switch (message)
    {
    case WM_COPYDATA:
    {
        pMyCDS = (PCOPYDATASTRUCT)lParam;
        DWORD n;
        switch (pMyCDS->dwData)
        {
        case 1:
        {
            int processId = (DWORD)((GEN*)(pMyCDS->lpData))->procId;
            TCHAR buf[300];
            _swprintf(buf, TEXT("A message from process with id: %d"), processId);
            MessageBox(hWnd, buf, TEXT("New generation"), 0);
        }
        break;
        }
    }
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case 41:
        {
            DestroyWindow(hWnd);
        }
        break;
        }
        break;
    case WM_CLOSE:
    {
        if (!CreateProcess(NULL,   // No module name (use command line)
            lpwAppName,        // Command line
            NULL,           // Process handle not inheritable
            NULL,           // Thread handle not inheritable
            FALSE,          // Set handle inheritance to FALSE
            0,              // No creation flags
            NULL,           // Use parent's environment block
            NULL,           // Use parent's starting directory 
            &si,            // Pointer to STARTUPINFO structure
            &pi)           // Pointer to PROCESS_INFORMATION structure
            )
        {
            return -1;
        }
        WaitForInputIdle(pi.hProcess, INFINITE);
        CloseHandle(pi.hThread);
        Gen.procId = (DWORD)GetCurrentProcessId();
        MyCDS.dwData = 1;          // function identifier
        MyCDS.cbData = sizeof(Gen);  // size of data
        MyCDS.lpData = &Gen;           // data structure

        TCHAR buf[300];
        _swprintf(buf, TEXT("Dialog Window. Process ID: %d"), GetProcessId(pi.hProcess));

        HWND hwDispatch = FindWindow(L"My Window Class!", buf);

        if (hwDispatch != NULL)
            SendMessage(hwDispatch,
                WM_COPYDATA,
                (WPARAM)(HWND)hWnd,
                (LPARAM)(LPVOID)&MyCDS);
        CloseHandle(pi.hProcess);
        DestroyWindow(hWnd);
    }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);  // реакци€ на сообщение
        break;
    default:
        // все сообщени€ не обработанные ¬ами обработает сама Windows
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}