#ifndef READTEXTDIALOG_HPP
#define READTEXTDIALOG_HPP

#include <QDialog>
#include <QString>

namespace Ui {
	class ReadTextDialog;
}

class ReadTextDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ReadTextDialog(QWidget *parent = nullptr);
	~ReadTextDialog();
	QString text() const;

private:
	Ui::ReadTextDialog *ui;
};

#endif // READTEXTDIALOG_HPP
