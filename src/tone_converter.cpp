#include "tone_converter.hpp"
#include <sstream>
#include <regex>
#include <iomanip>
#include <QSettings>
#include "text_conversion.hpp"

const QString ToneConverter::FORMAT_PATH_ = "formats.ini";

ToneConverter::ToneConverter()
{
	inFormats_ = {
		{
			"PMD",
			{
				FmEnvelopeTextType::Skip, FmEnvelopeTextType::AL, FmEnvelopeTextType::FB,
				FmEnvelopeTextType::AR1, FmEnvelopeTextType::DR1, FmEnvelopeTextType::SR1,
				FmEnvelopeTextType::RR1, FmEnvelopeTextType::SL1, FmEnvelopeTextType::TL1,
				FmEnvelopeTextType::KS1, FmEnvelopeTextType::ML1, FmEnvelopeTextType::DT1,
				FmEnvelopeTextType::AM1, FmEnvelopeTextType::AR2, FmEnvelopeTextType::DR2,
				FmEnvelopeTextType::SR2, FmEnvelopeTextType::RR2, FmEnvelopeTextType::SL2,
				FmEnvelopeTextType::TL2, FmEnvelopeTextType::KS2, FmEnvelopeTextType::ML2,
				FmEnvelopeTextType::DT2, FmEnvelopeTextType::AM2, FmEnvelopeTextType::AR3,
				FmEnvelopeTextType::DR3, FmEnvelopeTextType::SR3, FmEnvelopeTextType::RR3,
				FmEnvelopeTextType::SL3, FmEnvelopeTextType::TL3, FmEnvelopeTextType::KS3,
				FmEnvelopeTextType::ML3, FmEnvelopeTextType::DT3, FmEnvelopeTextType::AM3,
				FmEnvelopeTextType::AR4, FmEnvelopeTextType::DR4, FmEnvelopeTextType::SR4,
				FmEnvelopeTextType::RR4, FmEnvelopeTextType::SL4, FmEnvelopeTextType::TL4,
				FmEnvelopeTextType::KS4, FmEnvelopeTextType::ML4, FmEnvelopeTextType::DT4,
				FmEnvelopeTextType::AM4
			}
		},
		{
			"FMP",
			{
				FmEnvelopeTextType::Skip, FmEnvelopeTextType::AR1, FmEnvelopeTextType::DR1,
				FmEnvelopeTextType::SR1, FmEnvelopeTextType::RR1, FmEnvelopeTextType::SL1,
				FmEnvelopeTextType::TL1, FmEnvelopeTextType::KS1, FmEnvelopeTextType::ML1,
				FmEnvelopeTextType::DT1, FmEnvelopeTextType::AR2, FmEnvelopeTextType::DR2,
				FmEnvelopeTextType::SR2, FmEnvelopeTextType::RR2, FmEnvelopeTextType::SL2,
				FmEnvelopeTextType::TL2, FmEnvelopeTextType::KS2, FmEnvelopeTextType::ML2,
				FmEnvelopeTextType::DT2, FmEnvelopeTextType::AR3, FmEnvelopeTextType::DR3,
				FmEnvelopeTextType::SR3, FmEnvelopeTextType::RR3, FmEnvelopeTextType::SL3,
				FmEnvelopeTextType::TL3, FmEnvelopeTextType::KS3, FmEnvelopeTextType::ML3,
				FmEnvelopeTextType::DT3, FmEnvelopeTextType::AR4, FmEnvelopeTextType::DR4,
				FmEnvelopeTextType::SR4, FmEnvelopeTextType::RR4, FmEnvelopeTextType::SL4,
				FmEnvelopeTextType::TL4, FmEnvelopeTextType::KS4, FmEnvelopeTextType::ML4,
				FmEnvelopeTextType::DT4, FmEnvelopeTextType::AL, FmEnvelopeTextType::FB
			}
		},
		{
			"FMP7",
			{
				FmEnvelopeTextType::Skip, FmEnvelopeTextType::AR1, FmEnvelopeTextType::DR1,
				FmEnvelopeTextType::SR1, FmEnvelopeTextType::RR1, FmEnvelopeTextType::SL1,
				FmEnvelopeTextType::TL1, FmEnvelopeTextType::KS1, FmEnvelopeTextType::ML1,
				FmEnvelopeTextType::DT1, FmEnvelopeTextType::AM1, FmEnvelopeTextType::AR2,
				FmEnvelopeTextType::DR2, FmEnvelopeTextType::SR2, FmEnvelopeTextType::RR2,
				FmEnvelopeTextType::SL2, FmEnvelopeTextType::TL2, FmEnvelopeTextType::KS2,
				FmEnvelopeTextType::ML2, FmEnvelopeTextType::DT2, FmEnvelopeTextType::AM2,
				FmEnvelopeTextType::AR3, FmEnvelopeTextType::DR3, FmEnvelopeTextType::SR3,
				FmEnvelopeTextType::RR3, FmEnvelopeTextType::SL3, FmEnvelopeTextType::TL3,
				FmEnvelopeTextType::KS3, FmEnvelopeTextType::ML3, FmEnvelopeTextType::DT3,
				FmEnvelopeTextType::AM3, FmEnvelopeTextType::AR4, FmEnvelopeTextType::DR4,
				FmEnvelopeTextType::SR4, FmEnvelopeTextType::RR4, FmEnvelopeTextType::SL4,
				FmEnvelopeTextType::TL4, FmEnvelopeTextType::KS4, FmEnvelopeTextType::ML4,
				FmEnvelopeTextType::DT4, FmEnvelopeTextType::AM4, FmEnvelopeTextType::AL,
				FmEnvelopeTextType::FB
			}
		},
		{
			"MMLDRV",
			{
				FmEnvelopeTextType::Skip, FmEnvelopeTextType::AL, FmEnvelopeTextType::FB,
				FmEnvelopeTextType::AR1, FmEnvelopeTextType::DR1, FmEnvelopeTextType::SR1,
				FmEnvelopeTextType::RR1, FmEnvelopeTextType::SL1, FmEnvelopeTextType::TL1,
				FmEnvelopeTextType::KS1, FmEnvelopeTextType::ML1, FmEnvelopeTextType::DT1,
				FmEnvelopeTextType::Skip, FmEnvelopeTextType::AM1, FmEnvelopeTextType::AR2,
				FmEnvelopeTextType::DR2, FmEnvelopeTextType::SR2, FmEnvelopeTextType::RR2,
				FmEnvelopeTextType::SL2, FmEnvelopeTextType::TL2, FmEnvelopeTextType::KS2,
				FmEnvelopeTextType::ML2, FmEnvelopeTextType::DT2, FmEnvelopeTextType::Skip,
				FmEnvelopeTextType::AM2, FmEnvelopeTextType::AR3, FmEnvelopeTextType::DR3,
				FmEnvelopeTextType::SR3, FmEnvelopeTextType::RR3, FmEnvelopeTextType::SL3,
				FmEnvelopeTextType::TL3, FmEnvelopeTextType::KS3, FmEnvelopeTextType::ML3,
				FmEnvelopeTextType::DT3, FmEnvelopeTextType::Skip, FmEnvelopeTextType::AM3,
				FmEnvelopeTextType::AR4, FmEnvelopeTextType::DR4, FmEnvelopeTextType::SR4,
				FmEnvelopeTextType::RR4, FmEnvelopeTextType::SL4, FmEnvelopeTextType::TL4,
				FmEnvelopeTextType::KS4, FmEnvelopeTextType::ML4, FmEnvelopeTextType::DT4,
				FmEnvelopeTextType::Skip, FmEnvelopeTextType::AM4
			}
		},
		{
			"MUCOM88",
			{
				FmEnvelopeTextType::Skip, FmEnvelopeTextType::FB, FmEnvelopeTextType::AL,
				FmEnvelopeTextType::AR1, FmEnvelopeTextType::DR1, FmEnvelopeTextType::SR1,
				FmEnvelopeTextType::RR1, FmEnvelopeTextType::SL1, FmEnvelopeTextType::TL1,
				FmEnvelopeTextType::KS1, FmEnvelopeTextType::ML1, FmEnvelopeTextType::DT1,
				FmEnvelopeTextType::AR2, FmEnvelopeTextType::DR2, FmEnvelopeTextType::SR2,
				FmEnvelopeTextType::RR2, FmEnvelopeTextType::SL2, FmEnvelopeTextType::TL2,
				FmEnvelopeTextType::KS2, FmEnvelopeTextType::ML2, FmEnvelopeTextType::DT2,
				FmEnvelopeTextType::AR3, FmEnvelopeTextType::DR3, FmEnvelopeTextType::SR3,
				FmEnvelopeTextType::RR3, FmEnvelopeTextType::SL3, FmEnvelopeTextType::TL3,
				FmEnvelopeTextType::KS3, FmEnvelopeTextType::ML3, FmEnvelopeTextType::DT3,
				FmEnvelopeTextType::AR4, FmEnvelopeTextType::DR4, FmEnvelopeTextType::SR4,
				FmEnvelopeTextType::RR4, FmEnvelopeTextType::SL4, FmEnvelopeTextType::TL4,
				FmEnvelopeTextType::KS4, FmEnvelopeTextType::ML4, FmEnvelopeTextType::DT4
			}
		}
	};

	outFormats_ = {
		{
			"FMP",
			"'@ %{NO:3}	   ; %{NAME}\n"
			"'@ %{AR1:3},%{DR1:3},%{SR1:3},%{RR1:3},%{SL1:3},%{TL1:3},%{KS1:3},%{ML1:3},%{DT1:3}\n"
			"'@ %{AR2:3},%{DR2:3},%{SR2:3},%{RR2:3},%{SL2:3},%{TL2:3},%{KS2:3},%{ML2:3},%{DT2:3}\n"
			"'@ %{AR3:3},%{DR3:3},%{SR3:3},%{RR3:3},%{SL3:3},%{TL3:3},%{KS3:3},%{ML3:3},%{DT3:3}\n"
			"'@ %{AR4:3},%{DR4:3},%{SR4:3},%{RR4:3},%{SL4:3},%{TL4:3},%{KS4:3},%{ML4:3},%{DT4:3}\n"
			"'@ %{AL:3},%{FB:3}"
		},
		{
			"FMP7",
			"'@ FA  %{NO:3}	   ; %{NAME}\n"
			"'@ %{AR1:3},%{DR1:3},%{SR1:3},%{RR1:3},%{SL1:3},%{TL1:3},%{KS1:3},%{ML1:3},%{DT1:3},%{AM1:3}\n"
			"'@ %{AR2:3},%{DR2:3},%{SR2:3},%{RR2:3},%{SL2:3},%{TL2:3},%{KS2:3},%{ML2:3},%{DT2:3},%{AM2:3}\n"
			"'@ %{AR3:3},%{DR3:3},%{SR3:3},%{RR3:3},%{SL3:3},%{TL3:3},%{KS3:3},%{ML3:3},%{DT3:3},%{AM3:3}\n"
			"'@ %{AR4:3},%{DR4:3},%{SR4:3},%{RR4:3},%{SL4:3},%{TL4:3},%{KS4:3},%{ML4:3},%{DT4:3},%{AM4:3}\n"
			"'@ %{AL:3},%{FB:3}"
		},
		{
			"MMLDRV",
			"// %{NAME}\n"
			"@%{NO}"
			"  %{AL:3} %{FB:3}\n"
			"  %{AR1:3} %{DR1:3} %{SR1:3} %{RR1:3} %{SL1:3} %{TL1:3} %{KS1:3} %{ML1:3} %{DT1:3} %{AM1:3}\n"
			"  %{AR2:3} %{DR2:3} %{SR2:3} %{RR2:3} %{SL2:3} %{TL2:3} %{KS2:3} %{ML2:3} %{DT2:3} %{AM2:3}\n"
			"  %{AR3:3} %{DR3:3} %{SR3:3} %{RR3:3} %{SL3:3} %{TL3:3} %{KS3:3} %{ML3:3} %{DT3:3} %{AM3:3}\n"
			"  %{AR4:3} %{DR4:3} %{SR4:3} %{RR4:3} %{SL4:3} %{TL4:3} %{KS4:3} %{ML4:3} %{DT4:3} %{AM4:3}"
		},
		{
			"MUCOM88",
			"@%{NO}:{\n"
			"%{AL:3}, %{FB:3}\n"
			"%{AR1:3}, %{DR1:3}, %{SR1:3}, %{RR1:3}, %{SL1:3}, %{TL1:3}, %{KS1:3}, %{ML1:3}, %{DT1:3}\n"
			"%{AR2:3}, %{DR2:3}, %{SR2:3}, %{RR2:3}, %{SL2:3}, %{TL2:3}, %{KS2:3}, %{ML2:3}, %{DT2:3}\n"
			"%{AR3:3}, %{DR3:3}, %{SR3:3}, %{RR3:3}, %{SL3:3}, %{TL3:3}, %{KS3:3}, %{ML3:3}, %{DT3:3}\n"
			"%{AR4:3}, %{DR4:3}, %{SR4:3}, %{RR4:3}, %{SL4:3}, %{TL4:3}, %{KS4:3}, %{ML4:3}, %{DT4:3}, \"%{NAME}\"}"
		},
		{
			"PMD",
			"@ %{NO:2} %{AL:3} %{FB:3}  =%{NAME}\n"
			" %{AR1:3} %{DR1:3} %{SR1:3} %{RR1:3} %{SL1:3} %{TL1:3} %{KS1:3} %{ML1:3} %{DT1:3} %{AM1:3}\n"
			" %{AR2:3} %{DR2:3} %{SR2:3} %{RR2:3} %{SL2:3} %{TL2:3} %{KS2:3} %{ML2:3} %{DT2:3} %{AM2:3}\n"
			" %{AR3:3} %{DR3:3} %{SR3:3} %{RR3:3} %{SL3:3} %{TL3:3} %{KS3:3} %{ML3:3} %{DT3:3} %{AM3:3}\n"
			" %{AR4:3} %{DR4:3} %{SR4:3} %{RR4:3} %{SL4:3} %{TL4:3} %{KS4:3} %{ML4:3} %{DT4:3} %{AM4:3}"
		}
	};

	loadFormats();
}

