#ifndef NAMEDIALOG_H
#define NAMEDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class NameDialog;
}

class NameDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NameDialog(const QString& name, QWidget* parent = nullptr);
    ~NameDialog();
    QString toneName();

private:
    Ui::NameDialog *ui;
};

#endif // NAMEDIALOG_H
