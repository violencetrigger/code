#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QMainWindow>
#include <QString>
#include <QRegularExpression>
#include <cmath>

namespace Ui {
class Calculator;
}

class Calculator : public QMainWindow
{
    // Declares our class as a QObject which is the base class
    // for all Qt objects
    // QObjects handle events
    Q_OBJECT

public:
    // Declare a constructor and by passing nullptr we state this widget
    // has no parent
    explicit Calculator(QWidget *parent = nullptr);
    ~Calculator();
    void replaceMathSign();

private:
    Ui::Calculator *ui;

private slots:
    void numPressed();
    void mathButtonPressed();
    void equalButtonPressed();
    void changeSign();
    void inputHistory(QString);
    void clear();
    void backspace();
    void dot();
    void squareRoot();
    void square();
};

#endif // CALCULATOR_H
