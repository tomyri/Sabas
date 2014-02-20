#ifndef SABASBOOK_H
#define SABASBOOK_H

#include <QObject>
#include <QStringList>

//#define SAVE_PLAYLIST
class QMediaPlaylist;
class QFileSystemWatcher;
class SabasBook : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(QString coverPath READ coverPath WRITE setCoverPath NOTIFY coverPathChanged)
    Q_PROPERTY(QStringList possibleCovers READ possibleCovers NOTIFY possibleCoversChanged)
    Q_PROPERTY(QStringList tracks READ tracks NOTIFY tracksChanged)
    Q_PROPERTY(int trackCount READ trackCount NOTIFY trackCountChanged)

public:
    explicit SabasBook(const QString &folder, QObject *parent = 0);
    ~SabasBook();

    void setName(const QString &name);
    QString name() const;

    qint64 lastTrackPosition() const;

    QString rootPath() const;
    void setRootPath(const QString &rootPath);

    QMediaPlaylist *playlist() const;
#ifdef SAVE_PLAYLIST
    QStringList playListStrings() const;
    void setPlaylist(const QStringList &tracks);
#endif
    int currentIndex() const;
    void setCurrentIndex(int index);
    QString coverPath() const;

    int lastIndex() const;
    void setPlaybackRate(qreal rate);
    qreal playbackRate() const;
    QStringList possibleCovers() const;
    QStringList tracks() const;
    int trackCount() const;

    QStringList localCovers() const;
    void setLocalCovers(const QStringList &localCovers);

public slots:
    void locateMedia();
    void setLastTrackPosition(qint64 position);
    void next();
    void previous();
    void setCoverPath(const QString &path);
    void setOnlineCovers(const QStringList &coverUrls);

signals:
    void nameChanged(const QString &name);
    void trackPositionChanged(qint64 position);
    void trackDurationChanged(qint64 position);
    void currentIndexChanged(int index);
    void coverPathChanged(QString path);
    void possibleCoversChanged(QStringList covers);
    void tracksChanged(QStringList tracks);
    void trackCountChanged(int count);

private:
    void scanFolder(const QString &folder);
    QMediaPlaylist *m_playlist;
    QString m_name;
    QString m_rootPath;
    qint64 m_lastTrackPosition;
    QString m_coverPath;
    qreal m_playbackRate;
    QStringList m_localCovers;
    QStringList m_onlineCovers;
    QFileSystemWatcher *m_fsw;
    int m_trackCount;
};

#endif // SABASBOOK_H
