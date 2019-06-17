#include "calculator.h"
#include "ui_calculator.h"

// QString variables
// keeps whole input
static QString qstrInputKeeper = "";
// keeps input until last sign (including)
static QString qstrSecondInputKeeper = "";

// double variables
// holds current value of calculation, actual display value
static double dLeftValue = 0.0;
// holds right value
static double dRightValue = 0.0;
// holds result of calculation of left and right values
static double dResult = 0.0;

// bool variables
// has math sign in expression
static bool mathSign = false;
// math sign was pressed last
static bool wasMathSign = false;
// has sequence (non actually pressed equal button)
static bool sequence = false;
// has dot (point) in display value
static bool dotEntered = false;
// equal button was pressed last
static bool wasEqualButton = false;
// triggers of math signs
// will define if this was the last math button clicked
static bool divisionTrigger = false;
static bool multiplicationTrigger = false;
static bool additiveTrigger = false;
static bool subtractionTrigger = false;





// constructor
Calculator::Calculator(QWidget *parent) :
    // call the QMainWindow constructor
    QMainWindow(parent),
    // create the UI class and assign it to the ui member
    ui(new Ui::Calculator)
{
    // setup the UI
    ui->setupUi(this);

    // put 0.0 in Display
    ui->MainDisplay->setText(QString::number(dResult));
    // will hold references to all the number buttons
    QPushButton* numButtons[10];
    // cycle through locating the buttons
    for (int i = 0; i < 10; ++i) {
        QString buttonName = "Button" + QString::number(i);
        // get the buttons by name and add to array
        numButtons[i] = Calculator::findChild<QPushButton*>(buttonName);
        // when the button is released call numPressed()
        connect(numButtons[i], SIGNAL(released()), this, SLOT(numPressed()));
    }

    // Connect signals and slots for math buttons
    connect(ui->Add, SIGNAL(released()), this, SLOT(mathButtonPressed()));
    connect(ui->Subtract, SIGNAL(released()), this, SLOT(mathButtonPressed()));
    connect(ui->Multiply, SIGNAL(released()), this, SLOT(mathButtonPressed()));
    connect(ui->Divide, SIGNAL(released()), this, SLOT(mathButtonPressed()));
    // Connect equals button
    connect(ui->Equals, SIGNAL(released()), this, SLOT(equalButtonPressed()));
    // Connect change sign
    connect(ui->ChangeSign, SIGNAL(released()), this, SLOT(changeSign()));
    // Connect clear button
    connect(ui->Clear, SIGNAL(released()), this, SLOT(clear()));
    // Connect backspace button
    connect(ui->Backspace, SIGNAL(released()), this, SLOT(backspace()));
    // Connect dot(point) button
    connect(ui->Dot, SIGNAL(released()), this, SLOT(dot()));
    // Connect square root button
    connect(ui->SquareRoot, SIGNAL(released()), this, SLOT(squareRoot()));
    // Connect square button
    connect(ui->Square, SIGNAL(released()), this, SLOT(square()));
}



// delete created object
Calculator::~Calculator()
{
    delete ui;
}



