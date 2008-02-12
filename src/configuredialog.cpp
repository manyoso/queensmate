#include "configuredialog.h"

#include <QDebug>
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>

#include "uciengine.h"

ConfigureDialog::ConfigureDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);

    connect(ui_pageList, SIGNAL(currentRowChanged(int)),
            ui_stackedWidget, SLOT(setCurrentIndex(int)));

    connect(ui_addEngine, SIGNAL(pressed()),
            this, SLOT(addEngine()));

    connect(ui_modifyEngine, SIGNAL(pressed()),
            this, SLOT(modifyEngine()));

    connect(ui_deleteEngine, SIGNAL(pressed()),
            this, SLOT(deleteEngine()));

    fillEngineList();

    ui_pageList->item(0)->setHidden(true); //general
    ui_pageList->item(1)->setHidden(true); //themes
    ui_pageList->setCurrentRow(2); //engines
}

ConfigureDialog::~ConfigureDialog()
{
}

void ConfigureDialog::addEngine()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Select Computer Engine"));
    if (file.isEmpty() || !QFile::exists(file))
        return;

    QString engineName = UciEngine::engineName(file);
    if (engineName.isEmpty()) {
        QMessageBox::critical(this, tr("Error adding chess engine..."), tr("The engine does not identify itself properly!"));
        return;
    }

    qDebug() << "Engine name:" << engineName << endl;

    QSettings settings;
    settings.beginGroup("Engines");
    settings.setValue(engineName, file);
    settings.endGroup();

    fillEngineList();
}

void ConfigureDialog::modifyEngine()
{
//     QList<UciOption> list = UciEngine::optionList("/home/manyoso/dev/fruit_22/fruit_22");
//     foreach (UciOption option, list) {
//         qDebug() << option.toString() << endl;
//     }
}

void ConfigureDialog::deleteEngine()
{
    QSettings settings;
    settings.beginGroup("Engines");
    settings.remove(ui_engineList->currentItem()->text());
    settings.endGroup();

    fillEngineList();
}

void ConfigureDialog::fillEngineList()
{
    ui_engineList->clear();

    QSettings settings;
    settings.beginGroup("Engines");
    QStringList engines = settings.allKeys();
    ui_engineList->addItems(engines);
    settings.endGroup();
}
