#include "CoreEngine.h"
#include "Assets/IniReader/IniObject.h"

#include "Assets/Assets/IniAsset.h"
#include "Assets/IniReader/IniManager.h"
#include "Misc/StringHelpers.h"

FIniField::FIniField(std::string InName, std::string InValue)
	: Name(std::move(InName))
	, Value(std::move(InValue))
{
}

std::string FIniField::GetName() const
{
	return Name;
}

const std::string& FIniField::GetValueAsString() const
{
	return Value;
}

int FIniField::GetValueAsInt() const
{
	return SDL_atoi(Value.c_str());
}

bool FIniField::GetValueAsBool() const
{
	return FStringHelpers::ToBoolValue(Value);
}

bool FIniField::IsValid() const
{
	return (!Name.empty() && !Value.empty());
}

FIniObject::FIniObject(FIniManager* InIniManager, std::string InIniPath, std::string InIniName)
	: IniManager(InIniManager)
	, IniPath(std::move(InIniPath))
	, IniName(std::move(InIniName))
{
	// IniManager must not be nullptr
	ENSURE_VALID(InIniManager != nullptr);

#if DEBUG
	bIsLoaded = false;
#endif
}

FIniObject::~FIniObject()
{
	FieldsMap.Clear();
}

void FIniObject::Initialize()
{
	FAssetsManager* AssetsManager = IniManager->GetAssetsManager();
	IniAssetSharedPtr = AssetsManager->CreateAssetFromRelativePath<FIniAsset>(IniName, IniPath);
}

bool FIniObject::DoesIniExist() const
{
	return (IniAssetSharedPtr != nullptr && IniAssetSharedPtr->DoesFileExist());
}

void FIniObject::LoadIni()
{
#if DEBUG
	bIsLoaded = true;
#endif

	// Load disk data
	IniAssetSharedPtr->Load(IniManager->GetIniParser());

	const CArray<FParserLine>& Lines = IniAssetSharedPtr->GetLines();
	for (const FParserLine& Line : Lines)
	{
		std::string IniKey, IniValue;

		for (const FParserText& Text : Line.Texts)
		{
			if (Text.Type == EParserTextType::Word)
			{
				if (IniKey.empty())
				{
					IniKey = FStringHelpers::RemoveCharsInString(Text.Text, FFileSystem::GetNewLineChars());
				}
				else if (Text.Text != "=" && IniValue.empty())
				{
					IniValue = FStringHelpers::RemoveCharsInString(Text.Text, FFileSystem::GetNewLineChars());
				}
			}
		}

		FIniField IniField(IniKey, IniValue);
		if (IniField.IsValid())
		{
			FieldsMap.Emplace(IniKey, std::make_shared<FIniField>(IniField));
		}
	}

	// Unloaded after loading ini
	IniAssetSharedPtr->UnLoad();
}

void FIniObject::SaveIni()
{


	// Save data to disk
	IniAssetSharedPtr->Save(IniManager->GetIniParser());
}

bool FIniObject::ContainsFieldByName(const std::string& FieldName) const
{
#if DEBUG
	if (!bIsLoaded)
	{
		LOG_ERROR("Accessing ContainsFieldByName before ini is loaded, ini name: " << IniName);
	}
#endif

	return FieldsMap.ContainsKey(FieldName);
}

bool FIniObject::ContainsFieldByValue(const std::string& FieldValue) const
{
#if DEBUG
	if (!bIsLoaded)
	{
		LOG_ERROR("Accessing ContainsFieldByValue before ini is loaded, ini name: " << IniName);
	}
#endif

	const FFunctorLambda<bool, std::string, std::shared_ptr<FIniField>> SearchFunction = [&](std::string Key, std::shared_ptr<FIniField> Field)
	{
		if (FieldValue == Field->GetValueAsString())
		{
			return true;
		}

		return false;
	};

	const bool bHasValue = FieldsMap.ContainsByPredicate(SearchFunction);

	return bHasValue;
}

FIniField FIniObject::FindFieldByName(const std::string& FieldName)
{
#if DEBUG
	if (!bIsLoaded)
	{
		LOG_ERROR("Accessing FindFieldByName before ini is loaded, ini name: " << IniName);
	}
#endif

	FIniField Value;

	for (const std::pair<const std::string, std::shared_ptr<FIniField>>& Field : FieldsMap)
	{
		if (Field.first == FieldName)
		{
			Value = *Field.second;

			break;
		}
	}

	return Value;
}

FIniField FIniObject::FindFieldByValue(const std::string& FieldValue)
{
#if DEBUG
	if (!bIsLoaded)
	{
		LOG_ERROR("Accessing FindFieldByValue before ini is loaded, ini name: " << IniName);
	}
#endif

	FIniField Value;

	for (const std::pair<const std::string, std::shared_ptr<FIniField>>& Field : FieldsMap)
	{
		if (Field.second->GetValueAsString() == FieldValue)
		{
			Value = *Field.second;

			break;
		}
	}

	return Value;
}

void FIniObject::AddOrUpdateField(const FIniField& IniField)
{
	const std::string FieldName = IniField.GetName();
	if (!FieldName.empty())
	{
		if (ContainsFieldByName(FieldName))
		{
			FieldsMap[FieldName] = std::make_shared<FIniField>(IniField);
		}
		else
		{
			FieldsMap.Emplace(FieldName, std::make_shared<FIniField>(IniField));
		}
	}
}

int32 FIniObject::GetNumberOfFields() const
{
	return FieldsMap.Size();
}
