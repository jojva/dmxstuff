#pragma once

#include <QMainWindow>

#include "synesthesizer.h"

QT_BEGIN_NAMESPACE
namespace QtCharts
{
class QChart;
class QLineSeries;
}
class QSpinBox;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(CSynesthesizer& synesthesizer);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void adsrChanged();
    void update();

private:
    void readSettings();
    void writeSettings();
    void buildGui();
    void updateADSRview();
    void setupSignals();

private:
    CSynesthesizer& synesthesizer;

    QTimer* timer;
    QSpinBox* attackSpinbox;
    QSpinBox* decaySpinbox;
    QSpinBox* sustainSpinbox;
    QSpinBox* releaseSpinbox;
    QtCharts::QLineSeries *series;
    QtCharts::QChart *chart;
};
