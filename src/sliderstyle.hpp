#ifndef SLIDERSTYLE_HPP
#define SLIDERSTYLE_HPP

#include <QProxyStyle>

class SliderStyle : public QProxyStyle
{
public:
	virtual int styleHint (StyleHint hint, const QStyleOption* option = nullptr,
						   const QWidget* widget = nullptr, QStyleHintReturn* returnData = nullptr) const;
};

#endif // SLIDERSTYLE_HPP
