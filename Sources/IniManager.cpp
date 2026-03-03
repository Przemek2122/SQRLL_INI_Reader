#include "IniManager.h"
#include "SQRLLParser.h"
#include "IniObject.h"

FIniManager::FIniManager(SQRLLIniSettings InIniSettings)
	: IniSettings(std::move(InIniSettings))
{
	IniParser = std::make_shared<SQRLLParser>(
		IniSettings.InSeparatorCharArray,
		IniSettings.InCommentCharArray,
		IniSettings.InIgnoredCharArray
	);
}

std::shared_ptr<FIniObject> FIniManager::GetIniObject(const std::string& IniName, const std::string& IniPath)
{
	std::shared_ptr<FIniObject> IniObject;

	auto It = IniNameToObjectMap.find(IniName);
	if (It != IniNameToObjectMap.end())
	{
		IniObject = IniNameToObjectMap[IniName];
	}
	else
	{
		IniObject = CreateIniObject(IniName, IniPath);
	}

	return IniObject;
}

SQRLLParser* FIniManager::GetIniParser() const
{
	return IniParser.get();
}

void FIniManager::RemoveIniObject(const std::string& IniName)
{
	IniNameToObjectMap.erase(IniName);
}

std::shared_ptr<FIniObject> FIniManager::CreateIniObject(const std::string& IniName, const std::string& IniPath)
{
	std::shared_ptr<FIniObject> IniObjectSharedPtr = std::make_shared<FIniObject>(this, IniPath, IniName);

	IniObjectSharedPtr->Initialize();

	IniNameToObjectMap.emplace(IniName, IniObjectSharedPtr);

	return IniObjectSharedPtr;
}
