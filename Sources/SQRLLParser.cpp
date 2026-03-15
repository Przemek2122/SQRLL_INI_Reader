// Created by https://www.linkedin.com/in/przemek2122/ 2024-2026

#include "SQRLLParser.h"
#include <algorithm>

SQRLLParserLine::SQRLLParserLine(SQRLLParserLine&& InOther) noexcept
	: Texts(std::move(InOther.Texts))
{
}

SQRLLParserLine::SQRLLParserLine(const SQRLLParserLine& InOther)
	: Texts(InOther.Texts)
{
}

SQRLLParserLine::SQRLLParserLine(std::vector<SQRLLParserText> InTexts)
	: Texts(std::move(InTexts))
{
}

void SQRLLParserLine::operator=(const SQRLLParserLine& InOther)
{
	Texts = InOther.Texts;
}

SQRLLParserException::SQRLLParserException(const char* msg)
	: message(msg)
{
}

const char* SQRLLParserException::what() const noexcept
{
	return message.c_str();
}

SQRLLParser::SQRLLParser(SQRLLParserData InParserData)
	: SeparatorCharArray(std::move(InParserData.SeparatorCharArray))
	, CommentCharArray(std::move(InParserData.CommentCharArray))
	, IgnoredCharArray(std::move(InParserData.IgnoredCharArray))
	, NewLineChar(InParserData.NewLineChar)
{
	if (SeparatorCharArray.empty())
	{
		// This is crucial
		throw SQRLLParserException("FParser has no separator characters.");
	}

	if (CommentCharArray.empty())
	{
		// This is nice to have
		throw SQRLLParserException("FParser has no comment characters.");
	}

	if (IgnoredCharArray.empty())
	{
		// This is not crucial
		throw SQRLLParserException("FParser has no ignored characters.");
	}
}

std::vector<std::string> SQRLLParser::SimpleParseLineIntoStrings(const std::string& Line)
{
	std::vector<std::string> ParsedStrings;

	std::string CurrentWord;

	for (const char& Char : Line)
	{
		if (IsComment(Char))
		{
			// Comment means we stop interpreting as comment goes to the end of the line
			break;
		}
		
		if (IsSeparator(Char))
		{
			ParsedStrings.push_back(CurrentWord);

			CurrentWord.clear();
		}
		else if (!IsIgnored(Char))
		{
			CurrentWord += Char;
		}
	}

	if (!CurrentWord.empty())
	{
		ParsedStrings.push_back(CurrentWord);
	}

	return ParsedStrings;
}

std::string SQRLLParser::SimpleParseStringsIntoLine(const std::vector<std::string>& Strings) const
{
	std::string ParsedLine;

	const size_t StringsCount = Strings.size();
	const size_t LastStringIndex = StringsCount - 1;

	for (size_t i = 0; i < StringsCount; i++)
	{
		const bool bIsLast = (i == LastStringIndex);

		const std::string& String = Strings[i];

		if (bIsLast)
		{
			ParsedLine += String;
		}
		else
		{
			ParsedLine += String + SeparatorCharArray[0];
		}
	}

	return ParsedLine;
}

std::vector<SQRLLParserLine> SQRLLParser::AdvancedParseStringIntoLines(const std::string& String)
{
	std::string CurrentWord;

	SQRLLParserLine ParserLine;
	std::vector<SQRLLParserLine> ParserLineContainer;

	SQRLLParserTextType ParserType = SQRLLParserTextType::Word;

	for (const char& CurrentChar : String)
	{
		// Line split - Next line
		if (CurrentChar == NewLineChar)
		{
			ParserLineContainer.push_back(ParserLine);

			// Reset data for next line / word
			ParserLine = SQRLLParserLine();
			CurrentWord = "";

			continue;
		}

		if (IsComment(CurrentChar) || ParserType == SQRLLParserTextType::Comment)
		{
			if (ParserType != SQRLLParserTextType::Comment)
			{
				ParserType = SQRLLParserTextType::Comment;

				// We got comment to end of the line - always
				if (!CurrentWord.empty())
				{
					ParserLine.Texts.emplace_back(CurrentWord, ParserType);

					CurrentWord.clear();
				}
			}

			CurrentWord += CurrentChar;
		}
		else if (IsIgnored(CurrentChar))
		{
			if (!CurrentWord.empty())
			{
				ParserLine.Texts.emplace_back(CurrentWord, ParserType);

				CurrentWord.clear();
			}

			ParserLine.Texts.emplace_back(&CurrentChar, SQRLLParserTextType::Ignored);
		}
		else
		{
			CurrentWord += CurrentChar;
		}
	}

	ParserLine.Texts.emplace_back(CurrentWord, ParserType);

	return std::move(ParserLineContainer);
}

