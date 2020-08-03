#ifndef FMENVELOPEFORMATDIALOG_HPP
#define FMENVELOPEFORMATDIALOG_HPP

#include <QDialog>
#include <QString>

namespace Ui {
	class FmEnvelopeFormatDialog;
}

class FmEnvelopeFormatDialog : public QDialog
{
	Q_OBJECT

public:
	explicit FmEnvelopeFormatDialog(const QString& getText, QWidget *parent = nullptr);
	~FmEnvelopeFormatDialog();

	QString getText() const;

private:
	Ui::FmEnvelopeFormatDialog *ui;
};

#endif // FMENVELOPEFORMATDIALOG_HPP
