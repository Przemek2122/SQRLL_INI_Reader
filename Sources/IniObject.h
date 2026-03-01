// Created by https://www.linkedin.com/in/przemek2122/ 2024

#pragma once

#include "CoreMinimal.h"

class FIniAsset;
class FIniManager;

/**
 * Struct contains field from ini file.
 * It also can be converted to most common types - int and bool
 */
struct ENGINE_API FIniField
{
public:
	FIniField(std::string InName = std::string(), std::string InValue = std::string());

	/** @returns name of the field. */
	std::string GetName() const;

	/** @returns value of the field - most efficient way, returns string */
	const std::string& GetValueAsString() const;

	/** Performs atoi to int if possible */
	int GetValueAsInt() const;

	/** checks if string appears to be bool or returns false. */
	bool GetValueAsBool() const;

	bool IsValid() const;
	
protected:
	std::string Name;
	std::string Value;
};

class ENGINE_API FIniObject
{
public:
	FIniObject(FIniManager* InIniManager, std::string InIniPath, std::string InIniName);
	~FIniObject();

	void Initialize();

	bool DoesIniExist() const;

	void LoadIni();
	void SaveIni();

	/** @returns true if loaded ini contains given field name */
	[[nodiscard]] bool ContainsFieldByName(const std::string& FieldName) const;

	/** Avoid if possible because it will iterate all records until value is found. @returns true if loaded ini contains given field value. */
	[[nodiscard]] bool ContainsFieldByValue(const std::string& FieldValue) const;

	/** Recommended. Find field by name in map. */
	[[nodiscard]] FIniField FindFieldByName(const std::string& FieldName);

	/** Avoid if possible because it will iterate all records until value is found. Find field by value in map. */
	[[nodiscard]] FIniField FindFieldByValue(const std::string& FieldValue);

	/** Add or update field using given struct. */
	void AddOrUpdateField(const FIniField& IniField);

	/** Get size of fields array */
	int32 GetNumberOfFields() const;

protected:
	/** Fields map of ini file. */
	CUnorderedMap<std::string, std::shared_ptr<FIniField>> FieldsMap;

	/** Owner manager */
	FIniManager* IniManager;

	/** Pointer to the ini */
	std::shared_ptr<FIniAsset> IniAssetSharedPtr;

	/** relative path including ini name */
	std::string IniPath;

	/** Ini name only */
	std::string IniName;


#if DEBUG
	bool bIsLoaded;
#endif
};
