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

    SabasPlugin sp;
    sp.registerTypes("harbour.sabas.sabasplugin");

    QGuiApplication *app = SailfishApp::application(argc, argv);
    QTranslator translator;
    translator.load(QLocale::system(), "harbour-sabas", "_", SailfishApp::pathTo("i18n").toLocalFile(), ".qm");
    app->installTranslator(&translator);
    QQuickView *v = SailfishApp::createView();
    v->setSource(SailfishApp::pathTo("qml/harbour-sabas.qml"));
    v->show();
    return app->exec();
}
