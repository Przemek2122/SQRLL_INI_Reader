// Created by https://www.linkedin.com/in/przemek2122/ 2024-2026

#pragma once

#include "SQRLLParser.h"
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>

/** Custom exception for ini object */
class SQRLLIniObjectException : public std::exception
{
private:
	std::pmr::string message;
public:
	SQRLLIniObjectException(const char* msg);

	[[nodiscard]] const char* what() const noexcept override;
};

/**
 * Struct contains field from ini file.
 * It also can be converted to most common types - int and bool
 */
struct SQRLLIniField
{
public:
	SQRLLIniField(std::string InName = std::string(), std::string InValue = std::string());

	/** @returns name of the field. */
	[[nodiscard]] std::string GetName() const;

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

/**
 * Ini object, used for loading / saving ini file.
 * Ussage:
 * Create object -> LoadIni -> FindFieldByName
 */
class SQRLLIniObject
{
public:
	SQRLLIniObject(std::string InIniPath);

	/** If you want to check if file exists, use Initialize to load than IsLoaded() */
	bool IsLoaded() const { return bIsLoaded; }

	/** Load disk data into RAM. */
	void LoadIni(SQRLLParserData ParserDefaults = SQRLLParserData());

	/** Sav disk data into RAM. We do not care if it was loaded before. If not and called. Overwrite. */
	void SaveIni(SQRLLParserData ParserDefaults = SQRLLParserData());

	/** SaveIni but will save only if loaded before */
	void SaveIniOnlyIfLoaded(SQRLLParserData ParserDefaults = SQRLLParserData());

	/** @returns true if loaded ini contains given field name */
	[[nodiscard]] bool ContainsFieldByName(const std::string& FieldName) const;

	/** Recommended. Find field by name in map. */
	[[nodiscard]] SQRLLIniField FindFieldByName(const std::string& FieldName) const;

	/** Avoid if possible because it will iterate all records until value is found. Find field by value in map. */
	[[nodiscard]] SQRLLIniField FindFieldByValue(const std::string& FieldValue);

	/** Add or update field using given struct. */
	void AddOrUpdateField(const SQRLLIniField& IniField);

	/** Get size of fields array */
	uint32_t GetNumberOfFields() const;

private:
	std::string ReadWholeFile() const;

private:
	/** Fields map of ini file. */
	std::unordered_map<std::string, std::shared_ptr<SQRLLIniField>> FieldsMap;

	/** relative path including ini name */
	std::string IniPath;

	bool bIsLoaded;
};
