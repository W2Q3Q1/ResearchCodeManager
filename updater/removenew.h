#ifndef REMOVENEW_H
#define REMOVENEW_H

#include <QMainWindow>

namespace Ui {
class RemoveNew;
}

class RemoveNew : public QMainWindow
{
    Q_OBJECT

public:
    explicit RemoveNew(QWidget *parent = nullptr);
    ~RemoveNew();

private:
    Ui::RemoveNew *ui;
};

#endif // REMOVENEW_H
