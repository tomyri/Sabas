#ifndef SABASLIBRARY_H
#define SABASLIBRARY_H

#include "sabasbook.h"
#include <QObject>
#include <QList>

class QMediaPlayer;
class QTimer;
class SabasLibrary : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY isPlayingChanged)
    Q_PROPERTY(qint64 trackDuration READ trackDuration NOTIFY trackDurationChanged)
    Q_PROPERTY(qint64 trackPosition READ trackPosition WRITE setTrackPosition NOTIFY trackPositionChanged)
    Q_PROPERTY(SabasBook* selectedBook READ selectedBook NOTIFY selectedBookChanged)
    Q_PROPERTY(QString libraryRootPath READ libraryRootPath WRITE setLibraryRootPath NOTIFY libraryRootPathChanged)
    Q_PROPERTY(bool isSleepTimerActive READ isSleepTimerActive NOTIFY sleepTimerActivityChanged)

public:
    explicit SabasLibrary(QObject *parent = 0);
    ~SabasLibrary();
    QList<SabasBook*>  books() const;
    Q_INVOKABLE int count() const;
    Q_INVOKABLE SabasBook *at(int index);
    void scanNewBooks();
    qint64 trackDuration() const;
    qint64 trackPosition() const;
    SabasBook *selectedBook() const;
    QString libraryRootPath() const;
    bool isSleepTimerActive() const;

public slots:
    void toggle() const;
    bool isPlaying() const;
    void skip(qint64 msec) const;
    void stop();
    void pause();
    void play(SabasBook *book, bool fromBeginning = false);
    void setTrackPosition(qint64 position);
    void setLibraryRootPath(const QString &path);
    void startSleepTimer(int minutes);
    void stopSleepTimer();

signals:
    void booksChanged(QList<SabasBook*> books);
    void isPlayingChanged(bool playing);
    void trackDurationChanged(qint64 duration);
    void trackPositionChanged(qint64 position);
    void selectedBookChanged(SabasBook* book);
    void libraryRootPathChanged(QString arg);
    void sleepTimerActivityChanged(bool isRunning);

private:
    void saveSettings();
    void loadSettings();
    QList<SabasBook*> m_books;
    QMediaPlayer *m_player;
    QTimer *m_saveTimer;
    QTimer *m_sleepTimer;
    SabasBook *m_selectedBook;
    QString m_libraryRootPath;
};

#endif // SABASLIBRARY_H
