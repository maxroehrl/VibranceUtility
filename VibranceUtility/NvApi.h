#pragma once
#include "DriverInterface.h"

class NvApi : public DriverInterface {
public:
	NvApi();
	~NvApi() override;
	std::vector<std::wstring> GetDisplayNames() const override;

	FeatureValues GetDigitalVibranceInfo(const std::wstring displayName) const override;
	void SetDigitalVibrance(const std::wstring displayName, const int newValue) const override;
	
	FeatureValues GetSaturationInfo(const std::wstring displayName) const override { throw std::runtime_error("Unsupported operation"); }
	void SetSaturation(const std::wstring displayName, const int newValue) const override { throw std::runtime_error("Unsupported operation"); }

	FeatureValues GetContrastInfo(const std::wstring displayName) const override { throw std::runtime_error("Unsupported operation"); }
	void SetContrast(const std::wstring displayName, const int newValue) const override { throw std::runtime_error("Unsupported operation"); }

	FeatureValues GetBrightnessInfo(const std::wstring displayName) const override { throw std::runtime_error("Unsupported operation"); }
	void SetBrightness(const std::wstring displayName, const int newValue) const override { throw std::runtime_error("Unsupported operation"); }

	FeatureValues GetHueInfo(const std::wstring displayName) const override { throw std::runtime_error("Unsupported operation"); }
	void SetHue(const std::wstring displayName, const int newValue) const override { throw std::runtime_error("Unsupported operation"); }

private:
	const int NVAPI_END_ENUMERATION = -7;
	const int NV_SYSTEM_TYPE_DESKTOP = 2;

	DECLARE_HANDLE(NvDisplayHandle);
	DECLARE_HANDLE(NvPhysicalGpuHandle);

	struct NV_DISPLAY_DVC_INFO_EX {
		unsigned int version;
		int currentLevel;
		int minLevel;
		int maxLevel;
		int defaultLevel;
	};

	// Definitions of the used function pointers
	using NvAPI_QueryInterface_t                   = int*(*)(unsigned int);
	using NvAPI_Initialize_t                       = int(*)();
	using NvAPI_Unload_t                           = int(*)();
	using NvAPI_GetDVCInfoEx_t                     = int(*)(NvDisplayHandle, int, NV_DISPLAY_DVC_INFO_EX*);
	using NvAPI_SetDVCLevel_t                      = int(*)(NvDisplayHandle, int, int);
	using NvAPI_EnumNvidiaDisplayHandle_t          = int(*)(int, NvDisplayHandle*);
	using NvAPI_EnumPhysicalGPUs_t                 = int(*)(NvPhysicalGpuHandle[64], int*);
	using NvAPI_GetAssociatedNvidiaDisplayHandle_t = int(*)(const char*, NvDisplayHandle*);
	using NvAPI_GetAssociatedNvidiaDisplayName_t   = int(*)(NvDisplayHandle, char[64]);
	using NvAPI_GPU_GetSystemType_t                = int(*)(NvPhysicalGpuHandle, int*);

	// Function pointers
	NvAPI_QueryInterface_t                   NvAPI_QueryInterface;
	NvAPI_Initialize_t                       NvAPI_Initialize;
	NvAPI_Unload_t                           NvAPI_Unload;
	NvAPI_GetDVCInfoEx_t                     NvAPI_GetDVCInfoEx;
	NvAPI_SetDVCLevel_t	                     NvAPI_SetDVCLevel;
	NvAPI_EnumNvidiaDisplayHandle_t          NvAPI_EnumNvidiaDisplayHandle;
	NvAPI_EnumPhysicalGPUs_t                 NvAPI_EnumPhysicalGPUs;
	NvAPI_GetAssociatedNvidiaDisplayHandle_t NvAPI_GetAssociatedNvidiaDisplayHandle;
	NvAPI_GetAssociatedNvidiaDisplayName_t   NvAPI_GetAssociatedNvidiaDisplayName;
	NvAPI_GPU_GetSystemType_t                NvAPI_GPU_GetSystemType;

	// Private fields
	HINSTANCE hDll;
	std::vector<std::wstring> displayNames;

	// Private functions
	NvDisplayHandle GetHandle(const std::wstring displayName) const;
	bool IsDesktopGPU() const;
};