// number button was pressed
void Calculator::numPressed()
{
    // special conditions checks

    // if the expression was not entered and equal button was pressed last
    if (dResult == 0.0 && wasEqualButton) {
        dLeftValue = 0.0;
        dRightValue = 0.0;
    }
    // sequence check (non actually pressed equal button)
    if (sequence) {
        // if it, clear main display
        ui->MainDisplay->setText("");
        sequence = false;
    }
    // if last pressed button is equal button
    if (wasEqualButton) {
        // and expression dont contains math sign or dot
        if (!mathSign && !dotEntered) {
            // clear all for later input
            clear();
        }
    }
    // if any math signs was entered
    if (wasMathSign) {
        // clear main display
        ui->MainDisplay->setText("");
        // set it to false
        wasMathSign = false;
    }
    // if nothing but equal button was pressed
    // and dot button was pressed after that
    if (dotEntered && wasEqualButton) {
        // add "0." to main and side displays
        ui->MainDisplay->setText("0" + tr("."));
        ui->SideDisplay->setText("0" + tr("."));
    }
    // equal button checking reset
    wasEqualButton = false;

    // end of special conditions checks

    // sender returns a pointer to the button pressed
    QPushButton* Button = static_cast<QPushButton*>(sender());
    // get number on button
    QString qstrButtonValue = Button->text();
    // get the value in the display
    QString qstrDisplayValue = ui->MainDisplay->text();

    // if value doesn't have a dot and it's equal to 0
    if (!dotEntered && qstrDisplayValue.toDouble() == 0.0) {
        // store first input in display
        ui->MainDisplay->setText(qstrButtonValue);
        // store first input in side display
        inputHistory(qstrButtonValue);
    // if value has a dot and display value doesn't equal to 0
    } else if (dotEntered) {
        // put the number to the right
        // new string is addition of two string
        // current display value and pressed button
        QString qstrNewValue = qstrDisplayValue + qstrButtonValue;
        // if the string has more then 14 digits
        if ( !(qstrNewValue.length() >= 14) ) {
            // stops adding new values
            // keep showing current value in main display
            ui->MainDisplay->setText(qstrNewValue);
            // call a function that keeps the entered value
            inputHistory(qstrNewValue);
        }
    } else {
        // put the new number to the right
        // new string is addition of two string
        // current display value and pressed button
        QString qstrNewValue = qstrDisplayValue + qstrButtonValue;
        double dNewValue = qstrNewValue.toDouble();

        // set value in display and allow up to 14 digits
        if (! (dNewValue >= 1e014) ) {
            ui->MainDisplay->setText(QString::number(dNewValue, 'g', 14));
            QString strNewValue = QString::number(dNewValue, 'g', 14);
            // call a function that keeps the entered value
            inputHistory(strNewValue);
        }
    }
}



// math button was pressed
void Calculator::mathButtonPressed()
{
    // Cancel out previous math button clicks
    divisionTrigger = false;
    multiplicationTrigger = false;
    additiveTrigger = false;
    subtractionTrigger = false;

    // special conditions checks

    // if result more or less then 0 and expression don't have a math sign,
    // or latest pressed button is equal button
    if ( ((dResult > 0.0 || dResult < 0.0) && !mathSign) || wasEqualButton) {
        // store current display value
        QString qstrDisplayValue = ui->MainDisplay->text();
        dLeftValue = qstrDisplayValue.toDouble();
        // clear value
        ui->SideDisplay->setText("");
        // change string that keeps input to result value
        qstrInputKeeper = QString::number(dResult, 'g', 14);
        // math expression will have a math sign
        mathSign = true;
        // sequence protection
        wasMathSign = true;
    }

    // sequence check (non actually pressed equal button)
    // it will be available only when
    // second input keeper string will be more then zero
    // and previous button click is number button
    if (qstrSecondInputKeeper.length() > 0 && !wasMathSign) {
        // if last second input keeper string character is plus
        if (qstrSecondInputKeeper
                [qstrSecondInputKeeper.length() - 1] == "+") {
            // set additive trigger to true
            additiveTrigger = true;
            // set sequence trigger to true
            sequence = true;
            // calculate the expression
            equalButtonPressed();
            // switch it to false
            additiveTrigger = false;
            // now expression in side display has one or more math signs
            mathSign = true;
            // equal; but with minus
        } else if (qstrSecondInputKeeper
                   [qstrSecondInputKeeper.length() - 1] == "-") {
            subtractionTrigger = true;
            sequence = true;
            equalButtonPressed();
            subtractionTrigger = false;
            mathSign = true;
            // equal; but with multiplication
        } else if (qstrSecondInputKeeper
                   [qstrSecondInputKeeper.length() - 1] == "*") {
            multiplicationTrigger = true;
            sequence = true;
            equalButtonPressed();
            multiplicationTrigger = false;
            mathSign = true;
            // equal; but with division
        } else if (qstrSecondInputKeeper
                   [qstrSecondInputKeeper.length() - 1] == "/") {
            divisionTrigger = true;
            sequence = true;
            equalButtonPressed();
            divisionTrigger = false;
            mathSign = true;
        }
    }
    // equal button checking reset
    wasEqualButton = false;

    // end of special conditions checks

    // store current value in main display
    if (!wasMathSign) {
        QString qstrDisplayValue = ui->MainDisplay->text();
        dLeftValue = qstrDisplayValue.toDouble();
    }
    // sender returns a pointer to the button pressed
    QPushButton* Button = static_cast<QPushButton*>(sender());
    // set math symbol on the button
    QString qstrButtonValue = Button->text();

    // find the value of button
    if (QString::compare(qstrButtonValue, "/", Qt::CaseSensitive) == 0) { //Qt::CaseInsensitive {
        // call a function that keeps the entered value
        inputHistory(qstrButtonValue);
        // make sure the latest input was not a math symbol
        // if it, replace last math symbol to current value
        replaceMathSign();
        // math sign was the last input
        wasMathSign = true;
        // set current math trigger
        divisionTrigger = true;
    } else if (QString::compare(qstrButtonValue, "*", Qt::CaseSensitive) == 0) {
        // equal;
        inputHistory(qstrButtonValue);
        replaceMathSign();
        wasMathSign = true;
        multiplicationTrigger = true;
    } else if (QString::compare(qstrButtonValue, "-", Qt::CaseSensitive) == 0) {
        // equal;
        inputHistory(qstrButtonValue);
        replaceMathSign();
        wasMathSign = true;
        subtractionTrigger = true;
    } else if (QString::compare(qstrButtonValue, "+", Qt::CaseSensitive) == 0) {
        // equal;
        inputHistory(qstrButtonValue);
        replaceMathSign();
        wasMathSign = true;
        additiveTrigger = true;
    }

    // Clear display

    // if there's no sequence or math symbol was latest input
    // and expression don't has a dot
   if ( (!sequence || wasMathSign) && !dotEntered) {
        if (ui->MainDisplay->text().length() >= 14) {
            // clear main display
             ui->MainDisplay->setText("");
            // set result in display and allow up to 10 digits
            // before using exponents
            ui->MainDisplay->setText(QString::number(dResult, 'g', 10));
        } else {
            // clear main display
             ui->MainDisplay->setText("");
            // set result in display and allow up to 14 digits
            // before using exponents
            ui->MainDisplay->setText(QString::number(dResult, 'g', 14));
        }
    } else {
       // clear main display without any other actions
        ui->MainDisplay->setText("");
    }
    if (dResult >= 1e+300 || dResult <= (-1e+300)) {
       // clear all
       clear();
       // set the message to main display
       ui->MainDisplay->setText("Overflow");
    }
}



