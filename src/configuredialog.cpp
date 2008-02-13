#include "configuredialog.h"

#include <QDebug>
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QCoreApplication>

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

    connect(ui_pieceTheme, SIGNAL(activated(const QString &)),
            this, SLOT(pieceThemeChanged()));

    connect(ui_squareTheme, SIGNAL(activated(const QString &)),
            this, SLOT(squareThemeChanged()));

    fillPieceThemeList();
    fillSquareThemeList();
    fillEngineList();

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

void ConfigureDialog::pieceThemeChanged()
{
    QSettings settings;
    settings.beginGroup("Themes");
    settings.setValue("piecesTheme", ui_pieceTheme->currentText());
    settings.endGroup();
}

void ConfigureDialog::squareThemeChanged()
{
    QSettings settings;
    settings.beginGroup("Themes");
    settings.setValue("squaresTheme", ui_squareTheme->currentText());
    settings.endGroup();
}

void ConfigureDialog::fillPieceThemeList()
{
    QDir appDirectory(QCoreApplication::applicationDirPath());
    appDirectory.cdUp();

    QDir themeDirectory(QString(appDirectory.canonicalPath() +
                                QDir::separator() +
                                "themes" +
                                QDir::separator() +
                                "pieces"));

    QStringList pieceThemes = themeDirectory.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    ui_pieceTheme->addItems(pieceThemes);

    QSettings settings;
    settings.beginGroup("Themes");
    QString theme = settings.value("piecesTheme", "default").toString();
    ui_pieceTheme->setCurrentIndex(ui_pieceTheme->findText(theme));
    settings.endGroup();
}

void ConfigureDialog::fillSquareThemeList()
{
    QDir appDirectory(QCoreApplication::applicationDirPath());
    appDirectory.cdUp();

    QDir themeDirectory(QString(appDirectory.canonicalPath() +
                                QDir::separator() +
                                "themes" +
                                QDir::separator() +
                                "squares"));

    QStringList humanReadable;
    QStringList squareThemes = themeDirectory.entryList(QDir::Files);
    foreach (QString theme, squareThemes) {
        humanReadable << theme.replace(".castle", "");
    }

    ui_squareTheme->addItems(humanReadable);

    QSettings settings;
    settings.beginGroup("Themes");
    QString theme = settings.value("squaresTheme", "default").toString();
    ui_squareTheme->setCurrentIndex(ui_squareTheme->findText(theme));
    settings.endGroup();
}
