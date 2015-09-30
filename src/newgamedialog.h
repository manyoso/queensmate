#ifndef NEWGAMEDIALOG_H
#define NEWGAMEDIALOG_H

#include <QDialog>

#include "ui_newgamedialog.h"

class NewGameDialog : public QDialog, public Ui::NewGameDialog {
    Q_OBJECT
public:
    NewGameDialog(QWidget *parent);
    ~NewGameDialog();

    bool isClassicalChess() const { return ui_classicalChess->isChecked(); }
    bool isRandom960() const { return ui_random960->isChecked(); }
    int position960() const { return ui_position960->value(); }

    bool whiteIsHuman() const { return ui_whiteHuman->isChecked(); }
    bool blackIsHuman() const { return ui_blackHuman->isChecked(); }

    QString whiteComputer() const { return ui_whiteComputerCombo->currentText(); }
    QString blackComputer() const { return ui_blackComputerCombo->currentText(); }

    QString whiteClockType() const { return ui_whiteClockType->currentText(); }
    QString blackClockType() const { return ui_blackClockType->currentText(); }

    int whiteBase() const { return ui_whiteBase->value(); }
    int blackBase() const { return ui_blackBase->value(); }

    int whiteMove() const { return ui_whiteMove->value(); }
    int blackMove() const { return ui_blackMove->value(); }

    int whiteIncrement() const { return ui_whiteIncrement->value(); }
    int blackIncrement() const { return ui_blackIncrement->value(); }

    void setTypeVisible(bool visible) { ui_typeBox->setVisible(visible); }

private Q_SLOTS:
    void setIsClassicalChess(bool isClassicalChess);
    void setRandom960(bool random);
    void setPosition960(int position);

    void setWhiteIsHuman(bool whiteIsHuman);
    void setBlackIsHuman(bool blackIsHuman);

    void setWhiteComputer(const QString &computer);
    void setBlackComputer(const QString &computer);

    void setWhiteClockType(const QString &clockType);
    void setBlackClockType(const QString &clockType);

    void setWhiteBase(int value);
    void setWhiteIncrement(int value);
    void setWhiteMove(int value);

    void setBlackBase(int value);
    void setBlackIncrement(int value);
    void setBlackMove(int value);
};
#endif
