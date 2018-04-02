#pragma once

#include <QMainWindow>

#include "synesthesizer.h"

QT_BEGIN_NAMESPACE
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
    void update();

private:
    void readSettings();
    void writeSettings();
    void buildGui();

private:
    CSynesthesizer& synesthesizer;

    QTimer* timer;
    QSpinBox* attackSpinbox;
    QSpinBox* decaySpinbox;
    QSpinBox* sustainSpinbox;
    QSpinBox* releaseSpinbox;
};
