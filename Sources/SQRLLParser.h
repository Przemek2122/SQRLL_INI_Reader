// Created by https://www.linkedin.com/in/przemek2122/ 2024-2026

#pragma once

#include <cstdint>
#include <string>
#include <vector>

/** Defaults for Parser. */
namespace SQRLLParserDefaults
{
	static constexpr char NewLineChar = '\n';
	static const std::vector DefaultSeparatorCharArray = { ',' };
	static const std::vector DefaultCommentCharArray = { '#' };
	static const std::vector DefaultIgnoredCharArray = { ' ' };
}

/** Structure with parser data */
struct SQRLLParserData
{
	SQRLLParserData(
		const char InNewLineChar = SQRLLParserDefaults::NewLineChar,
		std::vector<char> InSeparatorCharArray = SQRLLParserDefaults::DefaultSeparatorCharArray,
		std::vector<char> InCommentCharArray = SQRLLParserDefaults::DefaultCommentCharArray,
		std::vector<char> InIgnoredCharArray = SQRLLParserDefaults::DefaultIgnoredCharArray
	)
		: SeparatorCharArray(std::move(InSeparatorCharArray))
		, CommentCharArray(std::move(InCommentCharArray))
		, IgnoredCharArray(std::move(InIgnoredCharArray))
		, NewLineChar(InNewLineChar)
	{
	}

	std::vector<char> SeparatorCharArray;
	std::vector<char> CommentCharArray;
	std::vector<char> IgnoredCharArray;
	char NewLineChar;
};

/** Enum for ParserLineType, it's used to determine what type of line is it. */
enum class SQRLLParserTextType : uint8_t
{
	/** This type will not have anything added before text */
	Unknown = 0,
	/** This means it's pure word and will have separator added */
	Word,
	/** This type will have comment added before text */
	Comment,
	/** This type will have ignored char added before text @see FParserDefaults::DefaultIgnoredCharArray for defaults */
	Ignored,
};

/** Text which is inside of the line, parsed by dividing one chars of InSeparatorCharArray of FParser */
struct SQRLLParserText
{
	SQRLLParserText(std::string InText, const SQRLLParserTextType InLineType = SQRLLParserTextType::Unknown)
		: Text(std::move(InText))
		, Type(InLineType)
	{
	}

	/** Text of the line */
	std::string Text;

	/** Type of the line */
	SQRLLParserTextType Type;
};

/** Struct for holding parsed line from file or lines to parse into file. */
struct SQRLLParserLine
{
	SQRLLParserLine() = default;

	/** Move constructor */
	SQRLLParserLine(SQRLLParserLine&& InOther) noexcept;

	/** Copy constructor */
	SQRLLParserLine(const SQRLLParserLine& InOther);

	SQRLLParserLine(std::vector<SQRLLParserText> InTexts);

	void operator=(const SQRLLParserLine& InOther);

	std::vector<SQRLLParserText> Texts;
};

/** Custom exception for parser */
class SQRLLParserException : public std::exception
{
private:
	std::pmr::string message;
public:
	SQRLLParserException(const char* msg);

	[[nodiscard]] const char* what() const noexcept override;
};

/**
 * Class for parsing strings into words, it's meant to be used for parsing files and commands.
 * It's not meant to be used for parsing code, as it's not meant to be fast.
 */
class SQRLLParser
{
public:
	/**
	 * Constructor for Parser, it's required to call before anything else in this class as it sets basic properties.
	 * @param InSeparatorCharArray Defines which chars are meant to be words breaker
	 * @param InIgnoredCharArray Defines which chars are ignored like they were not present
	 * @param InCommentCharArray Defines which chars means comment and rest of the line after them are ignored.
	 */
	SQRLLParser(SQRLLParserData InParserData);

	/**
	 * Good for line by line work. Bad for anything else
	 * Takes given string and splits it into words by Separators set in constructor
	 */
	std::vector<std::string> SimpleParseLineIntoStrings(const std::string& Line);

	/**
	 * Good for line by line work. Bad for anything else
	 * Reverse of SimpleParseLineIntoStrings, for saving purposes
	 */
	[[nodiscard]] std::string SimpleParseStringsIntoLine(const std::vector<std::string>& Strings) const;

	/**
	 * It's more complex and it returns comments
	 * Takes given string and splits it into lines by Separators set in constructor
	 */
	std::vector<SQRLLParserLine> AdvancedParseStringIntoLines(const std::string& String);

	/**
	 * Reverse of AdvancedParseStringIntoLines, for saving purposes
	 * It's more complex and it adds comments
	 * Takes given lines and merges them into one string with separators
	 */
	[[nodiscard]] std::string AdvancedParseLinesIntoString(const std::vector<SQRLLParserLine>& Lines) const;

	/** Splits given string by given  */
	static std::vector<std::string> SplitString(const std::string& InString, const std::vector<char>& InSeparatorCharArray);

	/** Utility to compare characters */
	inline static bool AreCharsEqual(char A, char B);

	bool IsComment(char Any);
	bool IsSeparator(char Any);
	bool IsIgnored(char Any);

protected:
	std::vector<char> SeparatorCharArray;
	std::vector<char> CommentCharArray;
	std::vector<char> IgnoredCharArray;
	char NewLineChar;

};
