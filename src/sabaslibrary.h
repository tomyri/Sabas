#ifndef SABASLIBRARY_H
#define SABASLIBRARY_H

#include "sabasbook.h"
#include <QObject>
#include <QList>
#include <QStringList>

class QMediaPlayer;
class QTimer;
class QNetworkAccessManager;
class QFileSystemWatcher;
class SabasLibrary : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool playing READ playing NOTIFY playingChanged)
    Q_PROPERTY(qint64 trackDuration READ trackDuration NOTIFY trackDurationChanged)
    Q_PROPERTY(qint64 trackPosition READ trackPosition WRITE setTrackPosition NOTIFY trackPositionChanged)
    Q_PROPERTY(SabasBook* selectedBook READ selectedBook NOTIFY selectedBookChanged)
    Q_PROPERTY(bool sleepTimerActive READ sleepTimerActive NOTIFY sleepTimerActiveChanged)
    Q_PROPERTY(bool searchingCover READ searchingCover NOTIFY searchingCoverChanged)
    Q_PROPERTY(QStringList books READ books NOTIFY booksChanged)

public:
    explicit SabasLibrary(QObject *parent = 0);
    ~SabasLibrary();
    Q_INVOKABLE SabasBook *at(int index);
    qint64 trackDuration() const;
    qint64 trackPosition() const;
    SabasBook *selectedBook() const;
    bool sleepTimerActive() const;
    Q_INVOKABLE bool coverSearchEnabled() const;
    bool searchingCover() const;
    QStringList books() const;

public slots:
    void toggle() const;
    bool playing() const;
    void skip(qint64 msec) const;
    void stop();
    void pause();
    void play(SabasBook *book, bool fromBeginning = false);
    void setTrackPosition(qint64 position);
    void startSleepTimer(int minutes);
    void stopSleepTimer();
    void searchCover(SabasBook *book, const QString &customSearchString = "", bool feelingLucky = true);
    void searchMissingCovers();
    void downloadCover(const QString &url, SabasBook *forBook);
    void setLibraryRootPath(const QString &path);

signals:
    void playingChanged(bool playing);
    void trackDurationChanged(qint64 duration);
    void trackPositionChanged(qint64 position);
    void selectedBookChanged(SabasBook* book);
    void sleepTimerActiveChanged(bool isRunning);
    void searchingCoverChanged(bool searching);
    void booksChanged(QStringList books);

private:
    void saveSettings();
    void loadSettings();
    void scanNewBooks();
    void scanDeletedBooks();
    void scanChanges(const QString &path);
    bool isUnderLibraryPaths(const QString &path) const;
    QList<SabasBook*> m_books;
    QMediaPlayer *m_player;
    QTimer *m_sleepTimer;
    SabasBook *m_selectedBook;
    QNetworkAccessManager *m_nam;
    QStringList m_libraryRootPaths;
    QTimer *m_saveTimer;
    bool m_searchingCover;
    QFileSystemWatcher *m_fsw;
};

#endif // SABASLIBRARY_H
