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
	AR1, DR1, SR1, RR1, SL1, TL1, KS1, ML1, DT1, AM1, SSGEG1,
	AR2, DR2, SR2, RR2, SL2, TL2, KS2, ML2, DT2, AM2, SSGEG2,
	AR3, DR3, SR3, RR3, SL3, TL3, KS3, ML3, DT3, AM3, SSGEG3,
	AR4, DR4, SR4, RR4, SL4, TL4, KS4, ML4, DT4, AM4, SSGEG4
};

using FmInEnvelopeOrders = std::vector<FmEnvelopeTextType>;
using FmInEnvelopeFormats = std::map<std::string, FmInEnvelopeOrders>;

using FmOutEnvelopeFormats = std::map<std::string, std::string>;

class ToneConverter
{
public:
	ToneConverter();
	~ToneConverter();
	QString toneToText(const Tone& tone, const QString& type) const;
	FmOutEnvelopeFormats getOutputFormats() const;
	void setOutputFormats(FmOutEnvelopeFormats list);
	Tone* textToTone(const QString& text, const QString& type) const;
	FmInEnvelopeFormats getInputFormats() const;
	void setInputFormats(FmInEnvelopeFormats list);

private:
	static const QString FORMAT_PATH_;
	FmOutEnvelopeFormats outFormats_;
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
	std::string replaceMacroWithData(std::string src, const std::string regex, const int value) const;
	std::string replaceMacroWithData(std::string src, const std::string regex, const std::string str) const;
	void parseMacro(std::string src, Manip& manip) const;
};
