#ifndef CHECKANDDOWNLOAD_H
#define CHECKANDDOWNLOAD_H

#include <QMainWindow>
#include "checker.h"
#include "downloader.h"
#include "unzipper.h"
#include <QNetworkProxy>

namespace Ui {
class CheckAndDownload;
}

class CheckAndDownload : public QMainWindow
{
    Q_OBJECT

public:
    explicit CheckAndDownload(int now_major, int now_minor, int now_fix, QWidget *parent = nullptr);
    ~CheckAndDownload();

private slots:
    void refreshClicked();
    void checkerProgressing(qint64 now, qint64 max);
    void checkerSucceeded(int major, int minor, int fix, Checker::Status status, const QUrl &url, const QString &SHA512);
    void checkerFailed(const QString &reason);
    void downloaderProgressing(qint64 now, qint64 max);
    void downloaderSucceeded();
    void downloaderFailed(const QString &reason);
    void unzipperSucceeded();
    void unzipperFailed(const QString &reason);

private:
    void setOnProcessing(bool on_processing);
    void closeEvent(QCloseEvent *event) override;
    void connectChecker();
    void disconnectChecker();
    void connectDownloader();
    void disconnectDownloader();
    void connectUnzipper();
    void disconnectUnzipper();

private:
    Ui::CheckAndDownload *ui;
    Checker updateChecker;
    Downloader updateDownloader;
    UnZipper updateUnzipper;
    const int nowMajor;
    const int nowMinor;
    const int nowFix;
    const QString tmpFilePath;
    bool onProcessing;
    QNetworkProxy proxy;
    int updateStatus;
};

#endif // CHECKANDDOWNLOAD_H
