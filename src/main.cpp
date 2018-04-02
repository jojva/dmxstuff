#include <QApplication>

#include "mainwindow.h"
#include "synesthesizer.h"

int main(int argc, char *argv[])
{
    CSynesthesizer Synesthesizer;
    Synesthesizer.Init(argc, argv);

    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("Musicology");
    QCoreApplication::setApplicationName("Synesthesizer");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

    MainWindow mainWin(Synesthesizer);
    mainWin.show();
    return app.exec();
}
