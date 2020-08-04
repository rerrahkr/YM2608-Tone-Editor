#ifndef OPERATOR_SLIDERS_H
#define OPERATOR_SLIDERS_H

#include <QWidget>
#include "parameter_state.hpp"

namespace Ui {
	class OperatorSliders;
}

class OperatorSliders : public QWidget
{
	Q_OBJECT

public:
	explicit OperatorSliders(QWidget *parent = 0, int number = 0);
	~OperatorSliders();

	int operatorNumber() const;
	void setOperatorNumber(int number);
	int parameterValue(ParameterState::Parameter param) const;
	void setParameterValue(ParameterState::Parameter param, int value);

signals:
	void parameterChanged(int op, const ParameterState& state);

private:
	Ui::OperatorSliders *ui;

	int number_;

	ParameterState arState_;
	ParameterState drState_;
	ParameterState srState_;
	ParameterState rrState_;
	ParameterState slState_;
	ParameterState tlState_;
	ParameterState ksState_;
	ParameterState mlState_;
	ParameterState dtState_;
	ParameterState amState_;

	void setStateValue(ParameterState& state, int value);

private slots:
	void onARChanged(int value);
	void onDRChanged(int value);
	void onSRChanged(int value);
	void onRRChanged(int value);
	void onSLChanged(int value);
	void onTLChanged(int value);
	void onKSChanged(int value);
	void onMLChanged(int value);
	void onDTChanged(int value);
	void onAMChanged(int value);
};

#endif // OPERATOR_SLIDERS_H
