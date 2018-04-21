#include <QApplication>

#include "mainwindow.h"
#include "synesthesizer.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("Musicology");
    QCoreApplication::setApplicationName("Synesthesizer");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

    MainWindow mainWin;
    mainWin.show();

    CSynesthesizer Synesthesizer(mainWin);
    Synesthesizer.Init(argc, argv);
    mainWin.AttachSynesthesizer(&Synesthesizer);
    return app.exec();
}
