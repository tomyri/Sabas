#ifndef SABASPLUGIN_H
#define SABASPLUGIN_H

#include <QQmlExtensionPlugin>

class SabasPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
public:
    explicit SabasPlugin(QObject *parent = 0);
    ~SabasPlugin();
    void registerTypes(const char *uri);
};

#endif // SABASPLUGIN_H
