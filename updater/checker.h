#ifndef CHECKER_H
#define CHECKER_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QByteArray>
#include <QEventLoop>
#include <QNetworkProxy>

class Checker : public QObject
{
    Q_OBJECT
public:
    enum Status{
        ALPHA = 1,
        BETA = 2,
        RELEASE = 4,
        LTS = 8
    };

private:
    QNetworkAccessManager manager;
    QNetworkRequest request;
    QNetworkReply *reply;
    QByteArray tmpJson;
    const int major;
    const int minor;
    const int fix;
    int checkingStatus;

public:
    Checker(int major, int minor, int fix);
    void startChecking_Async(int status = Checker::Status::ALPHA | Checker::Status::BETA | Checker::Status::RELEASE | Checker::Status::LTS, const QNetworkProxy &proxy = QNetworkProxy());

private:
    bool updatable(int mf_major, int mf_minor, int mf_fix);
    bool higherThanNow(int new_major, int new_minor, int new_fix);
    bool statusMatch(const QString &status_str);
    void clear();

private slots:
    void checkProgressing(qint64 now, qint64 max);
    void checkFinished();
    void checkReadyRead();

signals:
    void progressing(qint64 now, qint64 max);
    void succeeded(int major, int minor, int fix, Checker::Status status, const QUrl &url, const QString &SHA512);
    void failed(const QString &reason);
};

#endif // CHECKER_H
