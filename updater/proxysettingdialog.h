#ifndef PROXYSETTINGDIALOG_H
#define PROXYSETTINGDIALOG_H

#include <QDialog>

namespace Ui {
class ProxySettingDialog;
}

class ProxySettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProxySettingDialog(QWidget *parent = nullptr);
    ~ProxySettingDialog();

private:
    Ui::ProxySettingDialog *ui;
};

#endif // PROXYSETTINGDIALOG_H