// equal button was pressed
void Calculator::equalButtonPressed()
{
    // Holds new calculation
    dResult = 0.0;

    // get value in display
    QString qstrDisplayValue = ui->MainDisplay->text();
    double dDisplayValue = qstrDisplayValue.toDouble();

    // holds right value
    if (!wasEqualButton) {
        dRightValue = qstrDisplayValue.toDouble();
    }

    // trigger division by zero
    bool divisionByZero = false;

    // make sure a math button was pressed
    if (additiveTrigger || subtractionTrigger
            || multiplicationTrigger|| divisionTrigger) {

        // repeat pressing of equal button
        if (wasEqualButton) {
            dLeftValue = dRightValue;
        }

        if (additiveTrigger) {                      // additive trigger
            dResult = dLeftValue + dDisplayValue;
        } else if (subtractionTrigger) {            // subtraction trigger
            dResult = dLeftValue - dDisplayValue;
        } else if (multiplicationTrigger) {         // multiplication trigger
            dResult = dLeftValue * dDisplayValue;
        } else {                                    // division trigger
            if (dDisplayValue == 0.0) {             // if right value is 0
                divisionByZero = true;
            } else {
                dResult = dLeftValue / dDisplayValue;
            }
        }
    } else { // clicking the equal button without expression
        dResult = dDisplayValue;
    }

    // put result in display
    // if division by zero was not detected
    if (!divisionByZero) {
        // set result in display and allow up to 14
        // digits before using exponents
        ui->MainDisplay->setText(QString::number(dResult, 'g', 14));
        if (ui->MainDisplay->text().length() > 14) {
            // set result in display and allow up to 14
            // digits before using exponents
            ui->MainDisplay->setText(QString::number(dResult, 'g', 10));
        }
        // if no sequence, adds "-> " to side display result
        if (!sequence) {
            ui->SideDisplay->setText("-> " + qstrInputKeeper);
        }
        // if sequence and side display length is more then 31 characters
        if (sequence && ui->SideDisplay->text().length() >= 31) {
            // clear side display
            ui->SideDisplay->setText("");
            // clear both input keeper strings
            qstrInputKeeper = "";
            qstrSecondInputKeeper = "";
            // call a function that keeps the entered value
            inputHistory(QString::number(dResult, 'g', 14));
        }
    } else { // if division by zero, set the message to main display
        ui->MainDisplay->setText("Division by zero");
    }
    // if result is more then 1e+300 or less then 1e-300
    if (dResult >= 1e+300 || dResult <= (-1e+300)) {
        // clear all
        clear();
        // set the message to main display
        ui->MainDisplay->setText("Overflow");
    }
    // set dot and math triggers to false
    mathSign = false;
    wasMathSign = false;
    dotEntered = false;
    // trigger to check that last button pressed is equal button
    wasEqualButton = true;
}




