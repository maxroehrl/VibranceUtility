#pragma once
#include "DriverInterface.h"
#include "adl/adl_sdk.h"

class ADL : public DriverInterface {
public:
	ADL();
	~ADL() override;
	std::vector<std::wstring> GetDisplayNames() const override;

	FeatureValues GetDigitalVibranceInfo(const std::wstring displayName) const override;
	void SetDigitalVibrance(const std::wstring displayName, const int newValue) const override;

	FeatureValues GetSaturationInfo(const std::wstring displayName) const override;
	void SetSaturation(const std::wstring displayName, const int newValue) const override;

	FeatureValues GetContrastInfo(const std::wstring displayName) const override;
	void SetContrast(const std::wstring displayName, const int newValue) const override;

	FeatureValues GetBrightnessInfo(const std::wstring displayName) const override;
	void SetBrightness(const std::wstring displayName, const int newValue) const override;

	FeatureValues GetHueInfo(const std::wstring displayName) const override;
	void SetHue(const std::wstring displayName, const int newValue) const override;

private:
	struct DisplayAdapterInfo {
		int adapterIndex;
		int displayIndex;
		std::wstring name;
	};

	// Definitions of the used function pointers
	using ADL_MAIN_CONTROL_CREATE          = int(*)(ADL_MAIN_MALLOC_CALLBACK, int);
	using ADL_MAIN_CONTROL_DESTROY         = int(*)();
	using ADL_ADAPTER_NUMBEROFADAPTERS_GET = int(*)(int*);
	using ADL_ADAPTER_ADAPTERINFO_GET      = int(*)(LPAdapterInfo, int);
	using ADL_DISPLAY_COLORCAPS_GET        = int(*)(int, int, int*, int*);
	using ADL_DISPLAY_COLOR_GET            = int(*)(int, int, int, int*, int*, int*, int*, int*);
	using ADL_DISPLAY_COLOR_SET            = int(*)(int, int, int, int);
	using ADL_DISPLAY_DISPLAYINFO_GET      = int(*)(int, int*, ADLDisplayInfo**, int);

	// Function pointers
	ADL_MAIN_CONTROL_CREATE          ADL_Main_Control_Create;
	ADL_MAIN_CONTROL_DESTROY         ADL_Main_Control_Destroy;
	ADL_ADAPTER_NUMBEROFADAPTERS_GET ADL_Adapter_NumberOfAdapters_Get;
	ADL_ADAPTER_ADAPTERINFO_GET      ADL_Adapter_AdapterInfo_Get;
	ADL_DISPLAY_DISPLAYINFO_GET      ADL_Display_DisplayInfo_Get;
	ADL_DISPLAY_COLORCAPS_GET        ADL_Display_ColorCaps_Get;
	ADL_DISPLAY_COLOR_GET            ADL_Display_Color_Get;
	ADL_DISPLAY_COLOR_SET            ADL_Display_Color_Set;

	// Private fields
	HINSTANCE hDll;
	LPAdapterInfo adapterInfo;
	int numberAdapters;
	std::map<std::wstring, DisplayAdapterInfo> displays;

	// Private functions
	FeatureValues GetFeatureValues(const std::wstring displayName, const int feature) const;
	bool HasSupportForAllFeatures(const DisplayAdapterInfo displayInfo) const;
	void SetFeatureValues(const std::wstring displayName, const int feature, const int newValue) const;

	// Private memory (de-)allocation functions
	static void* __stdcall ADL_Main_Memory_Alloc(const int iSize);
	static void __stdcall ADL_Main_Memory_Free(void** lpBuffer);
};