ToneConverter::~ToneConverter()
{
	QSettings settings(FORMAT_PATH_, QSettings::IniFormat);

	settings.beginGroup("Input");
	settings.remove("");
	for (const auto& pair : inFormats_) {
		auto& ordrs = pair.second;
		QString str = std::accumulate(
						  ordrs.begin() + 1, ordrs.end(),
						  QString::number(static_cast<int>(ordrs.front())),
						  [](QString str, FmEnvelopeTextType type) {
					  return str + "," + QString::number(static_cast<int>(type));
	});
		settings.setValue(utf8ToQString(pair.first), str);
	}
	settings.endGroup();

	settings.beginGroup("Output");
	settings.remove("");
	for (const auto& pair : outFormats_) {
		settings.setValue(utf8ToQString(pair.first), utf8ToQString(pair.second));
	}
	settings.endGroup();
}

void ToneConverter::loadFormats()
{
	QSettings settings(FORMAT_PATH_, QSettings::IniFormat);

	settings.beginGroup("Input");
	FmInEnvelopeFormats inMap;
	for (const auto& key : settings.childKeys()) {
		auto strList = settings.value(key).toString().split(",");
		FmInEnvelopeOrders orders;
		orders.reserve(strList.size());
		std::transform(strList.begin(), strList.end(), std::back_inserter(orders),
					   [](const QString& str) { return static_cast<FmEnvelopeTextType>(str.toInt()); });
		inMap[key.toUtf8().toStdString()] = orders;
	}
	if (!inMap.empty()) inFormats_ = std::move(inMap);
	settings.endGroup();

	settings.beginGroup("Output");
	FmOutEnvelopeFormats outMap;
	for (const auto& key : settings.childKeys()) {
		outMap[key.toUtf8().toStdString()] = settings.value(key).toString().toUtf8().toStdString();
	}
	if (!outMap.empty()) outFormats_ = std::move(outMap);
	settings.endGroup();
}

