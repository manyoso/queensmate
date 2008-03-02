#include "newgamedialog.h"

#include <QSettings>

NewGameDialog::NewGameDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);

    ui_typeLabel->setPixmap(QPixmap(":icons/board.png"));
    ui_whiteLabel->setPixmap(QPixmap(":icons/white_king.png"));
    ui_blackLabel->setPixmap(QPixmap(":icons/black_king.png"));

    connect(ui_classicalChess, SIGNAL(toggled(bool)),
            this, SLOT(setIsClassicalChess(bool)));

    connect(ui_whiteHuman, SIGNAL(toggled(bool)),
            this, SLOT(setWhiteIsHuman(bool)));

    connect(ui_blackHuman, SIGNAL(toggled(bool)),
            this, SLOT(setBlackIsHuman(bool)));

    connect(ui_random960, SIGNAL(toggled(bool)),
            this, SLOT(setRandom960(bool)));

    connect(ui_position960, SIGNAL(valueChanged(int)),
            this, SLOT(setPosition960(int)));

    connect(ui_whiteComputerCombo, SIGNAL(activated(const QString &)),
            this, SLOT(setWhiteComputer(const QString &)));

    connect(ui_blackComputerCombo, SIGNAL(activated(const QString &)),
            this, SLOT(setBlackComputer(const QString &)));

    connect(ui_whiteClockType, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(setWhiteClockType(const QString &)));

    connect(ui_blackClockType, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(setBlackClockType(const QString &)));

    connect(ui_whiteBase, SIGNAL(valueChanged(int)),
            this, SLOT(setWhiteBase(int)));

    connect(ui_whiteIncrement, SIGNAL(valueChanged(int)),
            this, SLOT(setWhiteIncrement(int)));

    connect(ui_whiteMove, SIGNAL(valueChanged(int)),
            this, SLOT(setWhiteMove(int)));

    connect(ui_blackBase, SIGNAL(valueChanged(int)),
            this, SLOT(setBlackBase(int)));

    connect(ui_blackIncrement, SIGNAL(valueChanged(int)),
            this, SLOT(setBlackIncrement(int)));

    connect(ui_blackMove, SIGNAL(valueChanged(int)),
            this, SLOT(setBlackMove(int)));

    QSettings settings;
    settings.beginGroup("Engines");
    QStringList engines = settings.allKeys();
    ui_whiteComputerCombo->addItems(engines);
    ui_blackComputerCombo->addItems(engines);
    settings.endGroup();

    settings.beginGroup("NewGameDialog");
    ui_classicalChess->setChecked(settings.value("classicalChess", true).toBool());
    ui_modernChess->setChecked(!settings.value("classicalChess", true).toBool());
    ui_random960->setChecked(settings.value("random960", true).toBool());
    ui_position960->setValue(settings.value("position960", 518).toInt());
    ui_whiteHuman->setChecked(settings.value("whiteIsHuman", true).toBool());
    ui_whiteComputer->setChecked(!settings.value("whiteIsHuman", true).toBool());
    ui_blackHuman->setChecked(settings.value("blackIsHuman", true).toBool());
    ui_blackComputer->setChecked(!settings.value("blackIsHuman", true).toBool());

    int whiteComputer = ui_whiteComputerCombo->findText(settings.value("whiteComputer").toString());
    ui_whiteComputerCombo->setCurrentIndex(qMax(0, whiteComputer));
    int blackComputer = ui_whiteComputerCombo->findText(settings.value("blackComputer").toString());
    ui_blackComputerCombo->setCurrentIndex(qMax(0, blackComputer));

    int whiteClockType = ui_whiteClockType->findText(settings.value("whiteClockType").toString());
    ui_whiteClockType->setCurrentIndex(qMax(0, whiteClockType));
    int blackClockType = ui_blackClockType->findText(settings.value("blackClockType").toString());
    ui_blackClockType->setCurrentIndex(qMax(0, blackClockType));

    settings.endGroup();
}

NewGameDialog::~NewGameDialog()
{
}

void NewGameDialog::setIsClassicalChess(bool isClassicalChess)
{
    QSettings settings;
    settings.beginGroup("NewGameDialog");
    settings.setValue("classicalChess", isClassicalChess);
    settings.endGroup();

    ui_random960->setEnabled(!isClassicalChess);
    ui_position960->setEnabled(!ui_random960->isChecked() && !isClassicalChess);
}

void NewGameDialog::setRandom960(bool random)
{
    QSettings settings;
    settings.beginGroup("NewGameDialog");
    settings.setValue("random960", random);
    ui_position960->setEnabled(!random);
    settings.endGroup();
}

void NewGameDialog::setPosition960(int position)
{
    QSettings settings;
    settings.beginGroup("NewGameDialog");
    settings.setValue("position960", position);
    settings.endGroup();
}

void NewGameDialog::setWhiteIsHuman(bool whiteIsHuman)
{
    QSettings settings;
    settings.beginGroup("NewGameDialog");
    settings.setValue("whiteIsHuman", whiteIsHuman);
    settings.endGroup();
}

void NewGameDialog::setBlackIsHuman(bool blackIsHuman)
{
    QSettings settings;
    settings.beginGroup("NewGameDialog");
    settings.setValue("blackIsHuman", blackIsHuman);
    settings.endGroup();
}

