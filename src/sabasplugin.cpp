#include "sabasplugin.h"
#include "sabaslibrary.h"
#include "sabasbook.h"
#include <QJSEngine>
#include <QtQuick>

static SabasLibrary *slibrary(0);

static QObject *sabasLibraryInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    if (slibrary == 0)
        slibrary = new SabasLibrary;
    return slibrary;
}

SabasPlugin::SabasPlugin(QObject *parent) :
    QQmlExtensionPlugin(parent)
{
}

SabasPlugin::~SabasPlugin()
{
    delete slibrary;
}

void SabasPlugin::registerTypes(const char *uri)
{
    // @uri harbour.sabas.sabasplugin
    qmlRegisterSingletonType<SabasLibrary>(uri, 1, 0, "SabasLibrary", sabasLibraryInstance);
    qmlRegisterUncreatableType<SabasBook>(uri, 1, 0, "SabasBook", QStringLiteral("Books are provided by SabasLibrary"));
}
