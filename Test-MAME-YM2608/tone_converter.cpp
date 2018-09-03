#include "tone_converter.hpp"
#include <fstream>
#include <sstream>
#include <regex>
#include <iomanip>

ToneConverter::ToneConverter() {}

ToneConverter::~ToneConverter()
{
	std::ofstream ofs("format.conf", std::ios::out);
	if (ofs) {
		ofs << format_;
	}
}

void ToneConverter::loadFormat(std::string path)
{
    std::ifstream ifs(path, std::ios::in);
    if (ifs) {
        format_ = std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
    }
    else {
        format_ = "@ %{NO:2} %{AL:3} %{FB:3}  =%{NAME}\n"
                  " %{AR1:3} %{DR1:3} %{SR1:3} %{RR1:3} %{SL1:3} %{TL1:3} %{KS1:3} %{ML1:3} %{DT1:3} %{AM1:3}\n"
                  " %{AR2:3} %{DR2:3} %{SR2:3} %{RR2:3} %{SL2:3} %{TL2:3} %{KS2:3} %{ML2:3} %{DT2:3} %{AM2:3}\n"
                  " %{AR3:3} %{DR3:3} %{SR3:3} %{RR3:3} %{SL3:3} %{TL3:3} %{KS3:3} %{ML3:3} %{DT3:3} %{AM3:3}\n"
                  " %{AR4:3} %{DR4:3} %{SR4:3} %{RR4:3} %{SL4:3} %{TL4:3} %{KS4:3} %{ML4:3} %{DT4:3} %{AM4:3}";
        std::ofstream ofs("format.conf", std::ios::out);
        if (ofs) {
            ofs << format_;
        }
    }
}

std::string ToneConverter::toneToText(const Tone* tone)
{
    std::string out = format_;
	out = replaceMacroWithData(out, R"(%\{NO:(\d+)\})", 0);
	out = replaceMacroWithData(out, R"(%\{AL:(\d+)\})", tone->AL);
	out = replaceMacroWithData(out, R"(%\{FB:(\d+)\})", tone->FB);
	out = replaceMacroWithData(out, R"(%\{NAME\})", tone->name);
    for (int i = 0; i < 4; ++i) {
        const Operator& op = tone->op[i];
        std::string num = std::to_string(i + 1);
		out = replaceMacroWithData(out, R"(%\{AR)" + num + R"(:(\d+)\})", op.AR);
		out = replaceMacroWithData(out, R"(%\{DR)" + num + R"(:(\d+)\})", op.DR);
		out = replaceMacroWithData(out, R"(%\{SR)" + num + R"(:(\d+)\})", op.SR);
		out = replaceMacroWithData(out, R"(%\{RR)" + num + R"(:(\d+)\})", op.RR);
		out = replaceMacroWithData(out, R"(%\{SL)" + num + R"(:(\d+)\})", op.SL);
		out = replaceMacroWithData(out, R"(%\{TL)" + num + R"(:(\d+)\})", op.TL);
		out = replaceMacroWithData(out, R"(%\{KS)" + num + R"(:(\d+)\})", op.KS);
		out = replaceMacroWithData(out, R"(%\{ML)" + num + R"(:(\d+)\})", op.ML);
		out = replaceMacroWithData(out, R"(%\{DT)" + num + R"(:(\d+)\})", op.DT);
		out = replaceMacroWithData(out, R"(%\{AM)" + num + R"(:(\d+)\})", op.AM);
    }

    return out;
}

std::string ToneConverter::getOutputFormat() const
{
	return format_;
}

void ToneConverter::setOutputFormat(std::string str)
{
	format_ = str;
}

std::string ToneConverter::replaceMacroWithData(std::string src, const std::string regex, const int value)
{
    std::regex re(regex);
    std::smatch matches;
    std::stringstream ss;
    Manip manip;

    while (std::regex_search(src, matches, re)) {
		parseMacro(matches[1].str(), manip);
        if (manip.isFill0) {
            ss << std::setw(manip.setw) << std::setfill('0') << value;
        }
        else {
            ss << std::setw(manip.setw) << value;
        }

        src = std::regex_replace(src, re, ss.str(), std::regex_constants::format_first_only);
        manip.reset();
        ss.str("");
        ss.clear(std::stringstream::goodbit);
    }

    return src;
}

std::string ToneConverter::replaceMacroWithData(std::string src, const std::string regex, const std::string str)
{
    return std::regex_replace(src, std::regex(regex), str);
}

void ToneConverter::parseMacro(std::string src, Manip &manip)
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

std::unique_ptr<Tone> ToneConverter::textToTone(std::string text, std::vector<int> order)
{
	text = std::regex_replace(text, std::regex(R"(\D+)"), ",");
	if (text.front() == ',') text.erase(text.begin());
	if (text.back() == ',') text.erase(--text.end());

	if (text.empty()) return std::unique_ptr<Tone>();

	std::stringstream ss(text);
	std::string tmp;
	std::vector<std::string> vec;
	while (std::getline(ss, tmp, ',')) {
		vec.push_back(tmp);
	}
	if (vec.size() >= order.size()) {
		std::unique_ptr<Tone> tone = std::make_unique<Tone>();
		for (size_t i = 0; i < order.size(); ++i) {
			int v = std::stoi(vec[i]);
			switch (order[i]) {
			case 0:	// Skip tone number
				break;
			case 1:
				if (v < 8) tone->AL = v;
				else return std::unique_ptr<Tone>();
				break;
			case 2:
				if (v < 8) tone->FB = v;
				else return std::unique_ptr<Tone>();
				break;
			default:
			{
				int t = order[i] - 3;
				int o = t / 10;
				switch (t % 10) {
				case 0:
					if (v < 32) tone->op[o].AR = v;
					else return std::unique_ptr<Tone>();
					break;
				case 1:
					if (v < 32) tone->op[o].DR = v;
					else return std::unique_ptr<Tone>();
					break;
				case 2:
					if (v < 32) tone->op[o].SR = v;
					else return std::unique_ptr<Tone>();
					break;
				case 3:
					if (v < 16) tone->op[o].RR = v;
					else return std::unique_ptr<Tone>();
					break;
				case 4:
					if (v < 16) tone->op[o].SL = v;
					else return std::unique_ptr<Tone>();
					break;
				case 5:
					if (v < 127) tone->op[o].TL = v;
					else return std::unique_ptr<Tone>();
					break;
				case 6:
					if (v < 4) tone->op[o].KS = v;
					else return std::unique_ptr<Tone>();
					break;
				case 7:
					if (v < 16) tone->op[o].ML = v;
					else return std::unique_ptr<Tone>();
					break;
				case 8:
					if (v < 8) tone->op[o].DT = v;
					else return std::unique_ptr<Tone>();
					break;
				case 9:
					if (v < 2) tone->op[o].AM = v;
					else return std::unique_ptr<Tone>();
					break;
				}
				break;
			}
			}
		}
		tone->name = "";
		return tone;
	}
	else {
		return std::unique_ptr<Tone>();
	}
}
