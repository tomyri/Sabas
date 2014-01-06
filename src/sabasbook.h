#ifndef SABASBOOK_H
#define SABASBOOK_H

#include <QObject>

class QMediaPlaylist;
class QMediaPlayer;

class SabasBook : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(QString coverPath READ coverPath NOTIFY coverPathChanged)

public:
    explicit SabasBook(const QString &folder, QObject *parent = 0);
    ~SabasBook();

    void setName(const QString &name);
    QString name() const;

    qint64 lastTrackPosition() const;

    QString rootPath() const;
    void setRootPath(const QString &rootPath);

    QMediaPlaylist *playlist() const;
    QStringList playListStrings() const;
    void setPlaylist(const QStringList &tracks);

    bool findMedia();
    int currentIndex() const;
    void setCurrentIndex(int index);
    QString coverPath() const;

    int lastIndex() const;
    void setLastIndex(int lastIndex);
    Q_INVOKABLE int mediaCount() const;
    Q_INVOKABLE QString mediaToDisplayStringAt(int index) const;

public slots:
    void setLastTrackPosition(qint64 position);
    void next();
    void previous();
    void setCoverPath(QString path);

signals:
    void nameChanged(const QString &name);
    void trackPositionChanged(qint64 position);
    void trackDurationChanged(qint64 position);
    void currentIndexChanged(int index);
    void coverPathChanged(QString path);

private:
    void scanFolder(const QString &folder);
    QMediaPlaylist *m_playlist;
    QString m_name;
    QString m_rootPath;
    int m_currentIndex;
    int m_lastIndex;
    qint64 m_lastTrackPosition;
    QString m_coverPath;
};

#endif // SABASBOOK_H
