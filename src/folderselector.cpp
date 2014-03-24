#include "folderselector.h"

FolderSelector::FolderSelector(const QString &dir, QObject *parent) :
    QObject(parent),
    m_currentDir(dir)
{
    connect(this, &FolderSelector::currentDirChanged,
            this, &FolderSelector::updateFolderList);
    updateFolderList();
}

QString FolderSelector::currentDir() const
{
    return m_currentDir;
}

QStringList FolderSelector::folders() const
{
    return m_folders;
}

void FolderSelector::setCurrentDir(const QString &dir)
{
    if (m_currentDir != dir) {
        m_currentDir = dir;
        emit currentDirChanged(dir);
    }
}

void FolderSelector::goTo(const QString &dir)
{
    setCurrentDir(QDir(m_currentDir + "/" + dir).absolutePath());
}

void FolderSelector::setFolders(const QStringList &folders)
{
    if (m_folders != folders) {
        m_folders = folders;
        emit foldersChanged(folders);
    }
}

void FolderSelector::updateFolderList()
{
    QDir dir(m_currentDir);
    setFolders(dir.entryList(QDir::Dirs | QDir::NoDot));
}
