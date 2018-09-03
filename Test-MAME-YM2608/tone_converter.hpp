#pragma once

#include <string>
#include <vector>
#include <memory>
#include "tone.hpp"

class ToneConverter
{
public:
    ToneConverter();
	~ToneConverter();
    void loadFormat(std::string path);
	std::string toneToText(const Tone* tone);
	std::string getOutputFormat() const;
	void setOutputFormat(std::string str);
	std::unique_ptr<Tone> textToTone(std::string text, std::vector<int> order);

private:
    std::string format_;

    struct Manip
    {
        bool isFill0 = false;
        int setw = 3;
        void reset()
        {
            isFill0 = false;
            setw = 3;
        }
    };

	std::string replaceMacroWithData(std::string src, const std::string regex, const int value);
	std::string replaceMacroWithData(std::string src, const std::string regex, const std::string str);
	void parseMacro(std::string src, Manip& manip);
};
