
#define _CRT_SECURE_NO_WARNINGS

#include "framework.h"
#include "Resource.h"

#define MAX_LOADSTRING 100
#define windowH 720
#define windowW 1280
#define FPS 60
#define particles 4000
#define fountainX windowW / 2
#define fountainY 200
#define angles 140+20
#define particleW 1
#define particleH 1
#define particleS 80+20


HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING]; 
HWND hWnd;

struct {
    int birth,angle;
    float V0;
} typedef particle;

struct {
    uint8_t blue, green, red, header;
}typedef pixel_mem;

particle Droplets[particles];
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
    LoadStringW(hInstance, IDC_FOUNTAIN, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FOUNTAIN));
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
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FOUNTAIN));
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
void MyRenderer(){

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
    //rand-fill my Droplets
    for (int i = 0; i < particles; i++) {
        Droplets[i].angle = rand()%angles;
        Droplets[i].birth = 0;
        Droplets[i].V0 = rand()%particleS;
    }


    //infinite Draw cycle
    for (int timer=0;;timer++) {
        DrawFrame(timer);
        Sleep(10 / FPS);
    }

}


//assemble and draw all the particles
void DrawFrame(int CurrentFrame){
    ZeroMemory(memory, windowH * windowW * 4);    //clear the buffer!
    pixel_mem *pixel = (pixel_mem*)memory;
    int x, y, Time;            
    for (int i = 0; i < particles; i++) {
        //derive time from the iterations and fps
        Time = (CurrentFrame - Droplets[i].birth) / FPS;
        //      x = v0 cos( angle )t        [[[[[NEED TO ADD FOUNTAIN OFFSET]]]]]
        x = Droplets[i].V0 * cos(Droplets[i].angle * 0.017453) * Time  +fountainX;
        //      y = v0 sin( angle )t - gt   [[[[[NEED TO ADD FOUNTAIN OFFSET]]]]]
        y = Droplets[i].V0 * sin(Droplets[i].angle * 0.017453) * Time - (0.5*(9.8 * Time*Time)) + fountainY;
        //MessageBoxA(NULL,std::to_string(x).c_str(), std::to_string(y).c_str(),MB_OK);
        
        //if OOB reset it
        if (y - 20 < 0 || x - 20 < 0 || x + 20 > windowW) {
            Droplets[i].angle = rand() %angles;
            Droplets[i].birth = CurrentFrame;
            Droplets[i].V0 = rand()%particleS;
            continue;
        }
        //if OOB upwards then just don't render and wait for it to come down
        if (y + 20 > windowH)
            continue;

        
        for (int j = 0; j < particleW; j++) {
            //pixel[y * (windowW-j) + x + j].red = 0xff;
            //pixel[y * (windowW-j) + x - j].red = 0xff;
            pixel[y * (windowW) + x + j].blue = 0xff;
            pixel[y * (windowW) + x].blue = 0xff;
            pixel[y * (windowW) + x - j].blue = 0xff;
            for (int k = 0; k< particleH; k++) {
                pixel[(y+j) * (windowW) + x + k].blue = 0xff;
                pixel[(y+j) * (windowW) + x - k].blue = 0xff;
            }
        }
    }
    //bitmap to device context aka my window
    StretchDIBits(hdc, 0, 0, windowW, windowH, 0, 0, windowW, windowH, memory, &bitmap_info, DIB_RGB_COLORS, SRCCOPY);
}
