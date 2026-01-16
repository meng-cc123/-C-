#include <windows.h>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <tchar.h>
#include <iostream>  // 用于控制台输出

// 爱心配置
constexpr int MAX_HEARTS = 50;
constexpr int SCALE_RANGE[2] = {20, 80};
constexpr int LIFE_RANGE[2] = {200, 800};
constexpr int SPAWN_INTERVAL = 10;

struct Heart {
    int x, y;
    double scale;
    COLORREF color;
    int life;
};

std::vector<Heart> g_hearts;
int g_screenW, g_screenH;
HWND g_hwnd = nullptr;


COLORREF getRandomColor() {
    return RGB(rand() % 256, rand() % 256, rand() % 256);
}

int getRandomInt(int min, int max) {
    return min + rand() % (max - min + 1);
}

void drawHeart(HDC hdc, int cx, int cy, double scale, COLORREF color) {
    for (double y = 1.5; y > -1.5; y -= 0.01) {
        for (double x = -1.5; x < 1.5; x += 0.01) {
            if (pow(x*x + y*y - 1, 3) - x*x*pow(y, 3) <= 0) {
                int px = cx + static_cast<int>(x * scale);
                int py = cy - static_cast<int>(y * scale);
                if (px >= 0 && px < g_screenW && py >= 0 && py < g_screenH) {
                    SetPixel(hdc, px, py, color);
                }
            }
        }
    }
}

void spawnHeart() {
    if (g_hearts.size() >= MAX_HEARTS) return;
    g_hearts.push_back({
        getRandomInt(SCALE_RANGE[1], g_screenW - SCALE_RANGE[1]),
        getRandomInt(SCALE_RANGE[1], g_screenH - SCALE_RANGE[1]),
        getRandomInt(SCALE_RANGE[0], SCALE_RANGE[1]) / 10.0,
        getRandomColor(),
        getRandomInt(LIFE_RANGE[0], LIFE_RANGE[1])
    });
}

void updateAndDrawHearts(HDC hdc) {
    for (auto& h : g_hearts) {
        drawHeart(hdc, h.x, h.y, h.scale * 10, h.color);
        --h.life;
    }
    for (auto it = g_hearts.begin(); it != g_hearts.end();) {
        it->life <= 0 ? it = g_hearts.erase(it) : ++it;
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            updateAndDrawHearts(hdc);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_TIMER: {
            spawnHeart();
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }
        case WM_KEYDOWN:
        case WM_CLOSE: {
            KillTimer(hwnd, 1);
            DestroyWindow(hwnd);
            g_hwnd = nullptr;
            return 0;
        }
        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }
        default:
            return DefWindowProc(hwnd, msg, wp, lp);
    }
}

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    // 控制台输出启动提示 + “我爱你”
    std::cout << "请您按任意键退出我的世界" << std::endl;
    std::cout << "我爱你，这就是我的遗言。" << std::endl;  // 新增：在控制台下方输出“我爱你”

    // 获取屏幕尺寸
    g_screenW = GetSystemMetrics(SM_CXSCREEN);
    g_screenH = GetSystemMetrics(SM_CYSCREEN);

    // 注册窗口类
    WNDCLASSEX wc = {0};
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = GetModuleHandle(nullptr);
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszClassName = TEXT("TransparentHeartsClass");
    RegisterClassEx(&wc);

    // 创建透明窗口
    g_hwnd = CreateWindowEx(
        WS_EX_LAYERED,
        TEXT("TransparentHeartsClass"),
        TEXT("多爱心透明窗口"),
        WS_POPUP | WS_VISIBLE,
        0, 0, g_screenW, g_screenH,
        nullptr, nullptr, nullptr, nullptr
    );

    // 设置黑色透明
    SetLayeredWindowAttributes(g_hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);

    // 启动定时器
    SetTimer(g_hwnd, 1, SPAWN_INTERVAL, nullptr);

    // 消息循环
    MSG msg;
    while (g_hwnd && GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}