#pragma once

class DriverInterface {
public:
	struct FeatureValues {
		int currentValue;
		int defaultValue;
		int minValue;
		int maxValue;
	};

	DriverInterface() = default;
	virtual ~DriverInterface() = default;
	virtual std::vector<std::wstring> GetDisplayNames() const = 0;

	virtual FeatureValues GetDigitalVibranceInfo(std::wstring displayName) const = 0;
	virtual void SetDigitalVibrance(std::wstring displayName, int newValue) const = 0;

	virtual FeatureValues GetSaturationInfo(std::wstring displayName) const = 0;
	virtual void SetSaturation(std::wstring displayName, int newValue) const = 0;

	virtual FeatureValues GetContrastInfo(std::wstring displayName) const = 0;
	virtual void SetContrast(std::wstring displayName, int newValue) const = 0;

	virtual FeatureValues GetBrightnessInfo(std::wstring displayName) const = 0;
	virtual void SetBrightness(std::wstring displayName, int newValue) const = 0;

	virtual FeatureValues GetHueInfo(std::wstring displayName) const = 0;
	virtual void SetHue(std::wstring displayName, int newValue) const = 0;
};
