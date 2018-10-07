#ifndef RUNDIALOG_H
#define RUNDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QTimer>

#include "pl0.h"

namespace Ui {
class RunDialog;
}

class RunDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RunDialog(QWidget *parent, const QList<QString>* source, const QList<instruction>* code);
    ~RunDialog();


private slots:
    void reset();
    void runOneStep();
    void run();
    void pause();
    void quit();
    int base(int l);
    void runOneCode();
    void updateLabel();
    void changeSpeed(int speed);
private :
    void createStackTable();
private:
    Ui::RunDialog *ui;
    const QList<QString>* source;
    const QList<instruction>* code;
    QTimer * timer;

    int prevIR;

    int PC;
    int ESP;
    int EBP;
    std::array<int, 1024> runtime_stack;
    instruction IR;

};

#endif // RUNDIALOG_H