// change sign button was pressed
void Calculator::changeSign()
{
    // keep display value
    QString qstrDisplayValue = ui->MainDisplay->text();
    // if display value contains numbers
    QRegularExpression re("[-]?[0-9.]*");
    QRegularExpressionMatch match = re.match(qstrDisplayValue);
    if (match.hasMatch()) {
        // changing sign by multiplying minus one
        double dDisplayValue = qstrDisplayValue.toDouble();
        double dDisplayValueSign = dDisplayValue * (-1);
        // set value into displays
        ui->MainDisplay->setText(QString::number(dDisplayValueSign));
        ui->SideDisplay->setText(QString::number(dDisplayValueSign));

        // change sign in the expression on the side display
        // find last index of "sign" and replace it by the index
        int index;

        // if side display don't contains "->" or expression has a sequence
        // or any triggers were detected
        if (!wasEqualButton && (mathSign // !qstrInputKeeper.contains("->")
                || (additiveTrigger || subtractionTrigger
                            || multiplicationTrigger|| divisionTrigger)) ) {
            if (qstrInputKeeper.contains("-") || qstrInputKeeper.contains("+")) {
                // find last index in expression
                int minus = qstrInputKeeper.lastIndexOf("-");
                int plus = qstrInputKeeper.lastIndexOf("+");
                int multCheck = qstrInputKeeper.lastIndexOf("*");
                int divCheck = qstrInputKeeper.lastIndexOf("/");
                // if expression starts with a minus
                if (qstrInputKeeper[0] == "-" && qstrInputKeeper.lastIndexOf("-") == 0) {
                    // delete the minus
                    qstrInputKeeper.replace("-","");
                // if minus has the latest index
                } else if (minus > plus && minus > multCheck && minus > divCheck) {
                    index = minus;
                    // replace minus to plus
                    qstrInputKeeper.replace(index, 1, "+");
                // if plus has the latest index
                } else if (plus > minus && plus > multCheck && plus > divCheck) {
                    index = plus;
                    // replace plus to minus
                    qstrInputKeeper.replace(index, 1, "-");
                }
            }
            // show it up in side display
            ui->SideDisplay->setText(qstrInputKeeper);
        // if equal button was pressed before changing sign
        } else if (wasEqualButton) {
            // clear all
            clear();
            // define input keeper as a display value
            qstrInputKeeper = QString::number(dDisplayValueSign);
            // set both displays
            ui->MainDisplay->setText(QString::number(dDisplayValueSign));
            ui->SideDisplay->setText(QString::number(dDisplayValueSign));
        }
    }
}




// keeping the expression and set side display
void Calculator::inputHistory(QString qStr)
{
    // define temp string as empty string
    QString historyString = "";
    if (qStr == "/" || qStr == "*" || qStr == "-" || qStr == "+") {
        // if math sign was detected
        // add it to second input keeper string
        qstrSecondInputKeeper = qstrInputKeeper + qStr;
    } else {
        // if no math signs, just keep the input in the temp string
        historyString = qStr;
    }

    // if math sign in the expression
    if (qstrSecondInputKeeper.length() != 0) {
        // main input keeper string is addition of left value, math sign
        // and right value
        qstrInputKeeper =  qstrSecondInputKeeper + historyString;
    } else {
        // if no math signs, just keep input in the main string
        qstrInputKeeper = historyString;
    }
    // set side display
    ui->SideDisplay->setText(qstrInputKeeper);
}