std::string SQRLLParser::AdvancedParseLinesIntoString(const std::vector<SQRLLParserLine>& Lines) const
{
	std::string OutParsedString;

	for (size_t CurrentLineIndex = 0; CurrentLineIndex < Lines.size(); CurrentLineIndex++)
	{
		const SQRLLParserLine& ParserLine = Lines[CurrentLineIndex];

		std::string CurrentLine;

		for (size_t ParserTextIndex = 0; ParserTextIndex < ParserLine.Texts.size(); ParserTextIndex++)
		{
			const SQRLLParserText& FParserText = ParserLine.Texts[ParserTextIndex];

			std::string CurrentWord;

			switch (FParserText.Type)
			{
				case SQRLLParserTextType::Word:
				{
					// Do not add separator before first value
					if (ParserTextIndex == 0)
					{
						CurrentWord += FParserText.Text;
					}
					else
					{
						CurrentWord += SeparatorCharArray[0] + FParserText.Text;
					}

					break;
				}

				case SQRLLParserTextType::Comment:
				{
					CurrentWord += CommentCharArray[0] + FParserText.Text;

					break;
				}

				case SQRLLParserTextType::Ignored:
				{
					CurrentWord += IgnoredCharArray[0] + FParserText.Text;

					break;
				}

				case SQRLLParserTextType::Unknown:
				{
					throw SQRLLParserException("FParser found unkown type of line.");

					break;
				}

				default:
				{
					throw SQRLLParserException("FParser found default type of line.");
				}
			}

			CurrentLine += CurrentWord;
		}

		if (CurrentLineIndex != Lines.size() - 1)
		{
			OutParsedString += CurrentLine + NewLineChar;
		}
		else
		{
			// Do not add new line at the end (it is not needed for the last line
			OutParsedString += CurrentLine;
		}
	}

	return OutParsedString;
}

std::vector<std::string> SQRLLParser::SplitString(const std::string& InString, const std::vector<char>& InSeparatorCharArray)
{
	std::vector<std::string> SubStrings;

	std::string CurrentSubString;

	for (char Char : InString)
	{
		if (std::find(InSeparatorCharArray.begin(), InSeparatorCharArray.end(), Char) != InSeparatorCharArray.end())
		{
			SubStrings.emplace_back(CurrentSubString);

			CurrentSubString = "";
		}
		else
		{
			CurrentSubString += Char;
		}
	}

	return SubStrings;
}

bool SQRLLParser::AreCharsEqual(const char A, const char B)
{
	return (A == B);
}

bool SQRLLParser::IsComment(const char Any)
{
	bool bIsMatchFound = false;

	for (const char CommentChar : CommentCharArray)
	{
		if (AreCharsEqual(CommentChar, Any))
		{
			bIsMatchFound = true;

			break;
		}
	}

	return bIsMatchFound;
}

bool SQRLLParser::IsSeparator(const char Any)
{
	bool bIsMatchFound = false;

	for (const char& SeparatorChar : SeparatorCharArray)
	{
		if (AreCharsEqual(SeparatorChar, Any))
		{
			bIsMatchFound = true;

			break;
		}
	}

	return bIsMatchFound;
}

bool SQRLLParser::IsIgnored(char Any)
{
	bool bIsMatchFound = false;

	for (const char& IgnoredChar : IgnoredCharArray)
	{
		if (AreCharsEqual(IgnoredChar, Any))
		{
			bIsMatchFound = true;

			break;
		}
	}

	return bIsMatchFound;
}
