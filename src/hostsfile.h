#ifndef HOSTSFILE_H
#define HOSTSFILE_H

#include <QObject>

class HostsFile : public QObject
{
    Q_OBJECT
public:
    explicit HostsFile(QObject *parent = 0);

signals:

public slots:
    int sudo(const QString &password, const QStringList &command);
    bool backupHosts(const QString &password);
    QString restoreHosts(const QString &password);
    QString updateHosts(const QString &password);

};

#endif // HOSTSFILE_H
