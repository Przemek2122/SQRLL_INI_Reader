#include "CoreEngine.h"
#include "Assets/IniReader/IniManager.h"

#include "Assets/Parser.h"
#include "Assets/IniReader/IniObject.h"

FIniManager::FIniManager(FAssetsManager* InAssetsManager)
	: InSeparatorCharArray({ '=' })
	, InCommentCharArray({ ';', '#' })
	, InIgnoredCharArray({ ' ', '	' })
	, AssetsManager(InAssetsManager)
	, IniSuffix(".ini")
{
	IniParser = std::make_shared<FParser>(InSeparatorCharArray, InCommentCharArray, InIgnoredCharArray);
}

std::shared_ptr<FIniObject> FIniManager::GetIniObject(const std::string& IniName)
{
	std::shared_ptr<FIniObject> IniObject;

	if (IniNameToObjectMap.ContainsKey(IniName))
	{
		IniObject = IniNameToObjectMap[IniName];
	}
	else
	{
		IniObject = CreateIniObject(IniName);
	}

	return IniObject;
}

FParser* FIniManager::GetIniParser() const
{
	return IniParser.get();
}

void FIniManager::RemoveIniObject(const std::string& IniName)
{
	if (IniNameToObjectMap.ContainsKey(IniName))
	{
		IniNameToObjectMap.Remove(IniName);
	}
}

FAssetsManager* FIniManager::GetAssetsManager() const
{
	return AssetsManager;
}

std::shared_ptr<FIniObject> FIniManager::CreateIniObject(const std::string& IniName)
{
	std::string IniFullPath = ConvertIniNameToRelativeFullPath(IniName) + IniSuffix;

	std::shared_ptr<FIniObject> IniObjectSharedPtr = std::make_shared<FIniObject>(this, IniFullPath, IniName);

	IniObjectSharedPtr->Initialize();

	IniNameToObjectMap.Emplace(IniName, IniObjectSharedPtr);

	return IniObjectSharedPtr;
}

std::string FIniManager::ConvertIniNameToRelativeFullPath(const std::string& IniName) const
{
	std::string FullPath;

	FullPath += AssetsManager->GetConfigPathRelative();
	FullPath += FFileSystem::GetPlatformSlash();
	FullPath += IniName;

	return FullPath;
}
