#include "string_processing.h"
#include <algorithm>
#include <sstream>
/*
std::vector<std::string> SplitIntoWords(const std::string &text) {
	std::vector<std::string> words{};
	std::istringstream instream(text, std::istringstream::in);
	std::string word = "";
	while (instream >> word) {
		words.push_back(word);
		word.clear();
	}
	return words;
}
*/
std::vector<std::string> SplitIntoWords(const std::string& text)
{
	std::vector<std::string> words;
	std::string word;
	for (const char c : text) {
		if (c == ' ') {
			if (!word.empty()) {
				words.push_back(word);
				word.clear();
			}
		}
		else {
			word += c;
		}
	}
	if (!word.empty()) {
		words.push_back(word);
	}
	return words;
}
//из задания со string_view
std::vector<std::string_view> SplitIntoWordsView(std::string_view str) {

	std::vector<std::string_view> result;
	str.remove_prefix(std::min(str.find_first_not_of(" "), str.size()));
	while (!str.empty()) {
		int64_t space = str.find(' ');
		result.push_back(str.substr(0, static_cast<size_t>(space)));

		if (str.find(' ') != str.npos)
		{
			str.remove_prefix(static_cast<size_t>(space));
			str.remove_prefix(std::min(str.find_first_not_of(" "), str.size()));
		}
		else {
			str.remove_prefix(str.size());
		}
	}
	return result;
}