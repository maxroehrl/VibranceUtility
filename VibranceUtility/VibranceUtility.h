#pragma once

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include "resource.h"
#include "DriverInterface.h"
#include "ADL.h"
#include "NvApi.h"

// Global variables
std::unique_ptr<DriverInterface> driverInterface;
std::wstring selectedDisplay;

LPCWSTR szWindowClass = L"VibranceUtility";
HINSTANCE hInst;

HWND combobox;
HWND trackbar;
HWND label;

// Functions
LRESULT __stdcall WndProc(HWND, UINT, WPARAM, LPARAM);

std::unique_ptr<DriverInterface> CreateDriverInterface(HWND);

HWND CreateComboBox(HWND);
HWND CreateLabel(HWND);
HWND CreateTrackBar(HWND);

void UpdateSelectedDisplay(HWND);
void UpdateSaturation(HWND);

void UpdateTrackBar(HWND);
void UpdateLabel(HWND);

BOOL __stdcall SetFont(HWND, LPARAM);