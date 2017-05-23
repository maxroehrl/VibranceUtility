#include "stdafx.h"
#include "VibranceUtility.h"

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	HBRUSH brush = CreateSolidBrush(RGB(240, 240, 240));
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
	wc.hbrBackground = brush;
	wc.lpszClassName = szWindowClass;
	wc.lpszMenuName = nullptr;
	RegisterClass(&wc);

	hInst = hInstance;

	HWND hWnd = CreateWindow(szWindowClass, L"Vibrance Utility",
		WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, 270, 170,
		nullptr, nullptr, hInstance, nullptr);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	DeleteObject(brush);
	return static_cast<int>(msg.wParam);
}

LRESULT __stdcall WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CREATE:
		driverInterface = CreateDriverInterface(hWnd);

		// If the driver interface was created the controls can be added.
		if (driverInterface != nullptr) {
			combobox = CreateComboBox(hWnd);
			label = CreateLabel(hWnd);
			trackbar = CreateTrackBar(hWnd);

			// Set the font of all controls.
			EnumChildWindows(hWnd, reinterpret_cast<WNDENUMPROC>(SetFont),
				reinterpret_cast<LPARAM>(GetStockObject(DEFAULT_GUI_FONT)));
		}
		break;
	case WM_COMMAND:
		if (reinterpret_cast<HWND>(lParam) == combobox && HIWORD(wParam) == CBN_SELCHANGE) {
			UpdateSelectedDisplay(combobox);
		}
		break;
	case WM_HSCROLL:
		if (reinterpret_cast<HWND>(lParam) == trackbar) {
			UpdateSaturation(trackbar);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

std::unique_ptr<DriverInterface> CreateDriverInterface(HWND hWnd) {
	bool isNvidiaDriverPresent = std::experimental::filesystem::exists("C:/Windows/System32/nvapi64.dll") ||
		std::experimental::filesystem::exists("C:/Windows/System32/nvapi.dll");
	bool isAMDDriverPresent = std::experimental::filesystem::exists("C:/Windows/System32/atiadlxx.dll") ||
		std::experimental::filesystem::exists("C:/Windows/System32/atiadlxy.dll");

	if (isAMDDriverPresent && isNvidiaDriverPresent) {
		MessageBox(hWnd, L"Both AMD and Nvidia drivers were found!", L"Ambiguous drivers", MB_OK);
		PostMessage(hWnd, WM_CLOSE, 0, 0);
	} else if (isNvidiaDriverPresent) {
		return std::make_unique<NvApi>();
	} else if (isAMDDriverPresent) {
		return std::make_unique<ADL>();
	} else {
		MessageBox(hWnd, L"No AMD or Nvidia driver was found!", L"No driver", MB_OK);
		PostMessage(hWnd, WM_CLOSE, 0, 0);
	}
	return nullptr;
}

HWND CreateComboBox(HWND hWnd) {
	HWND hWndComboBox = CreateWindow(WC_COMBOBOX, L"Monitor combobox",
		CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_VISIBLE,
		10, 10, 235, 50,
		hWnd, nullptr, hInst, nullptr);
	std::vector<std::wstring> displays = driverInterface->GetDisplayNames();

	if (displays.empty()) {
		MessageBox(hWnd, L"No display supports changing the digitial vibrance!", L"No supported displays", MB_OK);
		PostMessage(hWnd, WM_CLOSE, 0, 0);
		return nullptr;
	}

	for (auto const& display : displays) {
		if (selectedDisplay.empty()) {
			selectedDisplay = display;
		}
		SendMessage(hWndComboBox, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(display.c_str()));
	}
	// Select first monitor.
	SendMessage(hWndComboBox, CB_SETCURSEL, 0, 0);

	// Hide the dashed focus outline.
	SendMessage(hWndComboBox, WM_UPDATEUISTATE, MAKEWPARAM(UIS_SET, UISF_HIDEFOCUS), 0);
	return hWndComboBox;
}

HWND CreateLabel(HWND hWnd) {
	HWND hWndLabel = CreateWindow(WC_STATIC, L"", WS_CHILD | WS_VISIBLE,
		10, 50, 235, 30,
		hWnd, nullptr, hInst, nullptr);
	UpdateLabel(hWndLabel);
	return hWndLabel;
}

HWND CreateTrackBar(HWND hWnd) {
	HWND hWndTrackbar = CreateWindow(TRACKBAR_CLASS, L"Saturation trackbar",
		WS_CHILD | WS_VISIBLE,
		10, 90, 235, 30,
		hWnd, nullptr, hInst, nullptr
	);
	// Hide the dashed focus outline.
	SendMessage(hWndTrackbar, WM_UPDATEUISTATE, MAKEWPARAM(UIS_SET, UISF_HIDEFOCUS), 0);

	UpdateTrackBar(hWndTrackbar);
	return hWndTrackbar;
}

void UpdateSelectedDisplay(HWND hWndCombobox) {
	// Get the index of the selected display.
	LRESULT selectedIndex = SendMessage(hWndCombobox, CB_GETCURSEL, 0, 0);

	// Save the name of the selected display.
	TCHAR selectedName[256];
	SendMessage(hWndCombobox, CB_GETLBTEXT, selectedIndex, reinterpret_cast<LPARAM>(selectedName));
	selectedDisplay = selectedName;
	UpdateLabel(label);
	UpdateTrackBar(trackbar);
}

void UpdateSaturation(HWND hWndTrackbar) {
	// Set saturation and update the level with the new one.
	int newValue = static_cast<int>(SendMessage(hWndTrackbar, TBM_GETPOS, 0, 0));
	driverInterface->SetSaturation(selectedDisplay, newValue);
	UpdateLabel(label);
}

void UpdateTrackBar(HWND hwndTrack) {
	DriverInterface::FeatureValues info = driverInterface->GetSaturationInfo(selectedDisplay);

	// Update the min, max and current saturation.
	SendMessage(hwndTrack, TBM_SETRANGE, TRUE, MAKELONG(info.minValue, info.maxValue));
	SendMessage(hwndTrack, TBM_SETPOS, TRUE, info.currentValue);
}

void UpdateLabel(HWND hWndLabel) {
	DriverInterface::FeatureValues info = driverInterface->GetSaturationInfo(selectedDisplay);

	// Update the text for default and max saturation.
	wchar_t buf[50];
	wsprintf(buf, L"Default Saturation: %ld%\nCurrent Saturation: %ld%", info.defaultValue, info.currentValue);
	SetWindowText(hWndLabel, buf);
}

BOOL __stdcall SetFont(HWND hWnd, LPARAM font) {
	SendMessage(hWnd, WM_SETFONT, font, TRUE);
	return TRUE;
}
