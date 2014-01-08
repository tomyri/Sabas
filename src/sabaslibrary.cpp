#include "sabaslibrary.h"
#include "bingkey.h"
#include <QCoreApplication>
#include <QDir>
#include <QMediaPlayer>
#include <QQmlEngine>
#include <QSettings>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

SabasLibrary::SabasLibrary(QObject *parent) :
    QObject(parent),
    m_player(new QMediaPlayer(this)),
    //    m_saveTimer(new QTimer(this)),
    m_sleepTimer(0),
    m_selectedBook(0),
    m_nam(0)
{
    m_libraryRootPaths << QDir::homePath() + "/Audiobooks" << QDir::homePath() + "/Documents/Audiobooks";
    loadSettings();
    scanNewBooks();
    saveSettings();
    m_player->setNotifyInterval(1000);
    connect(m_player, &QMediaPlayer::stateChanged, [=](QMediaPlayer::State state){
        emit isPlayingChanged(state == QMediaPlayer::PlayingState);
        //        if (state == QMediaPlayer::PlayingState)
        //            m_saveTimer->start();
        //        else
        //            m_saveTimer->stop();
    });
    connect(m_player, &QMediaPlayer::durationChanged, this, &SabasLibrary::trackDurationChanged);
    connect(m_player, &QMediaPlayer::positionChanged, this, &SabasLibrary::trackPositionChanged);
    //    connect(m_saveTimer, &QTimer::timeout, this, &SabasLibrary::saveSettings);
    //    m_saveTimer->setInterval(60000);
    //    m_saveTimer->setSingleShot(false);
    //        for (int k = 0; k < 50; ++k) {
    //            SabasBook *s = new SabasBook("mooo " + QString::number(k));
    //            s->setName("mooo " + QString::number(k));
    //            m_books.append(s);
    //        }
}

SabasLibrary::~SabasLibrary()
{
    stop();
    saveSettings();
    qDeleteAll(m_books);
}

QList<SabasBook*> SabasLibrary::books() const
{
    return m_books;
}

int SabasLibrary::count() const
{
    return m_books.size();
}

SabasBook *SabasLibrary::at(int index)
{
    return m_books.at(index);
}

void SabasLibrary::scanNewBooks()
{
    foreach (const QString &rootPath, m_libraryRootPaths) {
        QDir parentDir = QDir(rootPath);
        QStringList bookStrings = parentDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        foreach (const QString s, bookStrings) {
            bool alreadyThere = false;
            foreach (SabasBook *b, m_books) {
                if (b->rootPath() == parentDir.absolutePath() + "/" + s) {
                    alreadyThere = true;
                    qDebug() << "book already loaded from settings";
                    break;
                }
            }
            if (alreadyThere)
                continue;
            SabasBook *book = new SabasBook(parentDir.absolutePath() + "/" + s);
            book->setName(s);
            if (book->locateMedia())
                m_books.append(book);
            else
                delete book;
        }
    }
}

void SabasLibrary::toggle() const
{
    if (m_player->playlist() == 0)
        return;
    if (m_player->state() != QMediaPlayer::PlayingState)
        m_player->play();
    else
        m_player->pause();
}

bool SabasLibrary::isPlaying() const
{
    return m_player->state() == QMediaPlayer::PlayingState;
}

void SabasLibrary::skip(qint64 msec) const
{
    m_player->setPosition(m_player->position() + msec);
}

void SabasLibrary::stop()
{
    if (m_selectedBook == 0)
        return;
    m_selectedBook->setLastIndex(m_selectedBook->currentIndex());
    m_selectedBook->setLastTrackPosition(m_player->position());
    m_player->stop();
    m_player->setPlaylist(0);
    m_selectedBook = 0;
    emit selectedBookChanged(0);
    saveSettings();
}

void SabasLibrary::pause()
{
    m_player->pause();
}

void SabasLibrary::play(SabasBook *book, bool fromBeginning)
{
    if (m_selectedBook != 0) {
        m_selectedBook->setLastTrackPosition(m_player->position());
    }
    m_selectedBook = book;
    emit selectedBookChanged(m_selectedBook);
    if (fromBeginning) {
        book->setLastTrackPosition(0);
        book->setLastIndex(0);
    }
    //NOTE: if I set position here, it gets ignored. Didn't find right singnal when to set it, so
    //lets use helper boolean and set the position when status changed to playing
    m_player->setPlaylist(book->playlist());
    book->setCurrentIndex(book->lastIndex());
    m_player->play();
    QTimer *s = new QTimer(this);
    s->setSingleShot(true);
    s->start(500);
    connect(s, &QTimer::timeout, [=](){ //TODO: ok, this works. work out some way to get the right time to set the position
        m_player->setPosition(m_selectedBook->lastTrackPosition());
        s->deleteLater();
    });
}

