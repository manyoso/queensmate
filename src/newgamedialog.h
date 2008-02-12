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

private Q_SLOTS:
    void setIsClassicalChess(bool isClassicalChess);
    void setRandom960(bool random);
    void setPosition960(int position);

    void setWhiteIsHuman(bool whiteIsHuman);
    void setBlackIsHuman(bool blackIsHuman);

    void setWhiteComputer(const QString &computer);
    void setBlackComputer(const QString &computer);
};
#endif
