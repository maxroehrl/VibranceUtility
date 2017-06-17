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

	virtual FeatureValues GetDigitalVibranceInfo(const std::wstring displayName) const = 0;
	virtual void SetDigitalVibrance(const std::wstring displayName, const int newValue) const = 0;

	virtual FeatureValues GetSaturationInfo(const std::wstring displayName) const = 0;
	virtual void SetSaturation(const std::wstring displayName, const int newValue) const = 0;

	virtual FeatureValues GetContrastInfo(const std::wstring displayName) const = 0;
	virtual void SetContrast(const std::wstring displayName, const int newValue) const = 0;

	virtual FeatureValues GetBrightnessInfo(const std::wstring displayName) const = 0;
	virtual void SetBrightness(const std::wstring displayName, const int newValue) const = 0;

	virtual FeatureValues GetHueInfo(const std::wstring displayName) const = 0;
	virtual void SetHue(const std::wstring displayName, const int newValue) const = 0;
};
