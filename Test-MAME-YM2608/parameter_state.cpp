#include "parameter_state.hpp"

ParameterState::ParameterState(ParameterState::Parameter parameter, int value) :
    parameter_(parameter),
    value_(value)
{
}

ParameterState::Parameter ParameterState::getParametor() const
{
    return parameter_;
}

int ParameterState::getValue() const
{
    return value_;
}

void ParameterState::setValue(int value)
{
    value_ = value;
}
