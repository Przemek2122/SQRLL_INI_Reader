// Created by https://www.linkedin.com/in/przemek2122/ 2024

#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class SQRLLParser;
class FIniObject;

struct SQRLLIniSettings
{
	SQRLLIniSettings()
		: InSeparatorCharArray({ '=' })
		, InCommentCharArray({ ';', '#' })
		, InIgnoredCharArray({ ' ', '	' })
		, IniSuffix(".ini")
	{
	}

	std::vector<char> InSeparatorCharArray;
	std::vector<char> InCommentCharArray;
	std::vector<char> InIgnoredCharArray;

	std::string IniSuffix;
};

/**
 * Class for managing ini files.
 * Each ini file gets its own object for management.
 */
class FIniManager
{
public:
	FIniManager(SQRLLIniSettings InIniSettings = SQRLLIniSettings());

	/**
	 * Get existing or create new ini object
	 * @param IniName expects unique ini name for search purposes in map
	 * @param IniPath expects path to asset (Whole /var/config/SomeIniFile.ini)
	 */
	std::shared_ptr<FIniObject> GetIniObject(const std::string& IniName, const std::string& IniPath);

	/** Shared ini parser for FIniObject */
	SQRLLParser* GetIniParser() const;

	/**
	 * In case where ini is huge or you know for sure you do not want it to exist anymore.
	 * You can remove it from the array so if it do not have other references it will be removed.
	 */
	void RemoveIniObject(const std::string& IniName);

protected:
	/**
	 * Create new ini object.
	 * @param IniName expects unique ini name for search purposes in map
	 * @param IniPath expects path to asset (Whole /var/config/SomeIniFile.ini)
	 */
	std::shared_ptr<FIniObject> CreateIniObject(const std::string& IniName, const std::string& IniPath);

protected:
	/** Map of the objects. String to Object map. */
	std::pmr::unordered_map<std::string, std::shared_ptr<FIniObject>> IniNameToObjectMap;

	/** ini parser for shared access */
	std::shared_ptr<SQRLLParser> IniParser;

	SQRLLIniSettings IniSettings;

};
