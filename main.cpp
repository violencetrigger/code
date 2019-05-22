#include "calculator.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Run the program
    Calculator CalculatorRun;
    CalculatorRun.show();

    return a.exec();
}