QString ToneConverter::toneToText(const Tone& tone, const QString& type) const
{
	std::string out = outFormats_.at(type.toUtf8().toStdString());
	out = replaceMacroWithData(out, R"(%\{NO(:\d+)?\})", 0);
	out = replaceMacroWithData(out, R"(%\{AL(:\d+)?\})", tone.AL);
	out = replaceMacroWithData(out, R"(%\{FB(:\d+)?\})", tone.FB);
	out = replaceMacroWithData(out, R"(%\{NAME\})", tone.name);
	for (int i = 0; i < 4; ++i) {
		const Operator& op = tone.op[i];
		std::string num = std::to_string(i + 1);
		out = replaceMacroWithData(out, R"(%\{AR)" + num + R"((:\d+)?\})", op.AR);
		out = replaceMacroWithData(out, R"(%\{DR)" + num + R"((:\d+)?\})", op.DR);
		out = replaceMacroWithData(out, R"(%\{SR)" + num + R"((:\d+)?\})", op.SR);
		out = replaceMacroWithData(out, R"(%\{RR)" + num + R"((:\d+)?\})", op.RR);
		out = replaceMacroWithData(out, R"(%\{SL)" + num + R"((:\d+)?\})", op.SL);
		out = replaceMacroWithData(out, R"(%\{TL)" + num + R"((:\d+)?\})", op.TL);
		out = replaceMacroWithData(out, R"(%\{KS)" + num + R"((:\d+)?\})", op.KS);
		out = replaceMacroWithData(out, R"(%\{ML)" + num + R"((:\d+)?\})", op.ML);
		out = replaceMacroWithData(out, R"(%\{DT)" + num + R"((:\d+)?\})", op.DT);
		out = replaceMacroWithData(out, R"(%\{AM)" + num + R"((:\d+)?\})", op.AM);
		out = replaceMacroWithData(out, R"(%\{SSGEG)" + num + R"((:\d+)?\})", op.SSGEG);
	}

	return utf8ToQString(out);
}

