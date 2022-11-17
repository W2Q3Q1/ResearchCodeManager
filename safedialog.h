#ifndef SAFEDIALOG_H
#define SAFEDIALOG_H

#include <QDialog>
#include <QPushButton>

namespace Ui {
    class SafeDialog;
}

class SafeDialog : public QDialog
{
        Q_OBJECT

    public:
        enum SafeDialogType{SAFE_CONFIRM, SAFE_CANCEL};
        explicit SafeDialog(const QString &message = QString(), QWidget *parent = nullptr, SafeDialogType dialog_type = SafeDialogType::SAFE_CONFIRM);
        ~SafeDialog();

    private:
        Ui::SafeDialog *ui;
};

#endif // SAFEDIALOG_H
