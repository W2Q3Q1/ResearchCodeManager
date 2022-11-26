#ifndef UNZIPPER_H
#define UNZIPPER_H

#include <QObject>
#include <QProcess>

class UnZipper : public QObject
{
    Q_OBJECT
private:
    QProcess process;
    QString processOutput;

public:
    UnZipper();
    void startUnzipping_Async(const QString &zip_path);

private:
    void clear();

private slots:
    void unzipFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void unzipReadyRead();


signals:
    void succeeded();
    void failed(const QString &reason);

};

#endif // UNZIPPER_H