FmOutEnvelopeFormats ToneConverter::getOutputFormats() const
{
	return outFormats_;
}

void ToneConverter::setOutputFormats(FmOutEnvelopeFormats list)
{
	outFormats_ = list;
}

std::string ToneConverter::replaceMacroWithData(std::string src, const std::string regex, const int value) const
{
	std::regex re(regex);
	std::smatch matches;
	std::stringstream ss;
	Manip manip;

	while (std::regex_search(src, matches, re)) {
		std::string option = matches[1].str().erase(0, 1);
		if (option.empty()) {
			ss << value;
		}
		else {
			parseMacro(option, manip);
			if (manip.isFill0) {
				ss << std::setw(manip.setw) << std::setfill('0') << value;
			}
			else {
				ss << std::setw(manip.setw) << value;
			}
		}

		src = std::regex_replace(src, re, ss.str(), std::regex_constants::format_first_only);
		manip.reset();
		ss.str("");
		ss.clear(std::stringstream::goodbit);
	}

	return src;
}

std::string ToneConverter::replaceMacroWithData(std::string src, const std::string regex, const std::string str) const
{
	return std::regex_replace(src, std::regex(regex), str);
}

void ToneConverter::parseMacro(std::string src, Manip &manip) const
{
	if (src.size() > 1 && src[0] == '0') {
		manip.isFill0 = true;
		src = src.substr(1);
	}
	else {
		manip.isFill0 = false;
	}

	manip.setw = std::stoi(src);
}

