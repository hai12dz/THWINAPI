#include <windows.h>
#include <vector>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

std::vector<POINT> points; // Lưu danh sách điểm
bool connectPoints = false; // Biến kiểm soát việc vẽ đường nối

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = TEXT("ConnectPointsApp");
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wc.style = CS_DBLCLKS; // Cho phép bắt sự kiện double click

    RegisterClass(&wc);

    HWND hwnd = CreateWindow(wc.lpszClassName, TEXT("Connect-the-Points Mouse Demo"),
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 600, 500, nullptr, nullptr, hInstance, nullptr);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

void DrawPointsAndLines(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    SelectObject(hdc, hPen);

    // Vẽ các điểm
    for (const auto& pt : points) {
        Ellipse(hdc, pt.x - 2, pt.y - 2, pt.x + 2, pt.y + 2);
    }

    // Nếu có nhiều hơn 1 điểm và người dùng click đúp thì vẽ đường nối
    if (connectPoints && points.size() > 1) {
        for (size_t i = 0; i < points.size(); i++) {
            for (size_t j = i + 1; j < points.size(); j++) {
                MoveToEx(hdc, points[i].x, points[i].y, nullptr);
                LineTo(hdc, points[j].x, points[j].y);
            }
        }
    }

    DeleteObject(hPen);
    EndPaint(hwnd, &ps);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_LBUTTONDOWN: { // Click chuột trái để thêm điểm
        POINT pt = { LOWORD(lParam), HIWORD(lParam) };
        points.push_back(pt);
        connectPoints = false; // Reset nối điểm khi thêm điểm mới
        InvalidateRect(hwnd, nullptr, TRUE);
        break;
    }
    case WM_LBUTTONDBLCLK: { // Click đúp chuột trái để nối các điểm
        connectPoints = true;
        InvalidateRect(hwnd, nullptr, TRUE);
        break;
    }
    case WM_RBUTTONDOWN: { // Click chuột phải để xóa màn hình
        points.clear();
        connectPoints = false;
        InvalidateRect(hwnd, nullptr, TRUE);
        break;
    }
    case WM_PAINT:
        DrawPointsAndLines(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}
