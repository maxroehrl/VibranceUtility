#include "stdafx.h"
#include "VibranceUtility.h"
#include "ADL.h"
#include "NvApi.h"
#include "resource.h"

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
		CW_USEDEFAULT, CW_USEDEFAULT, 270, 430,
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
			CreateControls(hWnd);
		}
		break;
	case WM_COMMAND:
		if (reinterpret_cast<HWND>(lParam) == combobox && HIWORD(wParam) == CBN_SELCHANGE) {
			UpdateSelectedDisplay(combobox);
		}
		break;
	case WM_HSCROLL:
		UpdateFeatureValues(reinterpret_cast<HWND>(lParam));
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

	// This flag determines the available GUI controls.
	isAMD = isAMDDriverPresent;

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

void CreateControls(HWND hWnd) {
	int yOffset = 10;
	combobox = CreateComboBox(hWnd, L"Monitor Combobox", yOffset);

	// If the combobox is null no supported displays were found.
	if (combobox == nullptr) {
		return;
	}

	if (!isAMD) {
		CreateFeatureGroup(hWnd, L"Digital Vibrance", yOffset += 35,
			&DriverInterface::GetDigitalVibranceInfo, &DriverInterface::SetDigitalVibrance);

		// Make the window smaller.
		SetWindowPos(hWnd, nullptr, 0, 0, 270, 172, SWP_NOMOVE);
	} else {
		CreateFeatureGroup(hWnd, L"Saturation", yOffset += 35,
			&DriverInterface::GetSaturationInfo, &DriverInterface::SetSaturation);
		CreateFeatureGroup(hWnd, L"Contrast", yOffset += 85,
			&DriverInterface::GetContrastInfo, &DriverInterface::SetContrast);
		CreateFeatureGroup(hWnd, L"Brightness", yOffset += 85,
			&DriverInterface::GetBrightnessInfo, &DriverInterface::SetBrightness);
		CreateFeatureGroup(hWnd, L"Hue", yOffset += 85,
			&DriverInterface::GetHueInfo, &DriverInterface::SetHue);
	}

	// Set the selected display and update all controls with the correct values.
	UpdateSelectedDisplay(combobox);

	// Set the font of all controls.
	EnumChildWindows(hWnd, reinterpret_cast<WNDENUMPROC>(SetFont),
		reinterpret_cast<LPARAM>(GetStockObject(DEFAULT_GUI_FONT)));
}

HWND CreateComboBox(HWND hWnd, LPCWSTR name, int yOffset) {
	HWND hWndComboBox = CreateWindow(WC_COMBOBOX, name,
		WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_HASSTRINGS,
		10, yOffset, 235, 50,
		hWnd, nullptr, hInst, nullptr);
	std::vector<std::wstring> displays = driverInterface->GetDisplayNames();

	if (displays.empty()) {
		MessageBox(hWnd, L"No display supports changing the digitial vibrance!", L"No supported displays", MB_OK);
		PostMessage(hWnd, WM_CLOSE, 0, 0);
		return nullptr;
	}

	// Add all display names to the combobox.
	for (auto const& display : displays) {
		SendMessage(hWndComboBox, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(display.c_str()));
	}

	// Select first monitor.
	SendMessage(hWndComboBox, CB_SETCURSEL, 0, 0);

	// Hide the dashed focus outline.
	SendMessage(hWndComboBox, WM_UPDATEUISTATE, MAKEWPARAM(UIS_SET, UISF_HIDEFOCUS), 0);

	return hWndComboBox;
}

void CreateFeatureGroup(HWND hWnd, LPCWSTR name, int yOffset, GET_INFO getter, SET_VALUE setter) {
	// Create the groupbox.
	CreateWindow(WC_BUTTON, name,
		WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
		5, yOffset, 245, 85,
		hWnd, nullptr, hInst, nullptr);

	// Create the label.
	HWND hWndLabel = CreateWindow(WC_STATIC, name,
		WS_CHILD | WS_VISIBLE,
		10, yOffset + 20, 235, 30,
		hWnd, nullptr, hInst, nullptr);

	// Create the trackbar.
	HWND hWndTrackbar = CreateWindow(TRACKBAR_CLASS, name,
		WS_CHILD | WS_VISIBLE,
		10, yOffset + 50, 235, 30,
		hWnd, nullptr, hInst, nullptr);

	// Hide the dashed focus outline.
	SendMessage(hWndTrackbar, WM_UPDATEUISTATE, MAKEWPARAM(UIS_SET, UISF_HIDEFOCUS), 0);

	Feature feature = {name, hWndTrackbar, hWndLabel, getter, setter};
	features.insert(std::make_pair(hWndTrackbar, feature));
}

void UpdateSelectedDisplay(HWND hWndCombobox) {
	// Get the index of the selected display.
	LRESULT selectedIndex = SendMessage(hWndCombobox, CB_GETCURSEL, 0, 0);

	// Save the name of the selected display.
	TCHAR selectedName[MAX_PATH];
	SendMessage(hWndCombobox, CB_GETLBTEXT, selectedIndex, reinterpret_cast<LPARAM>(selectedName));
	selectedDisplay = selectedName;

	// Update the labels and trackbars.
	for (auto const& feature : features) {
		auto info = invoke(feature.second.getInfoFunction, driverInterface, selectedDisplay);
		UpdateLabel(feature.second.label, info);
		UpdateTrackBar(feature.second.trackbar, info);
	}
}

void UpdateFeatureValues(HWND hWndTrackbar) {
	// Set the selected value of the trackbar and update the label accordingly.
	Feature feature = features.at(hWndTrackbar);
	int newValue = static_cast<int>(SendMessage(feature.trackbar, TBM_GETPOS, 0, 0));
	invoke(feature.setValueFunction, driverInterface, selectedDisplay, newValue);
	UpdateLabel(feature.label, invoke(feature.getInfoFunction, driverInterface, selectedDisplay));
}

void UpdateTrackBar(HWND hwndTrackbar, DriverInterface::FeatureValues info) {
	// Update the min, max and current saturation.
	SendMessage(hwndTrackbar, TBM_SETRANGE, TRUE, MAKELONG(info.minValue, info.maxValue));
	SendMessage(hwndTrackbar, TBM_SETPOS, TRUE, info.currentValue);
}

void UpdateLabel(HWND hWndLabel, DriverInterface::FeatureValues info) {
	// Update the text with default and current values.
	TCHAR labelText[MAX_PATH];
	StringCbPrintf(labelText, _countof(labelText), L"Default: %ld%\nCurrent: %ld%",
		info.defaultValue, info.currentValue);
	SetWindowText(hWndLabel, labelText);
}

BOOL __stdcall SetFont(HWND hWnd, LPARAM font) {
	SendMessage(hWnd, WM_SETFONT, font, TRUE);
	return TRUE;
}
