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
	virtual FeatureValues GetSaturationInfo(const std::wstring displayName) const = 0;
	virtual void SetSaturation(const std::wstring displayName, const int newValue) const = 0;
};
