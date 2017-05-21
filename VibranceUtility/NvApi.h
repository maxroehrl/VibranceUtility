#pragma once
#include "DriverInterface.h"

class NvApi : public DriverInterface {
public:
	NvApi();
	~NvApi() override;
	std::vector<std::wstring> GetDisplayNames() const override;
	FeatureValues GetSaturationInfo(const std::wstring displayName) const override;
	void SetSaturation(const std::wstring displayName, const int newValue) const override;

private:
	const int NVAPI_END_ENUMERATION = -7;
	const int NV_SYSTEM_TYPE_DESKTOP = 2;

	struct NvDisplayHandle__ {
		int unused;
	};

	using NvDisplayHandle = NvDisplayHandle__*;

	struct NvPhysicalGpuHandle__ {
		int unused;
	};

	using NvPhysicalGpuHandle = NvPhysicalGpuHandle__*;

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
	using NvApi_EnumPhysicalGPUs_t                 = int(*)(NvPhysicalGpuHandle[64], int*);
	using NvAPI_GetAssociatedNvidiaDisplayHandle_t = int(*)(const char*, NvDisplayHandle*);
	using NvAPI_GetAssociatedNvidiaDisplayName_t   = int(*)(NvDisplayHandle, char[64]);
	using NvApi_GPU_GetSystemType_t                = int(*)(NvPhysicalGpuHandle, int*);

	// Function pointers
	NvAPI_QueryInterface_t                   NvAPI_QueryInterface;
	NvAPI_Initialize_t                       NvAPI_Initialize;
	NvAPI_Unload_t                           NvAPI_Unload;
	NvAPI_GetDVCInfoEx_t                     NvAPI_GetDVCInfoEx;
	NvAPI_SetDVCLevel_t	                     NvAPI_SetDVCLevel;
	NvAPI_EnumNvidiaDisplayHandle_t          NvAPI_EnumNvidiaDisplayHandle;
	NvApi_EnumPhysicalGPUs_t                 NvApi_EnumPhysicalGPUs;
	NvAPI_GetAssociatedNvidiaDisplayHandle_t NvAPI_GetAssociatedNvidiaDisplayHandle;
	NvAPI_GetAssociatedNvidiaDisplayName_t   NvAPI_GetAssociatedNvidiaDisplayName;
	NvApi_GPU_GetSystemType_t                NvApi_GPU_GetSystemType;

	// Private fields
	HINSTANCE hDll;
	std::vector<std::wstring> displayNames;

	// Private functions
	NvDisplayHandle GetHandle(const std::wstring displayName) const;
	bool IsDesktopGPU() const;
};
