#include <QDebug>
#include <QProcess>
#include <QFileInfo>

#include "hostsfile.h"

HostsFile::HostsFile(QObject *parent) :
    QObject(parent)
{
}

int HostsFile::sudo(const QString &password, const QStringList &command)
{
    QProcess sudo;
    sudo.start("devel-su", command);

    if( !sudo.waitForStarted() )
    {
        qCritical() << "cannot start devel-su";
        return 1;
    }

    sudo.write((password + "\n").toLatin1());
    sudo.waitForFinished();

    if( !sudo.exitCode() == 0 )
    {
        qDebug() << "Command failed:"
                 << sudo.readAllStandardError();
    }

    return sudo.exitCode();
}

bool HostsFile::backupHosts(const QString &password)
{
    QFileInfo *origin = new QFileInfo("/etc/hosts.origin");

    if( origin->exists() )
    {
        qDebug() << "Backup file" << origin->filePath() << "already exists:"
                 << origin->size() << "bytes.";
    }
    else
    {
        qDebug() << "Backup /etc/hosts to" << origin->filePath();
        QStringList backup_command;
        backup_command << "cp" << "/etc/hosts" << origin->filePath();
        if( !this->sudo(password, backup_command) == 0 )
        {
            qCritical() << "Backup failed";
            return false;
        }
    }

    return true;
}

QString HostsFile::restoreHosts(const QString &password)
{
    QFileInfo *origin = new QFileInfo("/etc/hosts.origin");

    if( !origin->exists() )
    {
        qCritical() << "Original file" << origin->filePath()
                    << "does not exist!";
        return QString("No backup file found.");
    }
    else
    {
        QFileInfo *hosts = new QFileInfo("/etc/hosts");
        qDebug() << "Hosts file size:" << hosts->size();

        QStringList restore_command;
        restore_command << "mv" << "-f" << origin->filePath() << "/etc/hosts";
        if( !this->sudo(password, restore_command) == 0 )
        {
            qCritical() << "Restoration failed";
            return QString("Could not restore original hosts file. Please check that you're using the developer mode and the password is correct.");
        }

        hosts->refresh();
        qDebug() << "Result file size:" << hosts->size();
    }

    return QString("Hosts file restored successfully.");
}

QString HostsFile::updateHosts(const QString &password)
{
    if( !this->backupHosts(password) )
        return QString("Hosts file can't be backed up. Please check that you're using the developer mode and the password is correct.");

    QFileInfo *hosts = new QFileInfo("/etc/hosts");
    qDebug() << "Original file size:" << hosts->size();

    QStringList update_command;
    update_command << "sh" << "-c"
                   << "curl -s https://adaway.org/hosts.txt http://winhelp2002.mvps.org/hosts.txt http://hosts-file.net/ad_servers.asp 'http://pgl.yoyo.org/adservers/serverlist.php?hostformat=hosts&showintro=0&mimetype=plaintext' > /etc/hosts";
    if( !this->sudo(password, update_command) == 0 )
    {
        qCritical() << "can not update /etc/hosts";
        return QString("Couldn't update hosts file. Please check that you're using the developer mode and the password is correct. You may need to reset the hosts file if you encounter issue while connecting to websites.");
    }

    hosts->refresh();
    qDebug() << "Result file size:" << hosts->size();

    return QString("Hosts file updated successfully.");
}
