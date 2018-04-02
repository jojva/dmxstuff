#include <QtWidgets>

#include "mainwindow.h"

MainWindow::MainWindow(CSynesthesizer &synesthesizer)
    : synesthesizer(synesthesizer)
    , timer(new QTimer(this))
    , attackSpinbox(new QSpinBox)
    , decaySpinbox(new QSpinBox)
    , sustainSpinbox(new QSpinBox)
    , releaseSpinbox(new QSpinBox)
{
    buildGui();
    readSettings();

    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(0);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

void MainWindow::readSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
        resize(availableGeometry.width() / 2, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
}

void MainWindow::writeSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
}

void MainWindow::buildGui()
{
    attackSpinbox->setRange(0, 100000);
    decaySpinbox->setRange(0, 100000);
    sustainSpinbox->setRange(0, 100);
    releaseSpinbox->setRange(0, 100000);

    QFormLayout *adsrFormLayout = new QFormLayout;
    adsrFormLayout->addRow(tr("Attack time (ms) "), attackSpinbox);
    adsrFormLayout->addRow(tr("Decay time (ms) "), decaySpinbox);
    adsrFormLayout->addRow(tr("Sustain level (%) "), sustainSpinbox);
    adsrFormLayout->addRow(tr("Release time (ms) "), releaseSpinbox);

    QGroupBox *adsrGroupbox = new QGroupBox(tr("ADSR configuration"));
    adsrGroupbox->setLayout(adsrFormLayout);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setAlignment(Qt::AlignVCenter);
    layout->addWidget(adsrGroupbox);

    QWidget *widget = new QWidget();
    widget->setLayout(layout);
    setCentralWidget(widget);
    show();
}

void MainWindow::update()
{
    synesthesizer.Run();
}
