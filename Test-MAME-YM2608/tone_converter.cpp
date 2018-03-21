#include "tone_converter.hpp"
#include <fstream>
#include <sstream>
#include <regex>
#include <iomanip>

ToneConverter::ToneConverter() {}

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

std::string ToneConverter::convert(const Tone* tone)
{
    std::string out = format_;
    out = replace(out, R"(%\{NO:(\d+)\})", 0);
    out = replace(out, R"(%\{AL:(\d+)\})", tone->AL);
    out = replace(out, R"(%\{FB:(\d+)\})", tone->FB);
    out = replace(out, R"(%\{NAME\})", tone->name);
    for (int i = 0; i < 4; ++i) {
        const Operator& op = tone->op[i];
        std::string num = std::to_string(i + 1);
        out = replace(out, R"(%\{AR)" + num + R"(:(\d+)\})", op.AR);
        out = replace(out, R"(%\{DR)" + num + R"(:(\d+)\})", op.DR);
        out = replace(out, R"(%\{SR)" + num + R"(:(\d+)\})", op.SR);
        out = replace(out, R"(%\{RR)" + num + R"(:(\d+)\})", op.RR);
        out = replace(out, R"(%\{SL)" + num + R"(:(\d+)\})", op.SL);
        out = replace(out, R"(%\{TL)" + num + R"(:(\d+)\})", op.TL);
        out = replace(out, R"(%\{KS)" + num + R"(:(\d+)\})", op.KS);
        out = replace(out, R"(%\{ML)" + num + R"(:(\d+)\})", op.ML);
        out = replace(out, R"(%\{DT)" + num + R"(:(\d+)\})", op.DT);
        out = replace(out, R"(%\{AM)" + num + R"(:(\d+)\})", op.AM);
    }

    return out;
}

std::string ToneConverter::replace(std::string src, const std::string regex, const int value)
{
    std::regex re(regex);
    std::smatch matches;
    std::stringstream ss;
    Manip manip;

    while (std::regex_search(src, matches, re)) {
        parse(matches[1].str(), manip);
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

std::string ToneConverter::replace(std::string src, const std::string regex, const std::string str)
{
    return std::regex_replace(src, std::regex(regex), str);
}

void ToneConverter::parse(std::string src, Manip &manip)
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
