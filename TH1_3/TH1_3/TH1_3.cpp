#include <windows.h>
#include <vector>
#include <string>
#include <sstream>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DrawShapes(HDC hdc);
void UpdateTimer(HWND hwnd);

#define ID_HINH_TAMGIACVUONG  101
#define ID_HINH_DUONGTHANG    102
#define ID_MAU_DO             201
#define ID_MAU_XANH           202

struct Shape {
    int type;
    RECT rect;
    COLORREF color;
};

std::vector<Shape> shapes;
int shapeType = ID_HINH_DUONGTHANG;
COLORREF lineColor = RGB(255, 165, 0);
POINT startPt, endPt;
bool isDrawing = false;
int elapsedMinutes = 0, elapsedSeconds = 0;
UINT_PTR timerID = 1;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = TEXT("StudentDrawingApp");
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);

    RegisterClass(&wc);

    HWND hwnd = CreateWindow(wc.lpszClassName, TEXT("Nguyen The Hoang - 023456"),
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 500,
        nullptr, nullptr, hInstance, nullptr);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    SetTimer(hwnd, timerID, 1000, NULL);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

void DrawShapes(HDC hdc) {
    for (const auto& shape : shapes) {
        HPEN hPen = CreatePen(PS_SOLID, 3, shape.color);
        SelectObject(hdc, hPen);

        if (shape.type == ID_HINH_TAMGIACVUONG) {
            POINT pts[4] = {
                { shape.rect.left, shape.rect.top },
                { shape.rect.left, shape.rect.bottom },
                { shape.rect.right, shape.rect.bottom },
                { shape.rect.left, shape.rect.top }
            };
            Polyline(hdc, pts, 4);
        }
        else if (shape.type == ID_HINH_DUONGTHANG) {
            MoveToEx(hdc, shape.rect.left, shape.rect.top, NULL);
            LineTo(hdc, shape.rect.right, shape.rect.bottom);
        }

        DeleteObject(hPen);
    }
}

void UpdateTimer(HWND hwnd) {
    elapsedSeconds++;
    if (elapsedSeconds == 60) {
        elapsedSeconds = 0;
        elapsedMinutes++;
    }

    // Yêu cầu vẽ lại toàn bộ cửa sổ để cập nhật đồng hồ
    InvalidateRect(hwnd, NULL, FALSE);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HMENU hMenu, hSubMenuHinh, hPopupMenu;
    RECT clientRect;

    switch (msg) {
    case WM_CREATE: {
        hMenu = CreateMenu();
        hSubMenuHinh = CreatePopupMenu();
        AppendMenu(hSubMenuHinh, MF_STRING, ID_HINH_TAMGIACVUONG, TEXT("Tam giác vuông"));
        AppendMenu(hSubMenuHinh, MF_STRING, ID_HINH_DUONGTHANG, TEXT("Đường thẳng"));

        AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hSubMenuHinh, TEXT("Hình"));
        SetMenu(hwnd, hMenu);

        hPopupMenu = CreatePopupMenu();
        AppendMenu(hPopupMenu, MF_STRING, ID_MAU_DO, TEXT("Đỏ"));
        AppendMenu(hPopupMenu, MF_STRING, ID_MAU_XANH, TEXT("Xanh lá cây"));
        break;
    }
    case WM_COMMAND: {
        int menuId = LOWORD(wParam);

        if (menuId == ID_HINH_TAMGIACVUONG || menuId == ID_HINH_DUONGTHANG) {
            shapeType = menuId;
        }
        else if (menuId == ID_MAU_DO) {
            lineColor = RGB(255, 0, 0);
        }
        else if (menuId == ID_MAU_XANH) {
            lineColor = RGB(0, 255, 0);
        }
        break;
    }
    case WM_LBUTTONDOWN:
        isDrawing = true;
        startPt.x = LOWORD(lParam);
        startPt.y = HIWORD(lParam);
        break;
    case WM_LBUTTONUP:
        isDrawing = false;
        endPt.x = LOWORD(lParam);
        endPt.y = HIWORD(lParam);

        Shape newShape;
        newShape.type = shapeType;
        newShape.rect = { startPt.x, startPt.y, endPt.x, endPt.y };
        newShape.color = lineColor;
        shapes.push_back(newShape);

        InvalidateRect(hwnd, NULL, TRUE);
        break;
    case WM_RBUTTONDOWN: {
        POINT pt;
        GetCursorPos(&pt);
        TrackPopupMenu(hPopupMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
        break;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Vẽ các hình
        DrawShapes(hdc);

        // Lấy kích thước vùng client
        GetClientRect(hwnd, &clientRect);

        // Tính toán vị trí cho đồng hồ ở góc dưới bên phải
        int clockWidth = 150;
        int clockHeight = 30;
        int clockX = clientRect.right - clockWidth - 10;  // Cách lề phải 10px
        int clockY = clientRect.bottom - clockHeight - 10; // Cách lề dưới 10px

        // Tạo vùng đồng hồ
        RECT clockRect = { clockX, clockY, clockX + clockWidth, clockY + clockHeight };

        // Vẽ nền cho đồng hồ
        FillRect(hdc, &clockRect, (HBRUSH)(COLOR_BTNFACE + 1));

        // Hiển thị đồng hồ
        std::wstringstream ss;
        ss << elapsedMinutes << L":" << (elapsedSeconds < 10 ? L"0" : L"") << elapsedSeconds;
        std::wstring timeText = ss.str();

        SetTextColor(hdc, RGB(0, 0, 0));
        SetBkMode(hdc, TRANSPARENT);
        HFONT hFont = CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
        SelectObject(hdc, hFont);

        // Canh giữa văn bản trong vùng đồng hồ
        RECT textRect = clockRect;
        DrawText(hdc, timeText.c_str(), timeText.length(), &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        DeleteObject(hFont);

        EndPaint(hwnd, &ps);
        break;
    }
    case WM_TIMER:
        UpdateTimer(hwnd);
        break;
    case WM_SIZE:
        // Yêu cầu vẽ lại toàn bộ cửa sổ khi thay đổi kích thước
        InvalidateRect(hwnd, NULL, TRUE);
        break;
    case WM_CLOSE:
        if (MessageBox(hwnd, TEXT("Bạn có muốn thoát không?"), TEXT("Xác nhận"), MB_YESNO | MB_ICONQUESTION) == IDYES) {
            DestroyWindow(hwnd);
        }
        return 0;
    case WM_DESTROY:
        KillTimer(hwnd, timerID);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}