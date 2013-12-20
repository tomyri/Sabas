#include "sabasbook.h"
#include <QDir>
#include <QMediaPlaylist>
#include <QMediaPlayer>
#include <QDebug>

SabasBook::SabasBook(const QString &folder, QObject *parent) :
    QObject(parent),
    m_playlist(new QMediaPlaylist(this)),
    m_rootPath(folder),
    m_currentIndex(0),
    m_lastIndex(0),
    m_lastTrackPosition(0)
{
    connect(m_playlist, &QMediaPlaylist::currentIndexChanged, [=](int index){
        m_currentIndex = index;
        emit currentIndexChanged(index);
    });
}

void SabasBook::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        emit nameChanged(name);
    }
}

QString SabasBook::name() const
{
    return m_name;
}
qint64 SabasBook::lastTrackPosition() const
{
    return m_lastTrackPosition;
}

void SabasBook::scanFolder(const QString &folder)
{
    QDir dir(folder);
    QStringList coverFilter;
    coverFilter << "*.jpg" << "*.jpeg" << "*.png" << "*.gif";
    QStringList coverFiles = dir.entryList(coverFilter, QDir::Files);
    if (!coverFiles.isEmpty()) {
        setCoverPath(dir.absolutePath() + "/" + coverFiles.first());
    }
    QStringList folders = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    if (!folders.isEmpty()) {
        foreach (const QString &f, folders) {
            scanFolder(dir.absolutePath() + "/" + f);
        }
    } else {
        QStringList filters;
        filters.append("*.mp3");
        QStringList files = dir.entryList(filters, QDir::Files);
        foreach (const QString &f, files) {
            m_playlist->addMedia(QMediaContent(QUrl::fromLocalFile(dir.absolutePath() + "/" + f)));
            qDebug() << "Added " << QUrl::fromLocalFile(dir.absolutePath() + "/" + f);
        }
    }
}
int SabasBook::lastIndex() const
{
    return m_lastIndex;
}

void SabasBook::setLastIndex(int lastIndex)
{
    m_lastIndex = lastIndex;
}


QString SabasBook::rootPath() const
{
    return m_rootPath;
}

void SabasBook::setRootPath(const QString &rootPath)
{
    m_rootPath = rootPath;
}
QMediaPlaylist *SabasBook::playlist() const
{
    return m_playlist;
}

QStringList SabasBook::playListStrings() const
{
    QStringList ret;
    for (int k = 0; k < m_playlist->mediaCount(); ++k) {
        ret << m_playlist->media(k).canonicalUrl().toLocalFile();
    }
    return ret;
}

void SabasBook::setPlaylist(const QStringList &tracks)
{
    foreach (const QString &f, tracks) {
        m_playlist->addMedia(QMediaContent(QUrl::fromLocalFile(f)));
    }
}

bool SabasBook::findMedia()
{
    if (m_rootPath.isEmpty())
        return false;
    scanFolder(m_rootPath);
    return !m_playlist->isEmpty();
}

void SabasBook::next()
{
    m_playlist->next();
}

void SabasBook::previous()
{
    m_playlist->previous();
}

void SabasBook::setCoverPath(QString path)
{
    if (m_coverPath != path) {
        m_coverPath = path;
        emit coverPathChanged(path);
    }
}

int SabasBook::currentIndex() const
{
    return m_currentIndex;
}

void SabasBook::setCurrentIndex(int index)
{
    qDebug() << "setting playlists current index to " << index;
    m_playlist->setCurrentIndex(index);
}

QString SabasBook::coverPath() const
{
    return m_coverPath;
}

void SabasBook::setLastTrackPosition(qint64 position)
{
    qDebug() << "last position to " << position;
    m_lastTrackPosition = position;
}
