#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include "sabasplugin.h"
#include <QCoreApplication>
#include <QTranslator>
#include <QLocale>
#include <sailfishapp.h>
#include <QQuickView>
#include <QGuiApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("Sabas");
    QCoreApplication::setOrganizationDomain("tomyri.com");
    QCoreApplication::setApplicationName("Sabas");

    QScopedPointer<SabasPlugin> sp(new SabasPlugin);
    sp->registerTypes("harbour.sabas.sabasplugin");

    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    QTranslator translator;
    translator.load(QLocale::system(), "harbour-sabas", "_", SailfishApp::pathTo("i18n").toLocalFile(), ".qm");
    app->installTranslator(&translator);
    QScopedPointer<QQuickView> v(SailfishApp::createView());
    v->setSource(SailfishApp::pathTo("qml/main.qml"));
    v->show();
    return app->exec();
}
