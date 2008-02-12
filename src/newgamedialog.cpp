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
    int blackComputer = ui_whiteComputerCombo->findText(settings.value("whiteComputer").toString());
    ui_blackComputerCombo->setCurrentIndex(qMax(0, blackComputer));

    settings.endGroup();
}

NewGameDialog::~NewGameDialog()
{
}

void NewGameDialog::setIsClassicalChess(bool isClassicalChess)
{
    QSettings settings;
    settings.beginGroup("NewGameDialog");
    settings.setValue("isClassicalChess", isClassicalChess);
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
