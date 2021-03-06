#include "sabasbook.h"
#include <QDir>
#include <QMediaPlaylist>
#include <QMediaPlayer>
#include <QFileSystemWatcher>
#include <QDebug>

bool naturalSort(const QString &s1, const QString &s2);

SabasBook::SabasBook(const QString &folder, QObject *parent) :
    QObject(parent),
    m_playlist(new QMediaPlaylist(this)),
    m_rootPath(folder),
    m_lastTrackPosition(0),
    m_playbackRate(1),
    m_fsw(new QFileSystemWatcher(this))
{
    connect(m_playlist, &QMediaPlaylist::currentIndexChanged, this, &SabasBook::currentIndexChanged);
    connect(m_fsw, &QFileSystemWatcher::directoryChanged,
            this, &SabasBook::locateMedia, Qt::QueuedConnection);
}

SabasBook::~SabasBook()
{
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
    m_fsw->addPath(folder);
    QDir dir(folder);
    QStringList coverFilter;
    coverFilter << "*.jpg" << "*.jpeg" << "*.png" << "*.gif";
    QStringList coverFiles = dir.entryList(coverFilter, QDir::Files);
    if (!coverFiles.isEmpty()) {
        setCoverPath("file://" + dir.absolutePath() + "/" + coverFiles.first());
        foreach (const QString &c, coverFiles) {
            if (!m_localCovers.contains("file://" + dir.absolutePath() + "/" + c)) {
                m_localCovers.append("file://" + dir.absolutePath() + "/" + c);
                emit possibleCoversChanged(possibleCovers());
            }
        }
    }
    QStringList filters;
    filters << "*.mp3" << "*.ogg" << "*.m4a" << "*.flac" << "*.acc" << "*.mp4" << "*.3gp";
    QStringList files = dir.entryList(filters, QDir::Files);
    qSort(files.begin(), files.end(), naturalSort);
    foreach (const QString &f, files) {
        m_playlist->addMedia(QMediaContent(QUrl::fromLocalFile(dir.absolutePath() + "/" + f)));
        emit tracksChanged(tracks());
        emit trackCountChanged(trackCount());
    }
    QStringList folders = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    if (!folders.isEmpty()) {
        qSort(folders.begin(), folders.end(), naturalSort);
        foreach (const QString &f, folders) {
            scanFolder(dir.absolutePath() + "/" + f);
        }
    }
}
QStringList SabasBook::localCovers() const
{
    return m_localCovers;
}

void SabasBook::setLocalCovers(const QStringList &localCovers)
{
    m_localCovers.clear();
    foreach (const QString &cover, localCovers) {
        if (QFile::exists(cover))
            m_localCovers.append(cover);
        else
            qDebug() << cover << "don't exists anymore";
    }
}

QStringList SabasBook::possibleCovers() const
{
    return m_localCovers + m_onlineCovers;
}

QStringList SabasBook::tracks() const
{
    QStringList l;
    for (int k = 0; k < m_playlist->mediaCount(); ++k) {
        l <<  m_playlist->media(k).canonicalUrl().toLocalFile().remove(m_rootPath + "/");
    }
    return l;
}

int SabasBook::trackCount() const
{
    return m_playlist->mediaCount();
}

void SabasBook::setPlaybackRate(qreal rate)
{
    m_playbackRate = rate;
}

qreal SabasBook::playbackRate() const
{
    return m_playbackRate;
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
#ifdef SAVE_PLAYLIST
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
#endif

void SabasBook::locateMedia()
{
    if (!m_playlist->isEmpty()) {
        m_playlist->clear();
        emit tracksChanged(tracks());
        emit trackCountChanged(0);
    }
    if (m_rootPath.isEmpty())
        return;
    scanFolder(m_rootPath);
}

void SabasBook::next()
{
    m_playlist->next();
}

void SabasBook::previous()
{
    m_playlist->previous();
}

void SabasBook::setCoverPath(const QString &path)
{
    if (!m_localCovers.contains(path)) {
        m_localCovers.append(path);
        emit possibleCoversChanged(possibleCovers());
    }
    if (m_coverPath != path) {
        m_coverPath = path;
        emit coverPathChanged(path);
    }
}

void SabasBook::setOnlineCovers(const QStringList &coverUrls)
{
    if (m_onlineCovers != coverUrls) {
        m_onlineCovers = coverUrls;
        emit possibleCoversChanged(possibleCovers());
    }
}

int SabasBook::currentIndex() const
{
    return m_playlist->currentIndex();
}

void SabasBook::setCurrentIndex(int index)
{
    m_playlist->setCurrentIndex(index);
}

QString SabasBook::coverPath() const
{
    return m_coverPath;
}

void SabasBook::setLastTrackPosition(qint64 position)
{
    m_lastTrackPosition = position;
}

bool naturalSort(const QString &s1, const QString &s2)
{
    if (s1.isEmpty() || s2.isEmpty())
        return s1 < s2;
    // ignore common prefix
    int i = 0;
    while (s1.at(i).toLower() == s2.at(i).toLower()) {
        ++i;
        //common until other ends, return smaller
        if ((i == s1.length()) || (i == s2.length())) {
            return s1.length() < s2.length();
        }
    }
    //If not number return native comparator
    if(!s1.at(i).isNumber() || !s2.at(i).isNumber()) {
        //make sure that 1_... < 12_...
        if(s1.at(i).isNumber())
            return false;
        if(s2.at(i).isNumber())
            return true;
        return QString::compare(s1, s2, Qt::CaseSensitive) < 0;
    }
    QString n;
    int k = i;
    while ((k >= 0) && (s1.at(k).isNumber())) {
        n = s1.at(k) + n;
        --k;
    }
    // get relevant number strings
    k = i;
    QString n1;
    while ((k < s1.length()) && (s1.at(k).isNumber()))
        n1 += s1.at(k++);
    k = i;
    QString n2;
    while ((k < s2.length()) && (s2.at(k).isNumber()))
        n2 += s2.at(k++);

    // got two numbers to compare?
    if (!n1.isEmpty() && !n2.isEmpty())
        return (n + n1).toInt() < (n + n2).toInt();
    // not a number has to win over a number.. number could have ended earlier... same prefix..
    if (!n1.isEmpty())
        return false;
    if (!n2.isEmpty())
        return true;
    return s1.at(i) < s2.at(i);
}
