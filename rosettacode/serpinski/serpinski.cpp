
#define _CRT_SECURE_NO_WARNINGS

#include "framework.h"
#include "Resource.h"

#define MAX_LOADSTRING 100
#define windowH 720
#define windowW 1280
#define FPS 60


HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
HWND hWnd;

struct {
    int X, Y;
} typedef point;

struct {
    uint8_t blue, green, red, header;
}typedef pixel_mem;

void* memory;

HDC hdc;
BITMAPINFO bitmap_info;


ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void DrawFrame(int CurrentFrame);
void MyRenderer();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR    lpCmdLine, _In_ int       nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SERPINSKI, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SERPINSKI));
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SERPINSKI));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;
    hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX, CW_USEDEFAULT, 0, windowW, windowH, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    std::thread t1(MyRenderer);
    t1.detach();



    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

//main renderer loop
void MyRenderer() {

    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

    srand(time(NULL));

    hdc = GetDC(hWnd);

    bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
    bitmap_info.bmiHeader.biWidth = windowW;
    bitmap_info.bmiHeader.biHeight = windowH;
    bitmap_info.bmiHeader.biPlanes = 1;
    bitmap_info.bmiHeader.biBitCount = 32;
    bitmap_info.bmiHeader.biCompression = BI_RGB;

    memory = VirtualAlloc(0, windowH * windowW * 4, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    DrawFrame(0);

}


//assemble and draw all the particles
void DrawFrame(int CurrentFrame) {
    ZeroMemory(memory, windowH * windowW * 4);    //clear the buffer!
    pixel_mem* pixel = (pixel_mem*)memory;
    point Middle,Vertices[3] = {0};

    Vertices[0].X = windowW / 2;
    Vertices[0].Y = windowH / 3 * 2;

    Vertices[1].X = windowW / 3;
    Vertices[1].Y = windowH / 3;

    Vertices[2].X = windowW / 3 * 2;
    Vertices[2].Y = windowH / 3;

    Middle.X = rand() % windowW;
    Middle.Y = rand() % windowH;

    for (int l,i = 0; i < 40000; i++) {
        l = rand() % 3;
        Middle.X = (Middle.X + Vertices[l].X) / 2;
        Middle.Y = (Middle.Y + Vertices[l].Y) / 2;
        if(i==1)
            MessageBoxA(NULL,"","",MB_OK);
        pixel[(int)(Middle.Y * windowW)+Middle.X].green = 0xff;
    }
    //bitmap to device context aka my window
    StretchDIBits(hdc, 0, 0, windowW, windowH, 0, 0, windowW, windowH, memory, &bitmap_info, DIB_RGB_COLORS, SRCCOPY);
}