qint64 SabasLibrary::trackDuration() const
{
    return m_player->duration();
}

qint64 SabasLibrary::trackPosition() const
{
    return m_player->position();
}

void SabasLibrary::setTrackPosition(qint64 position)
{
    qDebug() << "Setting position";
    m_player->setPosition(position);
}

void SabasLibrary::startSleepTimer(int minutes)
{
    if (m_sleepTimer == 0) {
        m_sleepTimer = new QTimer(this);
        m_sleepTimer->setSingleShot(true);
        connect(m_sleepTimer, &QTimer::timeout, [=](){
            pause();
            stopSleepTimer();
        });
    }
    m_sleepTimer->start(minutes * 60 * 1000);
    emit sleepTimerActivityChanged(true);
}

void SabasLibrary::stopSleepTimer()
{
    if (m_sleepTimer == 0)
        return;
    m_sleepTimer->stop();
    m_sleepTimer->deleteLater();
    m_sleepTimer = 0;
    emit sleepTimerActivityChanged(false);
}

void SabasLibrary::searchCover(SabasBook *book, const QString &customSearchString)
{
    if (!isCoverSearchEnabled())
        return;
    if (m_nam == 0)
        m_nam = new QNetworkAccessManager(this);
    QByteArray keyBase64 = BING_KEY + ":" + BING_KEY;
    keyBase64 = keyBase64.toBase64();
    QString searchString;
    if (customSearchString.isEmpty())
        searchString = book->name();
    else
        searchString = customSearchString;
    searchString.prepend("'");
    searchString.append("'");
    QUrlQuery query;
    query.addQueryItem("Query", QUrl(searchString).toEncoded());
    query.addQueryItem("$format", "json");
    query.addQueryItem("$top", "1");
    QUrl url("https://api.datamarket.azure.com/Bing/Search/Image");
    url.setQuery(query);
    QNetworkRequest request(url);
    request.setRawHeader("Authorization", "Basic " + keyBase64);
    QNetworkReply *reply = m_nam->get(request);
    connect(reply, &QNetworkReply::finished, [=](){
        QJsonObject j = QJsonDocument::fromJson(reply->readAll()).object();
        QString s = j["d"].toObject()["results"].toArray().first().toObject()["MediaUrl"].toString();
        if (!s.isEmpty()) //TODO: download and save cover
            book->setCoverPath(s);
    });
}

SabasBook *SabasLibrary::selectedBook() const
{
    return m_selectedBook;
}

bool SabasLibrary::isSleepTimerActive() const
{
    return m_sleepTimer != 0;
}

bool SabasLibrary::isCoverSearchEnabled() const
{
    return !BING_KEY.isEmpty();
}

void SabasLibrary::saveSettings()
{
    QSettings s;
    s.beginWriteArray("books");
    for (int k = 0; k < m_books.size(); ++k) {
        SabasBook *sb = m_books.at(k);
        s.setArrayIndex(k);
        s.setValue("rootPath", sb->rootPath());
        s.setValue("name", sb->name());
        s.setValue("lastIndex", sb->lastIndex());
        s.setValue("lastTrackPosition", sb->lastTrackPosition());
        s.setValue("coverPath", sb->coverPath());
        s.setValue("playlist", sb->playListStrings());
    }
    s.endArray();
}

void SabasLibrary::loadSettings()
{
    QSettings s;
    int size = s.beginReadArray("books");
    for (int k = 0; k < size; ++k) {
        s.setArrayIndex(k);
        QString rootPath = s.value("rootPath").toString();
        QDir rd = QDir(rootPath);
        if (!rd.exists()) {
            qDebug() << rootPath << "removed";
            continue;
        }
        SabasBook *sb = new SabasBook(rootPath);
        QQmlEngine::setObjectOwnership(sb, QQmlEngine::CppOwnership);
        sb->setName(s.value("name").toString());
        sb->setLastIndex(s.value("lastIndex").toInt());
        sb->setLastTrackPosition(s.value("lastTrackPosition").toLongLong());
        sb->setCoverPath(s.value("coverPath").toString());
        sb->setPlaylist(s.value("playlist").toStringList());
        m_books.append(sb);
    }
    s.endArray();
}
