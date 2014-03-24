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
#include <QStandardPaths>
#include <QFileSystemWatcher>

SabasLibrary::SabasLibrary(QObject *parent) :
    QObject(parent),
    m_player(new QMediaPlayer(this)),
    m_sleepTimer(0),
    m_selectedBook(0),
    m_nam(0),
    m_saveTimer(0),
    m_searchingCover(false),
    m_fsw(new QFileSystemWatcher(this))
{
    m_libraryRootPaths << QDir::homePath() + "/Audiobooks" << QDir::homePath() + "/Documents/Audiobooks";
    m_fsw->addPaths(m_libraryRootPaths);
    loadSettings();
    scanNewBooks();
    saveSettings();
    m_player->setNotifyInterval(1000);
    connect(m_player, &QMediaPlayer::stateChanged, [=](QMediaPlayer::State state){
        emit playingChanged(state == QMediaPlayer::PlayingState);
    });
    connect(m_player, &QMediaPlayer::mediaStatusChanged, [=](QMediaPlayer::MediaStatus status){
        if (status == QMediaPlayer::BufferedMedia) {
            QTimer *s = new QTimer(this);
            s->setSingleShot(true);
            s->start(1000);
            connect(s, &QTimer::timeout, [=](){
                if (m_selectedBook != 0)
                    m_player->setPlaybackRate(m_selectedBook->playbackRate());
                s->deleteLater();
            });

        }
    });
    connect(m_player, &QMediaPlayer::durationChanged, this, &SabasLibrary::trackDurationChanged);
    connect(m_player, &QMediaPlayer::positionChanged, this, &SabasLibrary::trackPositionChanged);
    connect(m_fsw, &QFileSystemWatcher::directoryChanged, this, &SabasLibrary::scanChanges);
}

SabasLibrary::~SabasLibrary()
{
    stop();
    saveSettings();
    qDeleteAll(m_books);
}

SabasBook *SabasLibrary::at(int index)
{
    return m_books.at(index);
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

bool SabasLibrary::playing() const
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
    int index = m_selectedBook->currentIndex();
    m_selectedBook->setLastTrackPosition(m_player->position());
    m_player->stop();
    m_player->setPlaylist(0);
    m_selectedBook->setCurrentIndex(index);
    m_selectedBook = 0;
    emit selectedBookChanged(0);
    m_saveTimer->stop();
    m_saveTimer->deleteLater();
    m_saveTimer = 0;
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

    //Rearrange list
    m_books.removeOne(m_selectedBook);
    m_books.prepend(m_selectedBook);
    emit booksChanged(books());

    emit selectedBookChanged(m_selectedBook);
    if (fromBeginning) {
        book->setLastTrackPosition(0);
        book->setCurrentIndex(0);
    }
    //NOTE: if I set position here, it gets ignored. Didn't find right singnal when to set it, so
    //lets use timer to set the position. Tested all functions that in my mind would make any sense,
    //most notably, seekable and media status buffered. It's same deal with playbackRate...
    int i = book->currentIndex();
    m_player->setPlaylist(book->playlist());
    book->setCurrentIndex(i);
    m_player->play();
    QTimer *s = new QTimer(this);
    s->setSingleShot(true);
    s->start(500);
    connect(s, &QTimer::timeout, [=](){
        if (m_selectedBook != 0) //check if already stopped
            m_player->setPosition(m_selectedBook->lastTrackPosition());
        s->deleteLater();
    });
    if (m_saveTimer == 0)
        m_saveTimer = new QTimer(this);
    m_saveTimer->setInterval(1000 * 30);
    m_saveTimer->setSingleShot(false);
    connect(m_saveTimer, &QTimer::timeout, this, &SabasLibrary::saveSettings);
    m_saveTimer->start();
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
    emit sleepTimerActiveChanged(true);
}

void SabasLibrary::stopSleepTimer()
{
    if (m_sleepTimer == 0)
        return;
    m_sleepTimer->stop();
    m_sleepTimer->deleteLater();
    m_sleepTimer = 0;
    emit sleepTimerActiveChanged(false);
}

void SabasLibrary::searchCover(SabasBook *book, const QString &customSearchString, bool feelingLucky)
{
    if (!isCoverSearchEnabled())
        return;
    if (m_nam == 0)
        m_nam = new QNetworkAccessManager(this);
    m_searchingCover = true;
    emit searchingCoverChanged(m_searchingCover);
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
    if (feelingLucky)
        query.addQueryItem("$top", "1");
    QUrl url("https://api.datamarket.azure.com/Bing/Search/Image");
    url.setQuery(query);
    QNetworkRequest request(url);
    request.setRawHeader("Authorization", "Basic " + keyBase64);
    QNetworkReply *reply = m_nam->get(request);
    connect(reply, &QNetworkReply::finished, [=](){
        QJsonObject j = QJsonDocument::fromJson(reply->readAll()).object();
        QJsonArray imageObjets = j["d"].toObject()["results"].toArray();
        if (feelingLucky) {
            QString s = imageObjets.first().toObject()["MediaUrl"].toString();
            if (!s.isEmpty())
                downloadCover(s, book);
        } else {
            QStringList urls;
            foreach (QJsonValue v, imageObjets) {
                QString s = v.toObject()["MediaUrl"].toString();
                if (!s.isEmpty())
                    urls.append(s);
            }
            book->setOnlineCovers(urls);
        }
        m_searchingCover = false;
        emit searchingCoverChanged(m_searchingCover);
    });
}

void SabasLibrary::searchMissingCovers()
{
    foreach (SabasBook *sb, m_books) {
        if (sb->coverPath().isEmpty())
            searchCover(sb);
    }
}

SabasBook *SabasLibrary::selectedBook() const
{
    return m_selectedBook;
}

