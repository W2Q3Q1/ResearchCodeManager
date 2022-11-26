#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QFile>
#include <QNetworkProxy>

class Downloader : public QObject
{
    Q_OBJECT

private:
    QNetworkAccessManager manager;
    QNetworkRequest request;
    QNetworkReply *reply;
    QFile tmpFile;
    QString tmpSHA512;

public:
    Downloader();
    void startDownload_Async(const QUrl &url, const QString &file_path, const QString &sha512 = QString(), const QNetworkProxy &proxy = QNetworkProxy());

private:
    void clear();

signals:
    void progressing(qint64 now, qint64 max);
    void succeeded();
    void failed(const QString &reason);

private slots:
    void downloadProgressing(qint64 now, qint64 max);
    void downloadFinished();
    void downloadReadyRead();
};

#endif // DOWNLOADER_H
