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

	numberAdapters = 0;
	ADL_Adapter_NumberOfAdapters_Get(&numberAdapters);

	// Get the AdapterInfo structure for all adapters in the system.
	if (numberAdapters > 0) {
		SIZE_T size = sizeof(AdapterInfo) * numberAdapters;
		adapterInfo = static_cast<LPAdapterInfo>(malloc(size));
		if (adapterInfo != 0) {
			memset(adapterInfo, '\0', size);
			ADL_Adapter_AdapterInfo_Get(adapterInfo, static_cast<int>(size));
		}
	} else {
		throw std::runtime_error("No adapters found");
	}

	// Repeat for all available adapters in the system.
	for (int i = 0; i < numberAdapters; i++) {
		int adapterIndex = adapterInfo[i].iAdapterIndex;
		LPADLDisplayInfo adlDisplayInfo = nullptr;
		int numDisplays;

		if (ADL_OK != ADL_Display_DisplayInfo_Get(adapterIndex, &numDisplays, &adlDisplayInfo, 0)) {
			continue;
		}

		for (int j = 0; j < numDisplays; j++) {
			// For each display, check its status.
			// Use the display only if it's connected AND mapped (iDisplayInfoValue: bit 0 and 1).
			if ((ADL_DISPLAY_DISPLAYINFO_DISPLAYCONNECTED | ADL_DISPLAY_DISPLAYINFO_DISPLAYMAPPED) !=
				(ADL_DISPLAY_DISPLAYINFO_DISPLAYCONNECTED | ADL_DISPLAY_DISPLAYINFO_DISPLAYMAPPED &
					adlDisplayInfo[j].iDisplayInfoValue)) {
				continue;
			}

			// Check if the display is mapped to the current adapter.
			if (adapterIndex != adlDisplayInfo[j].displayID.iDisplayLogicalAdapterIndex) {
				continue;
			}
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			DisplayAdapterInfo display{};
			display.adapterIndex = adapterIndex;
			display.displayIndex = adlDisplayInfo[j].displayID.iDisplayLogicalIndex;

			// Check if display supports changing contrast, brightness, hue and saturation.
			if (HasSupportForAllFeatures(display)) {
				displays.insert(make_pair(L"\\\\.\\DISPLAY" + std::to_wstring(display.displayIndex), display));
			}
		}
		ADL_Main_Memory_Free(reinterpret_cast<void**>(&adlDisplayInfo));
	}
}

ADL::~ADL() {
	ADL_Main_Memory_Free(reinterpret_cast<void**>(&adapterInfo));
	ADL_Main_Control_Destroy();
	FreeLibrary(hDll);
}

std::vector<std::wstring> ADL::GetDisplayNames() const {
	std::vector<std::wstring> names;

	for (auto const& display : displays) {
		names.push_back(display.first);
	}
	return names;
}

DriverInterface::FeatureValues ADL::GetDigitalVibranceInfo(const std::wstring displayName) const {
	throw std::runtime_error("Unsupported operation");
}

void ADL::SetDigitalVibrance(const std::wstring displayName, const int newValue) const {
	throw std::runtime_error("Unsupported operation");
}

ADL::FeatureValues ADL::GetSaturationInfo(const std::wstring displayName) const {
	return GetFeatureValues(displayName, ADL_DISPLAY_COLOR_SATURATION);
}

void ADL::SetSaturation(const std::wstring displayName, const int newValue) const {
	SetFeatureValues(displayName, ADL_DISPLAY_COLOR_SATURATION, newValue);
}

DriverInterface::FeatureValues ADL::GetContrastInfo(const std::wstring displayName) const {
	return GetFeatureValues(displayName, ADL_DISPLAY_COLOR_CONTRAST);
}

void ADL::SetContrast(const std::wstring displayName, const int newValue) const {
	SetFeatureValues(displayName, ADL_DISPLAY_COLOR_CONTRAST, newValue);
}

DriverInterface::FeatureValues ADL::GetBrightnessInfo(const std::wstring displayName) const {
	return GetFeatureValues(displayName, ADL_DISPLAY_COLOR_BRIGHTNESS);
}

void ADL::SetBrightness(const std::wstring displayName, const int newValue) const {
	SetFeatureValues(displayName, ADL_DISPLAY_COLOR_BRIGHTNESS, newValue);
}

DriverInterface::FeatureValues ADL::GetHueInfo(const std::wstring displayName) const {
	return GetFeatureValues(displayName, ADL_DISPLAY_COLOR_HUE);
}

void ADL::SetHue(const std::wstring displayName, const int newValue) const {
	SetFeatureValues(displayName, ADL_DISPLAY_COLOR_HUE, newValue);
}

DriverInterface::FeatureValues ADL::GetFeatureValues(const std::wstring& displayName, const int feature) const {
	int currentValue, defaultValue, minValue, maxValue;
	DisplayAdapterInfo displayInfo = displays.at(displayName);

	ADL_Display_Color_Get(displayInfo.adapterIndex, displayInfo.displayIndex, feature,
		&currentValue, &defaultValue, &minValue, &maxValue, nullptr);

	return {currentValue, defaultValue, minValue, maxValue};
}

bool ADL::HasSupportForAllFeatures(const DisplayAdapterInfo displayInfo) const {
	int colorCaps, validBits;
	ADL_Display_ColorCaps_Get(displayInfo.adapterIndex, displayInfo.displayIndex, &colorCaps, &validBits);

	// Use only the valid bits from colorCaps.
	return colorCaps & validBits & (ADL_DISPLAY_COLOR_CONTRAST | ADL_DISPLAY_COLOR_BRIGHTNESS
		| ADL_DISPLAY_COLOR_HUE | ADL_DISPLAY_COLOR_SATURATION);
}

void ADL::SetFeatureValues(const std::wstring& displayName, const int feature, const int newValue) const {
	DisplayAdapterInfo displayInfo = displays.at(displayName);
	ADL_Display_Color_Set(displayInfo.adapterIndex, displayInfo.displayIndex, feature, newValue);
}

void* __stdcall ADL::ADL_Main_Memory_Alloc(const int iSize) {
	return malloc(iSize);
}

void __stdcall ADL::ADL_Main_Memory_Free(void** lpBuffer) {
	if (nullptr != *lpBuffer) {
		free(*lpBuffer);
		*lpBuffer = nullptr;
	}
}
