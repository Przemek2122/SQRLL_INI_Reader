// Created by https://www.linkedin.com/in/przemek2122/ 2024

#pragma once

#include "CoreMinimal.h"

class FParser;
class FIniObject;

/**
 * Class for managing ini files.
 * Each ini file gets its own object for management.
 */
class ENGINE_API FIniManager
{
public:
	FIniManager(FAssetsManager* InAssetsManager);

	/** Get existing or create new ini object */
	std::shared_ptr<FIniObject> GetIniObject(const std::string& IniName);

	/** Shared ini parser for FIniObject */
	FParser* GetIniParser() const;

	/**
	 * In case where ini is huge or you know for sure you do not want it to exist anymore.
	 * You can remove it from the array so if it do not have other references it will be removed.
	 */
	void RemoveIniObject(const std::string& IniName);

	FAssetsManager* GetAssetsManager() const;

protected:
	/** Create new ini object. */
	std::shared_ptr<FIniObject> CreateIniObject(const std::string& IniName);

	/** Adds base path for ini name to be able to load file. */
	std::string ConvertIniNameToRelativeFullPath(const std::string& IniName) const;

protected:
	/** Map of the objects. String to Object map. */
	CMap<std::string, std::shared_ptr<FIniObject>> IniNameToObjectMap;

	/** ini parser for shared access */
	std::shared_ptr<FParser> IniParser;

	CArray<char> InSeparatorCharArray;
	CArray<char> InCommentCharArray;
	CArray<char> InIgnoredCharArray;

	FAssetsManager* AssetsManager;

	std::string IniSuffix;

};
