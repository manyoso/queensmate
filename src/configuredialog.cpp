#include "configuredialog.h"

#include <QDebug>
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QCoreApplication>

#include "uciengine.h"
#include "changetheme.h"

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

    QSettings settings;
    settings.beginGroup("Themes");
    QString piecesTheme = settings.value("piecesTheme", "default").toString();
    QString squaresTheme = settings.value("squaresTheme", "default").toString();
    settings.endGroup();

    ChangeTheme *changeTheme = new ChangeTheme(piecesTheme, squaresTheme, ui_themePage);
    connect(changeTheme, SIGNAL(pieceThemeChanged(const QString &)),
            this, SLOT(pieceThemeChanged(const QString &)));

    connect(changeTheme, SIGNAL(squareThemeChanged(const QString &)),
            this, SLOT(squareThemeChanged(const QString &)));

    QVBoxLayout *layout = new QVBoxLayout(ui_themePage);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(changeTheme);
    ui_themePage->setLayout(layout);

    ui_pageList->item(0)->setHidden(true); //general
    ui_pageList->setCurrentRow(1); //themes
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

    //qDebug() << "Engine name:" << engineName << endl;

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

void ConfigureDialog::pieceThemeChanged(const QString &theme)
{
    QSettings settings;
    settings.beginGroup("Themes");
    settings.setValue("piecesTheme", theme);
    settings.endGroup();
}

void ConfigureDialog::squareThemeChanged(const QString &theme)
{
    QSettings settings;
    settings.beginGroup("Themes");
    settings.setValue("squaresTheme", theme);
    settings.endGroup();
}
