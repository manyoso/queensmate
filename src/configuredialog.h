#ifndef CONFIGUREDIALOG_H
#define CONFIGUREDIALOG_H

#include <QDialog>

#include "ui_configuredialog.h"

class ConfigureDialog : public QDialog, public Ui::ConfigureDialog {
    Q_OBJECT
public:
    ConfigureDialog(QWidget *parent);
    ~ConfigureDialog();

private Q_SLOTS:
    void addEngine();
    void modifyEngine();
    void deleteEngine();
    void fillEngineList();
};
#endif