Tone* ToneConverter::textToTone(const QString& text, const QString& type) const
{
	std::string stdText = std::regex_replace(text.toUtf8().toStdString(), std::regex(R"(\D+)"), ",");
	if (stdText.front() == ',') stdText.erase(stdText.begin());
	if (stdText.back() == ',') stdText.erase(--stdText.end());

	if (stdText.empty()) return nullptr;

	std::stringstream ss(stdText);
	std::string tmp;
	std::vector<std::string> vec;
	while (std::getline(ss, tmp, ',')) {
		vec.push_back(tmp);
	}
	FmInEnvelopeOrders order = inFormats_.at(type.toUtf8().toStdString());
	if (vec.size() < order.size()) return nullptr;

	std::unique_ptr<Tone> tone = std::make_unique<Tone>();
	for (size_t i = 0; i < order.size(); ++i) {
		int v = std::stoi(vec[i]);
		switch (order[i]) {
		case FmEnvelopeTextType::Skip:	// Skip tone number
			break;
		case FmEnvelopeTextType::AL:
			if (v < 8) tone->AL = v;
			else return nullptr;
			break;
		case FmEnvelopeTextType::FB:
			if (v < 8) tone->FB = v;
			else return nullptr;
			break;
		default:
		{
			int o = (static_cast<int>(order[i]) - 3) / 11;
			switch (order[i]) {
			case FmEnvelopeTextType::AR1:
			case FmEnvelopeTextType::AR2:
			case FmEnvelopeTextType::AR3:
			case FmEnvelopeTextType::AR4:
				if (v < 32) tone->op[o].AR = v;
				else return nullptr;
				break;
			case FmEnvelopeTextType::DR1:
			case FmEnvelopeTextType::DR2:
			case FmEnvelopeTextType::DR3:
			case FmEnvelopeTextType::DR4:
				if (v < 32) tone->op[o].DR = v;
				else return nullptr;
				break;
			case FmEnvelopeTextType::SR1:
			case FmEnvelopeTextType::SR2:
			case FmEnvelopeTextType::SR3:
			case FmEnvelopeTextType::SR4:
				if (v < 32) tone->op[o].SR = v;
				else return nullptr;
				break;
			case FmEnvelopeTextType::RR1:
			case FmEnvelopeTextType::RR2:
			case FmEnvelopeTextType::RR3:
			case FmEnvelopeTextType::RR4:
				if (v < 16) tone->op[o].RR = v;
				else return nullptr;
				break;
			case FmEnvelopeTextType::SL1:
			case FmEnvelopeTextType::SL2:
			case FmEnvelopeTextType::SL3:
			case FmEnvelopeTextType::SL4:
				if (v < 16) tone->op[o].SL = v;
				else return nullptr;
				break;
			case FmEnvelopeTextType::TL1:
			case FmEnvelopeTextType::TL2:
			case FmEnvelopeTextType::TL3:
			case FmEnvelopeTextType::TL4:
				if (v < 127) tone->op[o].TL = v;
				else return nullptr;
				break;
			case FmEnvelopeTextType::KS1:
			case FmEnvelopeTextType::KS2:
			case FmEnvelopeTextType::KS3:
			case FmEnvelopeTextType::KS4:
				if (v < 4) tone->op[o].KS = v;
				else return nullptr;
				break;
			case FmEnvelopeTextType::ML1:
			case FmEnvelopeTextType::ML2:
			case FmEnvelopeTextType::ML3:
			case FmEnvelopeTextType::ML4:
				if (v < 16) tone->op[o].ML = v;
				else return nullptr;
				break;
			case FmEnvelopeTextType::DT1:
			case FmEnvelopeTextType::DT2:
			case FmEnvelopeTextType::DT3:
			case FmEnvelopeTextType::DT4:
				if (v < 8) tone->op[o].DT = v;
				else return nullptr;
				break;
			case FmEnvelopeTextType::AM1:
			case FmEnvelopeTextType::AM2:
			case FmEnvelopeTextType::AM3:
			case FmEnvelopeTextType::AM4:
				if (v < 2) tone->op[o].AM = v;
				else return nullptr;
				break;
			case FmEnvelopeTextType::SSGEG1:
			case FmEnvelopeTextType::SSGEG2:
			case FmEnvelopeTextType::SSGEG3:
			case FmEnvelopeTextType::SSGEG4:
				if (v < 16) tone->op[o].SSGEG = v;
				else return nullptr;
				break;
			default:
				break;
			}
			break;
		}
		}
	}
	tone->name = "";

	return tone.release();
}

FmInEnvelopeFormats ToneConverter::getInputFormats() const
{
	return inFormats_;
}

void ToneConverter::setInputFormats(FmInEnvelopeFormats list)
{
	inFormats_ = list;
}