// clear all function
void Calculator::clear()
{
    // reset all
    dResult = 0.0;
    dLeftValue = 0.0;
    dRightValue = 0.0;
    qstrInputKeeper = "";
    qstrSecondInputKeeper = "";
    // clear both displays
    ui->MainDisplay->setText("");
    ui->SideDisplay->setText("");

    // set all triggers and checkers to false
    divisionTrigger = false;
    multiplicationTrigger = false;
    additiveTrigger = false;
    subtractionTrigger = false;

    mathSign = false;
    sequence = false;
    wasMathSign = false;
    dotEntered = false;
    wasEqualButton = false;
}




// baskspace (delete last character)
void Calculator::backspace()
{
    QString qstrDisplayValue = ui->MainDisplay->text();
    // chop(n) emoves n characters from the end of the string
    if (ui->MainDisplay->text().length() >= 1 && qstrInputKeeper.length() >= 1) {
        // do not delete the math sign
        if (!wasMathSign
                && (qstrInputKeeper[qstrInputKeeper.length() - 1] !=  "*"
                && qstrInputKeeper[qstrInputKeeper.length() - 1] !=  "/"
                && qstrInputKeeper[qstrInputKeeper.length() - 1] !=  "+"
                && qstrInputKeeper[qstrInputKeeper.length() - 1] !=  "-") ) {
            // delete rightmost character in main display
            qstrDisplayValue.chop(1);
            // if string is empty, set '0' in display
            if (qstrDisplayValue.isEmpty()) {
                qstrDisplayValue = "0";
            }
            // set main display
            ui->MainDisplay->setText(qstrDisplayValue);
            // delete rightmost character in side display
            // if equal button was not pressed before
            if (!wasEqualButton
                    && qstrInputKeeper[qstrInputKeeper.length() - 1] !=  ")") {
                if (qstrInputKeeper.length() == 1) {
                    qstrInputKeeper = "";
                } else {
                    qstrInputKeeper.chop(1);
                }
                    ui->SideDisplay->setText(qstrInputKeeper);
            } else {
                clear();
                qstrInputKeeper = "0";
            }
        }
    }
}




// adds dot(point) to display value
void Calculator::dot()
{
    // if last input was not a math sign button
    if (!wasMathSign) {
        // if last input was a equal button
        if (wasEqualButton) {
            // set main display to empty
            ui->MainDisplay->setText("");
        }
        // if main display don't have dot yet
        if ( !(ui->MainDisplay->text().contains('.')) ) {
            // if main display is empty
            if (ui->MainDisplay->text() == "") {
                // set main display to "0."
                ui->MainDisplay->setText("0" + tr("."));
            } else {
                // if main display is not empty, add dot to current value
                ui->MainDisplay->setText(ui->MainDisplay->text() + tr("."));
            }
            // if current value in side display is zero
            if (qstrInputKeeper.length() == 0) {
                // set the side display to "0."
                ui->SideDisplay->setText("0" + tr("."));
            }
            dotEntered = true;
        }
    }
}




// square root extraction
void Calculator::squareRoot()
{
    // if last input was not a math sign button
    if (!wasMathSign) {
        // extract square root by using sqrt function from <cmath>
        QString sqrtRoot = ui->MainDisplay->text();
        double dResult = sqrt(sqrtRoot.toDouble());
        if (!wasEqualButton) {
            inputHistory("sqrt(" + sqrtRoot + ")");
        } else {
            clear();
            inputHistory("sqrt(" + sqrtRoot + ")");
        }
        ui->MainDisplay->setText(QString::number(dResult, 'g', 14));
    }
}




void Calculator::square()
{
    // if last input was not a math sign button
    if (!wasMathSign) {
        // finding square degree of the value by using sqr function from <cmath>
        QString square = ui->MainDisplay->text();
        double dResult = pow(square.toDouble(), 2);
        if (!wasEqualButton) {
            inputHistory("sqr(" + square + ")");
        } else {
            clear();
            inputHistory("sqr(" + square + ")");
        }
        ui->MainDisplay->setText(QString::number(dResult, 'g', 14));
    }
}




// non-slot function

// check for multiply entered math signs
void Calculator::replaceMathSign()
{
    // if expression has left value and sign
    if (qstrInputKeeper.length() >= 2) {
        // (position, replace_length, what_is_being_replaced)
        // replace math sign to current value (in mathButtonPressed() function)
        qstrInputKeeper.replace( (qstrInputKeeper.length() - 1), 1,"");
    }
}