bool SabasLibrary::sleepTimerActive() const
{
    return m_sleepTimer != 0;
}

bool SabasLibrary::isCoverSearchEnabled() const
{
    return !BING_KEY.isEmpty();
}

bool SabasLibrary::searchingCover() const
{
    return m_searchingCover;
}

void SabasLibrary::saveSettings()
{
    qDebug() << "Saving settings";
    QSettings s;
    s.setValue("libraryRootPaths", m_libraryRootPaths);
    s.beginWriteArray("books");
    for (int k = 0; k < m_books.size(); ++k) {
        SabasBook *sb = m_books.at(k);
        s.setArrayIndex(k);
        s.setValue("rootPath", sb->rootPath());
        s.setValue("name", sb->name());
        s.setValue("lastIndex", sb->currentIndex());
        s.setValue("lastTrackPosition", sb->lastTrackPosition());
        s.setValue("localCovers", sb->localCovers());
        s.setValue("coverPath", sb->coverPath());
#ifdef SAVE_PLAYLIST
        s.setValue("playlist", sb->playListStrings());
#endif
    }
    s.endArray();
}

void SabasLibrary::loadSettings()
{
    QSettings s;
    m_libraryRootPaths = s.value("libraryRootPaths",
                                 QStringList() << QDir::homePath() + "/Audiobooks"
                                 << QDir::homePath() + "/Documents/Audiobooks").toStringList();
    int size = s.beginReadArray("books");
    for (int k = 0; k < size; ++k) {
        s.setArrayIndex(k);
        QString rootPath = s.value("rootPath").toString();
        QDir rd = QDir(rootPath);
        if (!rd.exists()) {
            qDebug() << rootPath << "removed";
            foreach (const QString &cover, s.value("localCovers").toStringList()) {
                if (cover.startsWith(QStandardPaths::writableLocation(QStandardPaths::CacheLocation))) {
                    QFile::remove(cover);
                    qDebug() << "removed downloaded cover" << cover;
                }
            }
            continue;
        }
        SabasBook *sb = new SabasBook(rootPath);
        sb->setName(s.value("name").toString());
#ifdef SAVE_PLAYLIST
        sb->setPlaylist(s.value("playlist").toStringList());
#else
        sb->locateMedia();
#endif
        m_books.append(sb);
        emit booksChanged(books());
        QQmlEngine::setObjectOwnership(sb, QQmlEngine::CppOwnership);
        sb->setCurrentIndex(s.value("lastIndex").toInt());
        sb->setLastTrackPosition(s.value("lastTrackPosition").toLongLong());
        sb->setLocalCovers(s.value("localCovers").toStringList());
        QString savedCover = s.value("coverPath").toString();
        if (!savedCover.isEmpty())
            sb->setCoverPath(savedCover);
    }
    s.endArray();
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
//                    qDebug() << "book already loaded from settings";
                    break;
                }
            }
            if (alreadyThere)
                continue;
            SabasBook *book = new SabasBook(parentDir.absolutePath() + "/" + s);
            QQmlEngine::setObjectOwnership(book, QQmlEngine::CppOwnership);
            book->setName(s);
            book->locateMedia();
            m_books.append(book);
            emit booksChanged(books());
        }
    }
}

void SabasLibrary::scanDeletedBooks()
{
    foreach (SabasBook *sb, m_books) {
        if (!QDir(sb->rootPath()).exists() || !isUnderLibraryPaths(sb->rootPath())) {
            if (m_selectedBook == sb) {
                m_selectedBook = 0;
                emit selectedBookChanged(0);
            }
            foreach (const QString &cover, sb->localCovers()) {
                if (cover.startsWith(QStandardPaths::writableLocation(QStandardPaths::CacheLocation))) {
                    QFile::remove(cover);
                    qDebug() << "removed downloaded cover" << cover;
                }
            }
            m_books.removeOne(sb);
            emit booksChanged(books());
            sb->deleteLater();
        }
    }
}

void SabasLibrary::scanChanges(const QString &path)
{
    Q_UNUSED(path)
    scanNewBooks();
    scanDeletedBooks();
}

bool SabasLibrary::isUnderLibraryPaths(const QString &path) const
{
    foreach (const QString &p, m_libraryRootPaths) {
        if (path.startsWith(p))
            return true;
    }
    return false;
}

void SabasLibrary::downloadCover(const QString &url, SabasBook *forBook)
{
    QUrl u(url);
    QNetworkReply *reply = m_nam->get(QNetworkRequest(u));
    connect(reply, &QNetworkReply::finished, [=](){
        QFileInfo fi(u.path());
        fi.setFile(QStandardPaths::writableLocation(QStandardPaths::CacheLocation)
                   + "/cover-" + forBook->name().toLatin1().toPercentEncoding("", "~.", '_') + "-" + fi.fileName());
        QDir fileDir = fi.absoluteDir();
        if (!fileDir.exists()) {
            if (!fileDir.mkpath(fileDir.absolutePath())) {
                qWarning() << "Can't create dir " << fileDir;
                return;
            }
        }

        QFile f(fi.absoluteFilePath());
        if (!f.open(QFile::WriteOnly)) {
            qWarning() << "Can't write file " << f.fileName();
            return;
        }
        qDebug() << "Saving filename " << f.fileName();
        f.write(reply->readAll());
        f.close();
        forBook->setCoverPath("file://" + f.fileName());
    });
}

void SabasLibrary::setLibraryRootPath(const QString &path)
{
    m_libraryRootPaths = QStringList(path);
    scanChanges(path);
}

QStringList SabasLibrary::books() const
{
    QStringList names;
    foreach (SabasBook *sb, m_books) {
        names.append(sb->name());
    }
    return names;
}
