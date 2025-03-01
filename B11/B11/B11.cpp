#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// ID menu
#define ID_HINH_CHUNHAT  101
#define ID_HINH_ELLIPSE  102
#define ID_HINH_TAMGIAC  103
#define ID_HINH_HINHTHOI 104
#define ID_MAUNEN_VANG   201
#define ID_MAUNEN_XANH   202
#define ID_MAUVIEN_DO     301
#define ID_MAUVIEN_XANHDUONG 302

// Lưu trạng thái lựa chọn
int shapeType = ID_HINH_ELLIPSE;   // Mặc định Ellipse
COLORREF fillColor = RGB(255, 255, 0); // Mặc định Vàng
COLORREF borderColor = RGB(255, 0, 0); // Mặc định Đỏ

// Biến điều khiển vẽ
POINT startPt, endPt;
bool isDrawing = false;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = TEXT("DrawingApp");
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);

    RegisterClass(&wc);

    HWND hwnd = CreateWindow(wc.lpszClassName, TEXT("Vẽ hình bằng chuột"),
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 700, 500,
        nullptr, nullptr, hInstance, nullptr);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

void DrawShape(HDC hdc, int shape, RECT rect) {
    HPEN hPen = CreatePen(PS_SOLID, 2, borderColor);
    HBRUSH hBrush = CreateSolidBrush(fillColor);
    SelectObject(hdc, hPen);
    SelectObject(hdc, hBrush);

    switch (shape) {
    case ID_HINH_CHUNHAT:
        Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
        break;
    case ID_HINH_ELLIPSE:
        Ellipse(hdc, rect.left, rect.top, rect.right, rect.bottom);
        break;
    case ID_HINH_TAMGIAC: {
        POINT pts[3] = {
            { (rect.left + rect.right) / 2, rect.top },
            { rect.left, rect.bottom },
            { rect.right, rect.bottom }
        };
        Polygon(hdc, pts, 3);
        break;
    }
    case ID_HINH_HINHTHOI: {
        POINT pts[4] = {
            { (rect.left + rect.right) / 2, rect.top },
            { rect.right, (rect.top + rect.bottom) / 2 },
            { (rect.left + rect.right) / 2, rect.bottom },
            { rect.left, (rect.top + rect.bottom) / 2 }
        };
        Polygon(hdc, pts, 4);
        break;
    }
    }

    DeleteObject(hPen);
    DeleteObject(hBrush);
}

void UpdateMenuCheck(HMENU hMenu, int itemId, int menuGroupStart, int menuGroupEnd) {
    for (int i = menuGroupStart; i <= menuGroupEnd; i++) {
        CheckMenuItem(hMenu, i, MF_UNCHECKED);
    }
    CheckMenuItem(hMenu, itemId, MF_CHECKED);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HMENU hMenu, hSubMenuHinh, hSubMenuNen, hSubMenuVien;

    switch (msg) {
    case WM_CREATE: {
        hMenu = CreateMenu();

        hSubMenuHinh = CreatePopupMenu();
        AppendMenu(hSubMenuHinh, MF_STRING, ID_HINH_CHUNHAT, TEXT("Chữ nhật"));
        AppendMenu(hSubMenuHinh, MF_STRING, ID_HINH_ELLIPSE, TEXT("Ellipse"));
        AppendMenu(hSubMenuHinh, MF_STRING, ID_HINH_TAMGIAC, TEXT("Tam giác cân"));
        AppendMenu(hSubMenuHinh, MF_STRING, ID_HINH_HINHTHOI, TEXT("Hình thoi"));

        hSubMenuNen = CreatePopupMenu();
        AppendMenu(hSubMenuNen, MF_STRING, ID_MAUNEN_VANG, TEXT("Vàng"));
        AppendMenu(hSubMenuNen, MF_STRING, ID_MAUNEN_XANH, TEXT("Xanh lá cây"));

        hSubMenuVien = CreatePopupMenu();
        AppendMenu(hSubMenuVien, MF_STRING, ID_MAUVIEN_DO, TEXT("Đỏ"));
        AppendMenu(hSubMenuVien, MF_STRING, ID_MAUVIEN_XANHDUONG, TEXT("Xanh dương"));

        AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hSubMenuHinh, TEXT("Hình"));
        AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hSubMenuNen, TEXT("Màu nền của hình"));
        AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hSubMenuVien, TEXT("Màu viền"));

        SetMenu(hwnd, hMenu);
        break;
    }
    case WM_COMMAND: {
        int menuId = LOWORD(wParam);
        HMENU hMenu = GetMenu(hwnd);

        if (menuId >= ID_HINH_CHUNHAT && menuId <= ID_HINH_HINHTHOI) {
            shapeType = menuId;
            UpdateMenuCheck(hMenu, menuId, ID_HINH_CHUNHAT, ID_HINH_HINHTHOI);
        }
        else if (menuId == ID_MAUNEN_VANG || menuId == ID_MAUNEN_XANH) {
            fillColor = (menuId == ID_MAUNEN_VANG) ? RGB(255, 255, 0) : RGB(0, 255, 0);
            UpdateMenuCheck(hMenu, menuId, ID_MAUNEN_VANG, ID_MAUNEN_XANH);
        }
        else if (menuId == ID_MAUVIEN_DO || menuId == ID_MAUVIEN_XANHDUONG) {
            borderColor = (menuId == ID_MAUVIEN_DO) ? RGB(255, 0, 0) : RGB(0, 0, 255);
            UpdateMenuCheck(hMenu, menuId, ID_MAUVIEN_DO, ID_MAUVIEN_XANHDUONG);
        }
        InvalidateRect(hwnd, nullptr, TRUE);
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
        InvalidateRect(hwnd, nullptr, TRUE);
        break;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        if (!isDrawing) {
            RECT rect = { startPt.x, startPt.y, endPt.x, endPt.y };
            DrawShape(hdc, shapeType, rect);
        }
        EndPaint(hwnd, &ps);
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}
