#include "aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);
    ui_iconLabel->setPixmap(QPixmap(":icons/board.png"));
}

AboutDialog::~AboutDialog()
{
}
