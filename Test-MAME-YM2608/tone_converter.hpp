#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <QString>
#include "tone.hpp"

enum class FmEnvelopeTextType : int
{
	Skip = 0, AL, FB,
	AR1, DR1, SR1, RR1, SL1, TL1, KS1, ML1, DT1, AM1,
	AR2, DR2, SR2, RR2, SL2, TL2, KS2, ML2, DT2, AM2,
	AR3, DR3, SR3, RR3, SL3, TL3, KS3, ML3, DT3, AM3,
	AR4, DR4, SR4, RR4, SL4, TL4, KS4, ML4, DT4, AM4
};

using FmInEnvelopeOrders = std::vector<FmEnvelopeTextType>;
using FmInEnvelopeFormats = std::map<std::string, FmInEnvelopeOrders>;

class ToneConverter
{
public:
	ToneConverter();
	~ToneConverter();
	std::string toneToText(const Tone* tone);
	std::string getOutputFormat() const;
	void setOutputFormat(std::string str);
	std::unique_ptr<Tone> textToTone(const QString& text, const QString& type);
	FmInEnvelopeFormats getInputFormats() const;
	void setInputFormats(FmInEnvelopeFormats list);

private:
	static const QString FORMAT_PATH_;
	std::string outFormat_;
	FmInEnvelopeFormats inFormats_;

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

	void loadFormats();
	std::string replaceMacroWithData(std::string src, const std::string regex, const int value);
	std::string replaceMacroWithData(std::string src, const std::string regex, const std::string str);
	void parseMacro(std::string src, Manip& manip);
};
