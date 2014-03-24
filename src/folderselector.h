#ifndef FOLDERSELECTOR_H
#define FOLDERSELECTOR_H

#include <QObject>
#include <QStringList>
#include <QDir>

class FolderSelector : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentDir READ currentDir WRITE setCurrentDir NOTIFY currentDirChanged)
    Q_PROPERTY(QStringList folders READ folders NOTIFY foldersChanged)

public:
    explicit FolderSelector(const QString &dir = QDir::homePath(), QObject *parent = 0);
    QString currentDir() const;
    QStringList folders() const;

signals:
    void currentDirChanged(const QString &dir);
    void foldersChanged(const QStringList &folders);

public slots:
    void setCurrentDir(const QString &dir);
    void goTo(const QString &dir);

private:
    void setFolders(const QStringList &folders);
    void updateFolderList();
    QString m_currentDir;
    QStringList m_folders;
};

#endif // FOLDERSELECTOR_H
