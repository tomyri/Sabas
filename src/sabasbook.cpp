#include "sabasbook.h"
#include <QDir>
#include <QMediaPlaylist>
#include <QMediaPlayer>
#include <QDebug>

bool naturalSort(const QString &s1, const QString &s2);

SabasBook::SabasBook(const QString &folder, QObject *parent) :
    QObject(parent),
    m_playlist(new QMediaPlaylist(this)),
    m_rootPath(folder),
    m_currentIndex(0),
    m_lastIndex(0),
    m_lastTrackPosition(0),
    m_playbackRate(1)
{
    connect(m_playlist, &QMediaPlaylist::currentIndexChanged, [=](int index){
        m_currentIndex = index;
        emit currentIndexChanged(index);
    });
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
    QDir dir(folder);
    QStringList coverFilter;
    coverFilter << "*.jpg" << "*.jpeg" << "*.png" << "*.gif";
    QStringList coverFiles = dir.entryList(coverFilter, QDir::Files);
    if (!coverFiles.isEmpty()) {
        setCoverPath(dir.absolutePath() + "/" + coverFiles.first());
    }
    QStringList folders = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    if (!folders.isEmpty()) {
        qSort(folders.begin(), folders.end(), naturalSort);
        foreach (const QString &f, folders) {
            scanFolder(dir.absolutePath() + "/" + f);
        }
    } else {
        QStringList filters;
        filters << "*.mp3" << "*.ogg" << "*.m4a" << "*.flac" << "*.acc" << "*.mp4" << "*.3gp";
        QStringList files = dir.entryList(filters, QDir::Files);
        qSort(files.begin(), files.end(), naturalSort);
        foreach (const QString &f, files) {
            m_playlist->addMedia(QMediaContent(QUrl::fromLocalFile(dir.absolutePath() + "/" + f)));
        }
    }
}
QStringList SabasBook::possibleCovers() const
{
    return m_possibleCovers;
}

int SabasBook::lastIndex() const
{
    return m_lastIndex;
}

void SabasBook::setLastIndex(int lastIndex)
{
    m_lastIndex = lastIndex;
}

int SabasBook::mediaCount() const
{
    return m_playlist->mediaCount();
}

QString SabasBook::mediaToDisplayStringAt(int index) const
{
    return m_playlist->media(index).canonicalUrl().toLocalFile().remove(m_rootPath + "/");
}

void SabasBook::setPlaybackRate(qreal rate)
{
    m_playbackRate = rate;
}

qreal SabasBook::playbackRate() const
{
    return m_playbackRate;
}

void SabasBook::emitVissibleDataChangedSignals()
{
    emit nameChanged(m_name);
    emit coverPathChanged(m_coverPath);
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

bool SabasBook::locateMedia()
{
    if (m_rootPath.isEmpty())
        return false;
    scanFolder(m_rootPath);
    return !m_playlist->isEmpty();
}

bool SabasBook::relocateMedia()
{
    m_playlist->clear();
    return locateMedia();
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
    if (m_coverPath != path) {
        m_coverPath = path;
        emit coverPathChanged(path);
    }
}

void SabasBook::setPossibleCovers(const QStringList &coverUrls)
{
    if (m_possibleCovers != coverUrls) {
        m_possibleCovers = coverUrls;
        emit possibleCoversChanged(coverUrls);
    }
}

int SabasBook::currentIndex() const
{
    return m_currentIndex;
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
