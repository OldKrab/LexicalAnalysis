#pragma once
#include <string>
class SourceText
{
public:
	SourceText(std::string sourceText) :sourceText(std::move(sourceText))
	{	}
	SourceText() :sourceText(std::string()) {	}

	operator std::string() const
	{
		return sourceText;
	}

	struct Iterator
	{
		Iterator(const std::string::iterator& curPos) :curPos(curPos), row(1), column(1) {}
		Iterator() :Iterator(std::string::iterator()) {}

		Iterator& operator++() noexcept
		{
			if (*curPos == '\n')
			{
				row++;
				column = 0;
			}
			else
				column++;

			++curPos;

			return *this;
		}

		Iterator operator++(int) noexcept
		{
			auto tmp = *this;
			++* this;
			return tmp;
		}

		char operator*() const noexcept
		{
			return *curPos;
		}

		bool operator==(const Iterator& right) const noexcept {
			return curPos == right.curPos;
		}

		bool operator!=(const Iterator& right) const noexcept {
			return !(*this == right);
		}

		std::string::iterator curPos;
		size_t row, column;
	};

	Iterator begin()
	{
		return { sourceText.begin() };
	}

	Iterator end()
	{
		return { sourceText.end() };
	}
private:
	std::string sourceText;
};

