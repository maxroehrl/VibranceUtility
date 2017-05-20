#include "stdafx.h"
#include "ADL.h"

ADL::ADL() {
	// Try loading the 64 bit library.
	hDll = LoadLibraryA("atiadlxx.dll");

	// A 32 bit calling application on 64 bit OS will fail to LoadLibrary.
	// Try to load the 32 bit library (atiadlxy.dll) instead.
	if (hDll == nullptr) {
		hDll = LoadLibraryA("atiadlxy.dll");
	}

	// Try loading the function pointers.
	ADL_Main_Control_Create          = reinterpret_cast<ADL_MAIN_CONTROL_CREATE>(         GetProcAddress(hDll, "ADL_Main_Control_Create"));
	ADL_Main_Control_Destroy         = reinterpret_cast<ADL_MAIN_CONTROL_DESTROY>(        GetProcAddress(hDll, "ADL_Main_Control_Destroy"));
	ADL_Adapter_NumberOfAdapters_Get = reinterpret_cast<ADL_ADAPTER_NUMBEROFADAPTERS_GET>(GetProcAddress(hDll, "ADL_Adapter_NumberOfAdapters_Get"));
	ADL_Adapter_AdapterInfo_Get      = reinterpret_cast<ADL_ADAPTER_ADAPTERINFO_GET>(     GetProcAddress(hDll, "ADL_Adapter_AdapterInfo_Get"));
	ADL_Display_DisplayInfo_Get      = reinterpret_cast<ADL_DISPLAY_DISPLAYINFO_GET>(     GetProcAddress(hDll, "ADL_Display_DisplayInfo_Get"));
	ADL_Display_ColorCaps_Get        = reinterpret_cast<ADL_DISPLAY_COLORCAPS_GET>(       GetProcAddress(hDll, "ADL_Display_ColorCaps_Get"));
	ADL_Display_Color_Get            = reinterpret_cast<ADL_DISPLAY_COLOR_GET>(           GetProcAddress(hDll, "ADL_Display_Color_Get"));
	ADL_Display_Color_Set            = reinterpret_cast<ADL_DISPLAY_COLOR_SET>(           GetProcAddress(hDll, "ADL_Display_Color_Set"));

	// Initialize ADL. The second parameter is 1, which means:
	// retrieve adapter information only for adapters that are
	// physically present and enabled in the system.
	ADL_Main_Control_Create(ADL_Main_Memory_Alloc, 1);

	ADL_Adapter_NumberOfAdapters_Get(&numberAdapters);

	// Get the AdapterInfo structure for all adapters in the system.
	if (numberAdapters > 0) {
		size_t size = sizeof(AdapterInfo) * numberAdapters;
		adapterInfo = static_cast<LPAdapterInfo>(malloc(size));
		memset(adapterInfo, '\0', size);
		ADL_Adapter_AdapterInfo_Get(adapterInfo, static_cast<int>(size));
	}

	// Repeat for all available adapters in the system.
	for (int i = 0; i < numberAdapters; i++) {
		int iAdapterIndex = adapterInfo[i].iAdapterIndex;
		LPADLDisplayInfo lpAdlDisplayInfo = nullptr;
		int iNumDisplays;

		if (ADL_OK != ADL_Display_DisplayInfo_Get(iAdapterIndex, &iNumDisplays, &lpAdlDisplayInfo, 0)) {
			continue;
		}

		for (int j = 0; j < iNumDisplays; j++) {
			// For each display, check its status.
			// Use the display only if it's connected AND mapped (iDisplayInfoValue: bit 0 and 1).
			if ((ADL_DISPLAY_DISPLAYINFO_DISPLAYCONNECTED | ADL_DISPLAY_DISPLAYINFO_DISPLAYMAPPED) !=
				(ADL_DISPLAY_DISPLAYINFO_DISPLAYCONNECTED | ADL_DISPLAY_DISPLAYINFO_DISPLAYMAPPED &
					lpAdlDisplayInfo[j].iDisplayInfoValue)) {
				continue;
			}

			// Check if the display is mapped to the current adapter.
			if (iAdapterIndex != lpAdlDisplayInfo[j].displayID.iDisplayLogicalAdapterIndex) {
				continue;
			}
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			DisplayAdapterInfo display;
			display.adapterIndex = iAdapterIndex;
			display.displayIndex = lpAdlDisplayInfo[j].displayID.iDisplayLogicalIndex;
			display.name = converter.from_bytes(lpAdlDisplayInfo[j].strDisplayName);

			// Check if display supports changing the saturation.
			if (IsFeatureSupported(display, ADL_DISPLAY_COLOR_SATURATION)) {
				displays.push_back(display);
			}
		}
		ADL_Main_Memory_Free(reinterpret_cast<void**>(&lpAdlDisplayInfo));
	}
}

ADL::~ADL() {
	ADL_Main_Memory_Free(reinterpret_cast<void**>(&adapterInfo));
	ADL_Main_Control_Destroy();
	FreeLibrary(hDll);
}

std::vector<std::wstring> ADL::GetDisplayNames() const {
	std::vector<std::wstring> names{};

	for (DisplayAdapterInfo display : displays) {
		names.push_back(display.name);
	}
	return names;
}

ADL::FeatureValues ADL::GetSaturationInfo(const std::wstring displayName) const {
	int currentValue, defaultValue, minValue, maxValue;
	DisplayAdapterInfo displayInfo = GetDisplayInfo(displayName);

	ADL_Display_Color_Get(displayInfo.adapterIndex, displayInfo.displayIndex, ADL_DISPLAY_COLOR_SATURATION,
		&currentValue, &defaultValue, &minValue, &maxValue, nullptr);

	return {currentValue, defaultValue, minValue, maxValue};
}

void ADL::SetSaturation(const std::wstring displayName, const int newValue) const {
	DisplayAdapterInfo displayInfo = GetDisplayInfo(displayName);
	ADL_Display_Color_Set(displayInfo.adapterIndex, displayInfo.displayIndex,
		ADL_DISPLAY_COLOR_SATURATION, newValue);
}

ADL::DisplayAdapterInfo ADL::GetDisplayInfo(const std::wstring displayName) const {
	for(const auto& info : displays) {
		if (info.name == displayName) {
			return info;
		}
	}
	return {};
}

bool ADL::IsFeatureSupported(const DisplayAdapterInfo display, const int feature) const {
	int iColorCaps, iValidBits;
	ADL_Display_ColorCaps_Get(display.adapterIndex, display.displayIndex, &iColorCaps, &iValidBits);

	// Use only the valid bits from iColorCaps.
	iColorCaps &= iValidBits;

	// Check if the display supports this particular capability.
	return feature & iColorCaps;
}

void* __stdcall ADL::ADL_Main_Memory_Alloc(int iSize) {
	return malloc(iSize);
}

void __stdcall ADL::ADL_Main_Memory_Free(void** lpBuffer) {
	if (nullptr != *lpBuffer) {
		free(*lpBuffer);
		*lpBuffer = nullptr;
	}
}
