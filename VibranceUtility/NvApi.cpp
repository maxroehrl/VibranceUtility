#include "stdafx.h"
#include "NvApi.h"

NvApi::NvApi() {
	// Try loading the 64 bit library.
	hDll = LoadLibraryA("nvapi64.dll");

	// A 32 bit calling application on 64 bit OS will fail to LoadLibrary.
	// Try to load the 32 bit library (nvapi.dll) instead.
	if (hDll == nullptr) {
		hDll = LoadLibraryA("nvapi.dll");
	}

	if (hDll == nullptr) {
		throw std::runtime_error("nvapi.dll not found");
	}

	// Try loading the query function pointer.
	NvAPI_QueryInterface = reinterpret_cast<NvAPI_QueryInterface_t>(GetProcAddress(hDll, "nvapi_QueryInterface"));

	// Use the query funtion to get the pointers to the used functions.
	// The offset of each function in the NvApi can be found at:
	// https://github.com/jNizM/AHK_NVIDIA_NvAPI/blob/master/src/Class_NvAPI.ahk
	NvAPI_Initialize                       = reinterpret_cast<NvAPI_Initialize_t>(                    (*NvAPI_QueryInterface)(0x0150E828));
	NvAPI_Unload                           = reinterpret_cast<NvAPI_Unload_t>(                        (*NvAPI_QueryInterface)(0xD22BDD7E));
	NvAPI_GetDVCInfoEx                     = reinterpret_cast<NvAPI_GetDVCInfoEx_t>(                  (*NvAPI_QueryInterface)(0x0E45002D));
	NvAPI_SetDVCLevel                      = reinterpret_cast<NvAPI_SetDVCLevel_t>(                   (*NvAPI_QueryInterface)(0x172409B4));
	NvAPI_EnumNvidiaDisplayHandle          = reinterpret_cast<NvAPI_EnumNvidiaDisplayHandle_t>(       (*NvAPI_QueryInterface)(0x9ABDD40D));
	NvAPI_EnumPhysicalGPUs                 = reinterpret_cast<NvAPI_EnumPhysicalGPUs_t>(              (*NvAPI_QueryInterface)(0xE5AC921F));
	NvAPI_GetAssociatedNvidiaDisplayName   = reinterpret_cast<NvAPI_GetAssociatedNvidiaDisplayName_t>((*NvAPI_QueryInterface)(0x22A78B05));
	NvAPI_GPU_GetSystemType                = reinterpret_cast<NvAPI_GPU_GetSystemType_t>(             (*NvAPI_QueryInterface)(0xBAAABFCC));

	NvAPI_Initialize();

	// Only desktop GPUs support changing the digital vibrance.
	if (IsDesktopGPU()) {
		// Add all displays to the display names vector.
		int counter = 0;
		NvDisplayHandle handle = nullptr;
		char name[64] = {0};
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

		while (NvAPI_EnumNvidiaDisplayHandle(counter, &handle) != NVAPI_END_ENUMERATION) {
			NvAPI_GetAssociatedNvidiaDisplayName(handle, name);
			displays.insert(make_pair(converter.from_bytes(name), handle));
			counter++;
		}
	}
}

NvApi::~NvApi() {
	NvAPI_Unload();
	FreeLibrary(hDll);
}

std::vector<std::wstring> NvApi::GetDisplayNames() const {
	std::vector<std::wstring> names;

	for (auto const& display : displays) {
		names.push_back(display.first);
	}
	return names;
}

DriverInterface::FeatureValues NvApi::GetDigitalVibranceInfo(const std::wstring displayName) const {
	NV_DISPLAY_DVC_INFO_EX info = {};
	info.version = sizeof(NV_DISPLAY_DVC_INFO_EX) | 0x10000;
	NvAPI_GetDVCInfoEx(displays.at(displayName), 0, &info);

	// HACK: The reported min digital vibrance of 0 sets the vibrance to 50.
	info.currentLevel = 2 * (info.currentLevel - 50);
	info.defaultLevel -= 50;

	return {info.currentLevel, info.defaultLevel, info.minLevel, info.maxLevel};
}

void NvApi::SetDigitalVibrance(const std::wstring displayName, const int newValue) const {
	// HACK: The reported max digital vibrance of 100 gets already set with the newValue 63.
	NvAPI_SetDVCLevel(displays.at(displayName), 0, static_cast<int>(newValue * 0.63));
}

DriverInterface::FeatureValues NvApi::GetSaturationInfo(const std::wstring displayName) const {
	throw std::runtime_error("Unsupported operation");
}

void NvApi::SetSaturation(const std::wstring displayName, const int newValue) const {
	throw std::runtime_error("Unsupported operation");
}

DriverInterface::FeatureValues NvApi::GetContrastInfo(const std::wstring displayName) const {
	throw std::runtime_error("Unsupported operation");
}

void NvApi::SetContrast(const std::wstring displayName, const int newValue) const {
	throw std::runtime_error("Unsupported operation");
}

DriverInterface::FeatureValues NvApi::GetBrightnessInfo(const std::wstring displayName) const {
	throw std::runtime_error("Unsupported operation");
}

void NvApi::SetBrightness(const std::wstring displayName, const int newValue) const {
	throw std::runtime_error("Unsupported operation");
}

DriverInterface::FeatureValues NvApi::GetHueInfo(const std::wstring displayName) const {
	throw std::runtime_error("Unsupported operation");
}

void NvApi::SetHue(const std::wstring displayName, const int newValue) const {
	throw std::runtime_error("Unsupported operation");
}

bool NvApi::IsDesktopGPU() const {
	int count, system_type;
	NvPhysicalGpuHandle handles[64] = {nullptr};

	NvAPI_EnumPhysicalGPUs(handles, &count);
	NvAPI_GPU_GetSystemType(handles[0], &system_type);

	return system_type == NV_SYSTEM_TYPE_DESKTOP;
}
