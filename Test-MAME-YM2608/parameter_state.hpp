#pragma once

class ParameterState
{
public:
    enum Parameter
    {
        DT, ML,
        TL,
        KS, AR,
        AM, DR,
        SR,
        SL, RR,
        FB, AL
    };

    ParameterState(Parameter parameter, int value = 0);
    Parameter getParametor() const;
    int getValue() const;
    void setValue(int value);

private:
    Parameter parameter_;
    int value_;
};
