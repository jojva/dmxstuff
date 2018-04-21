#include "mainwindow.h"

#include <QtWidgets>
//#include <QtCharts/QChart>
//#include <QtCharts/QChartView>
//#include <QtCharts/QLineSeries>

#include <fstream>

#define FILE_CONFIG_NAME "adsr.txt"

MainWindow::MainWindow(void)
    : timer(new QTimer(this))
    , attackSpinbox(new QSpinBox)
    , decaySpinbox(new QSpinBox)
    , sustainSpinbox(new QSpinBox)
    , releaseSpinbox(new QSpinBox)
//    , series(new QtCharts::QLineSeries)
//    , chart(new QtCharts::QChart)
{
    buildGui();
    setupSignals();
    readSettings();

    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(0);
}

void MainWindow::AttachSynesthesizer(CSynesthesizer *synesthesizer)
{
    synesthesizer = synesthesizer;
}

CADSR* MainWindow::GetADSR(void) const
{
    return new CADSR(attackSpinbox->value(), decaySpinbox->value(), sustainSpinbox->value(), releaseSpinbox->value());
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
    std::ifstream adsrfile(FILE_CONFIG_NAME);
    if(adsrfile.is_open())
    {
        int value;
        adsrfile >> value;
        attackSpinbox->setValue(value);
        adsrfile >> value;
        decaySpinbox->setValue(value);
        adsrfile >> value;
        sustainSpinbox->setValue(value);
        adsrfile >> value;
        releaseSpinbox->setValue(value);
    }
    else
    {
        attackSpinbox->setValue(50);
        decaySpinbox->setValue(200);
        sustainSpinbox->setValue(50);
        releaseSpinbox->setValue(1000);
    }
}

void MainWindow::writeSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
    std::ofstream adsrfile(FILE_CONFIG_NAME, std::ofstream::trunc);
    adsrfile << attackSpinbox->value() << " ";
    adsrfile << decaySpinbox->value() << " ";
    adsrfile << sustainSpinbox->value() << " ";
    adsrfile << releaseSpinbox->value();
}

void MainWindow::buildGui()
{
    // ADSR configuration part
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

    // ADSR view part
//    series->append(0, 0);
//    series->append(attackSpinbox->value(), 100);
//    series->append(attackSpinbox->value() + decaySpinbox->value(), sustainSpinbox->value());
//    series->append(attackSpinbox->value() + decaySpinbox->value() + releaseSpinbox->value(), 0);

//    chart->legend()->hide();
//    chart->addSeries(series);
//    chart->createDefaultAxes();
//    chart->setTitle("ADSR view");

//    QtCharts::QChartView *chartView = new QtCharts::QChartView(chart);
//    chartView->setRenderHint(QPainter::Antialiasing);

    // Top-level widgets
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setAlignment(Qt::AlignHCenter);
    layout->addWidget(adsrGroupbox);
//    layout->addWidget(chartView);

    QWidget *widget = new QWidget();
    widget->setLayout(layout);
    setCentralWidget(widget);
    show();
}

void MainWindow::setupSignals()
{
    connect(attackSpinbox, SIGNAL(valueChanged(int)), this, SLOT(adsrChanged()));
    connect(decaySpinbox, SIGNAL(valueChanged(int)), this, SLOT(adsrChanged()));
    connect(sustainSpinbox, SIGNAL(valueChanged(int)), this, SLOT(adsrChanged()));
    connect(releaseSpinbox, SIGNAL(valueChanged(int)), this, SLOT(adsrChanged()));
}

void MainWindow::adsrChanged()
{
//    series->replace(1, attackSpinbox->value(), 100);
//    series->replace(2, attackSpinbox->value() + decaySpinbox->value(), sustainSpinbox->value());
//    series->replace(3, attackSpinbox->value() + decaySpinbox->value() + releaseSpinbox->value(), 0);
//    chart->createDefaultAxes();
}

void MainWindow::update()
{
    synesthesizer->Run();
}
