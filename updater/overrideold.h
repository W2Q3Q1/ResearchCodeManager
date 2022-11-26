#ifndef OVERRIDEOLD_H
#define OVERRIDEOLD_H

#include <QMainWindow>

namespace Ui {
class OverrideOld;
}

class OverrideOld : public QMainWindow
{
    Q_OBJECT

public:
    explicit OverrideOld(int old_major, int old_minor, int old_fix, QWidget *parent = nullptr);
    ~OverrideOld();

private:
    Ui::OverrideOld *ui;
};

#endif // OVERRIDEOLD_H
