#pragma once

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include "DriverInterface.h"

using GET_INFO = DriverInterface::FeatureValues(DriverInterface::*)(const std::wstring) const;
using SET_VALUE = void(DriverInterface::*)(const std::wstring, const int) const;

struct Feature {
	LPCWSTR name;
	HWND trackbar;
	HWND label;
	GET_INFO getInfoFunction;
	SET_VALUE setValueFunction;
};

// Global variables
std::map<HWND, Feature> features;
std::unique_ptr<DriverInterface> driverInterface;
std::wstring selectedDisplay;

LPCWSTR szWindowClass = L"VibranceUtility";
HINSTANCE hInst;
HWND combobox;

// Functions
LRESULT __stdcall WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

std::unique_ptr<DriverInterface> CreateDriverInterface(HWND hWnd);

void CreateControls(HWND hWnd);
HWND CreateComboBox(HWND hWnd, LPCWSTR name, int yOffset);
void CreateFeatureGroup(HWND hWnd, LPCWSTR name, int yOffset, GET_INFO getter, SET_VALUE setter);

void UpdateSelectedDisplay(HWND hWndCombobox);
void UpdateFeatureValues(HWND hWndTrackbar);
void UpdateTrackBar(HWND hWndTrackbar, DriverInterface::FeatureValues info);
void UpdateLabel(HWND hWndLabel, DriverInterface::FeatureValues info);

BOOL __stdcall SetFont(HWND hWnd, LPARAM font);