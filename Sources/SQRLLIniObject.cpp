// Created by https://www.linkedin.com/in/przemek2122/ 2024-2026

#include "SQRLLIniObject.h"
#include "SQRLLParser.h"
#include <cstdint>
#include <fstream>

SQRLLIniObjectException::SQRLLIniObjectException(const char* msg)
	: message(msg)
{
}

const char* SQRLLIniObjectException::what() const noexcept
{
	return message.c_str();
}

SQRLLIniField::SQRLLIniField(std::string InName, std::string InValue)
	: Name(std::move(InName))
	, Value(std::move(InValue))
{
}

std::string SQRLLIniField::GetName() const
{
	return Name;
}

const std::string& SQRLLIniField::GetValueAsString() const
{
	return Value;
}

int SQRLLIniField::GetValueAsInt() const
{
	return atoi(Value.c_str());
}

bool SQRLLIniField::GetValueAsBool() const
{
	static const std::string Predefined_True1 = "true";
	static const std::string Predefined_True2 = "True";
	static const std::string Predefined_True3 = "TRUE";
	static const std::string Predefined_TrueInt = "1";

	if (Value == Predefined_True1 || Value == Predefined_True2 || Value == Predefined_True3 || Value == Predefined_TrueInt)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool SQRLLIniField::IsValid() const
{
	return (!Name.empty() && !Value.empty());
}

SQRLLIniObject::SQRLLIniObject(std::string InIniPath)
	: IniPath(std::move(InIniPath))
{
	bIsLoaded = false;
}

void SQRLLIniObject::LoadIni(SQRLLParserData ParserDefaults)
{
	if (bIsLoaded) return;

	const std::string WholeIniTextFile = ReadWholeFile();
	if (!WholeIniTextFile.empty())
	{
		throw SQRLLIniObjectException("Ini file is empty.");
	}

	SQRLLParser Parser(std::move(ParserDefaults));
	const std::vector<SQRLLParserLine> Lines = Parser.AdvancedParseStringIntoLines(WholeIniTextFile);

	for (const SQRLLParserLine& Line : Lines)
	{
		std::string IniKey, IniValue;

		for (const SQRLLParserText& Text : Line.Texts)
		{
			if (Text.Type == SQRLLParserTextType::Word)
			{
				if (IniKey.empty())
				{
					IniKey = Text.Text;
				}
				else if (Text.Text != "=" && IniValue.empty())
				{
					IniValue = Text.Text;
				}
			}
		}

		const SQRLLIniField IniField(IniKey, IniValue);
		if (IniField.IsValid())
		{
			FieldsMap.emplace(IniKey, std::make_shared<SQRLLIniField>(IniField));
		}
	}

	bIsLoaded = true;
}

void SQRLLIniObject::SaveIni(SQRLLParserData ParserDefaults)
{
	std::vector<SQRLLParserLine> Lines;

	// Convert everything we have into string
	for (std::pair<const std::string, std::shared_ptr<SQRLLIniField>>& Field : FieldsMap)
	{
		SQRLLParserLine ParserLine;

		SQRLLParserText KeyParserText(Field.first, SQRLLParserTextType::Word);
		ParserLine.Texts.push_back(KeyParserText);

		SQRLLParserText ValueParserText(Field.second.get()->GetValueAsString(), SQRLLParserTextType::Word);
		ParserLine.Texts.push_back(ValueParserText);

		Lines.push_back(ParserLine);
	}

	const SQRLLParser Parser(std::move(ParserDefaults));
	std::string WholeFileString = Parser.AdvancedParseLinesIntoString(Lines);

	std::ofstream File(IniPath, std::ios::binary);
	if (File.is_open())
	{
		File.write(WholeFileString.data(), WholeFileString.size());
	}
}

void SQRLLIniObject::SaveIniOnlyIfLoaded(SQRLLParserData ParserDefaults)
{
	if (bIsLoaded)
	{
		SaveIni(std::move(ParserDefaults));
	}
}

bool SQRLLIniObject::ContainsFieldByName(const std::string& FieldName) const
{
	return FieldsMap.find(FieldName) != FieldsMap.end();
}

SQRLLIniField SQRLLIniObject::FindFieldByName(const std::string& FieldName) const
{
	SQRLLIniField Value;

	for (const std::pair<const std::string, std::shared_ptr<SQRLLIniField>>& Field : FieldsMap)
	{
		if (Field.first == FieldName)
		{
			Value = *Field.second;

			break;
		}
	}

	return Value;
}

SQRLLIniField SQRLLIniObject::FindFieldByValue(const std::string& FieldValue)
{
#if DEBUG
	if (!bIsLoaded)
	{
		LOG_ERROR("Accessing FindFieldByValue before ini is loaded, ini name: " << IniName);
	}
#endif

	SQRLLIniField Value;

	for (const std::pair<const std::string, std::shared_ptr<SQRLLIniField>>& Field : FieldsMap)
	{
		if (Field.second->GetValueAsString() == FieldValue)
		{
			Value = *Field.second;

			break;
		}
	}

	return Value;
}

void SQRLLIniObject::AddOrUpdateField(const SQRLLIniField& IniField)
{
	const std::string FieldName = IniField.GetName();
	if (!FieldName.empty())
	{
		if (ContainsFieldByName(FieldName))
		{
			FieldsMap[FieldName] = std::make_shared<SQRLLIniField>(IniField);
		}
		else
		{
			FieldsMap.emplace(FieldName, std::make_shared<SQRLLIniField>(IniField));
		}
	}
}

uint32_t SQRLLIniObject::GetNumberOfFields() const
{
	return FieldsMap.size();
}

std::string SQRLLIniObject::ReadWholeFile() const
{
	// 1. Open at the end (ios::ate) to find the file size immediately
	// 2. Open in binary mode (ios::binary) to avoid slow newline translations
	std::ifstream file(IniPath, std::ios::binary | std::ios::ate);

	if (!file) return ""; // Handle error

	const std::streamsize IniFileSize = file.tellg(); // Get the size
	file.seekg(0, std::ios::beg);        // Move back to the beginning

	std::string TextBuffer;
	TextBuffer.resize(IniFileSize);                 // Allocate memory ONCE

	// 3. Perform a single bulk read
	if (file.read(&TextBuffer[0], IniFileSize))
	{
		return TextBuffer;
	}

	return "";
}
