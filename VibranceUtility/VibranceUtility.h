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

std::unique_ptr<DriverInterface> __stdcall CreateDriverInterface(HWND);

HWND __stdcall CreateComboBox(HWND);
HWND __stdcall CreateLabel(HWND);
HWND __stdcall CreateTrackBar(HWND);

void __stdcall UpdateSelectedDisplay(HWND);
void __stdcall UpdateSaturation(HWND);

void __stdcall UpdateTrackBar(HWND);
void __stdcall UpdateLabel(HWND);

bool __stdcall SetFont(HWND, WPARAM);