void NewGameDialog::setWhiteComputer(const QString &computer)
{
    QSettings settings;
    settings.beginGroup("NewGameDialog");
    settings.setValue("whiteComputer", computer);
    settings.endGroup();
}

void NewGameDialog::setBlackComputer(const QString &computer)
{
    QSettings settings;
    settings.beginGroup("NewGameDialog");
    settings.setValue("blackComputer", computer);
    settings.endGroup();
}

void NewGameDialog::setWhiteClockType(const QString &clockType)
{
    QSettings settings;
    settings.beginGroup("NewGameDialog");
    settings.setValue("whiteClockType", clockType);
    settings.endGroup();

    ui_whiteBase->setEnabled(clockType == "Custom");
    ui_whiteIncrement->setEnabled(clockType == "Custom");
    ui_whiteMove->setEnabled(clockType == "Custom");

    if (clockType == "Unlimited") {
        ui_whiteBase->setValue(9999);
        ui_whiteIncrement->setValue(0);
        ui_whiteMove->setValue(-1);
    } else if (clockType == "Standard") {
        ui_whiteBase->setValue(15);
        ui_whiteIncrement->setValue(0);
        ui_whiteMove->setValue(-1);
    } else if (clockType == "Blitz") {
        ui_whiteBase->setValue(5);
        ui_whiteIncrement->setValue(0);
        ui_whiteMove->setValue(-1);
    } else if (clockType == "Lightning") {
        ui_whiteBase->setValue(2);
        ui_whiteIncrement->setValue(0);
        ui_whiteMove->setValue(-1);
    } else if (clockType == "Custom") {
        QSettings settings;
        settings.beginGroup("NewGameDialog");

        ui_whiteBase->setValue(settings.value("whiteBase", 15).toInt());
        ui_whiteIncrement->setValue(settings.value("whiteIncrement", 0).toInt());
        ui_whiteMove->setValue(settings.value("whiteMove", -1).toInt());

        settings.endGroup();
    }
}

void NewGameDialog::setBlackClockType(const QString &clockType)
{
    QSettings settings;
    settings.beginGroup("NewGameDialog");
    settings.setValue("blackClockType", clockType);
    settings.endGroup();

    ui_blackBase->setEnabled(clockType == "Custom");
    ui_blackIncrement->setEnabled(clockType == "Custom");
    ui_blackMove->setEnabled(clockType == "Custom");

    if (clockType == "Unlimited") {
        ui_blackBase->setValue(9999);
        ui_blackIncrement->setValue(0);
        ui_blackMove->setValue(-1);
    } else if (clockType == "Standard") {
        ui_blackBase->setValue(15);
        ui_blackIncrement->setValue(0);
        ui_blackMove->setValue(-1);
    } else if (clockType == "Blitz") {
        ui_blackBase->setValue(5);
        ui_blackIncrement->setValue(0);
        ui_blackMove->setValue(-1);
    } else if (clockType == "Lightning") {
        ui_blackBase->setValue(2);
        ui_blackIncrement->setValue(0);
        ui_blackMove->setValue(-1);
    } else if (clockType == "Custom") {
        QSettings settings;
        settings.beginGroup("NewGameDialog");

        ui_blackBase->setValue(settings.value("blackBase", 15).toInt());
        ui_blackIncrement->setValue(settings.value("blackIncrement", 0).toInt());
        ui_blackMove->setValue(settings.value("blackMove", -1).toInt());

        settings.endGroup();
    }
}

void NewGameDialog::setWhiteBase(int value)
{
    if (ui_whiteClockType->currentText() != "Custom")
        return;
    QSettings settings;
    settings.beginGroup("NewGameDialog");
    settings.setValue("whiteBase", value);
    settings.endGroup();
}

void NewGameDialog::setWhiteIncrement(int value)
{
    if (ui_whiteClockType->currentText() != "Custom")
        return;
    QSettings settings;
    settings.beginGroup("NewGameDialog");
    settings.setValue("whiteIncrement", value);
    settings.endGroup();
}

void NewGameDialog::setWhiteMove(int value)
{
    if (ui_whiteClockType->currentText() != "Custom")
        return;
    QSettings settings;
    settings.beginGroup("NewGameDialog");
    settings.setValue("whiteMove", value);
    settings.endGroup();
}

void NewGameDialog::setBlackBase(int value)
{
    if (ui_blackClockType->currentText() != "Custom")
        return;
    QSettings settings;
    settings.beginGroup("NewGameDialog");
    settings.setValue("blackBase", value);
    settings.endGroup();
}

void NewGameDialog::setBlackIncrement(int value)
{
    if (ui_blackClockType->currentText() != "Custom")
        return;
    QSettings settings;
    settings.beginGroup("NewGameDialog");
    settings.setValue("blackIncrement", value);
    settings.endGroup();
}

void NewGameDialog::setBlackMove(int value)
{
    if (ui_blackClockType->currentText() != "Custom")
        return;
    QSettings settings;
    settings.beginGroup("NewGameDialog");
    settings.setValue("blackMove", value);
    settings.endGroup();
}
