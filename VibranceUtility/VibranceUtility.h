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
bool isAMD;

LPCWSTR szWindowClass = L"VibranceUtility";
HINSTANCE hInst;
HWND combobox;

// Functions
LRESULT __stdcall WndProc(HWND, UINT, WPARAM, LPARAM);

std::unique_ptr<DriverInterface> CreateDriverInterface(HWND);

void CreateControls(HWND);
HWND CreateComboBox(HWND);
Feature CreateFeatureGroup(HWND, LPCWSTR, int, GET_INFO, SET_VALUE);

void UpdateSelectedDisplay(HWND);
void UpdateFeatureValues(HWND);
void UpdateTrackBar(HWND, DriverInterface::FeatureValues);
void UpdateLabel(HWND, DriverInterface::FeatureValues);

BOOL __stdcall SetFont(HWND, LPARAM);