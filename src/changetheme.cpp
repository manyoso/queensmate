#include "changetheme.h"

#include <QDir>
#include <QDebug>
#include <QDialogButtonBox>

#include "theme.h"

ChangeTheme::ChangeTheme(const QString &defaultPieceTheme, const QString &defaultSquareTheme, QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);

    connect(ui_pieceTheme, SIGNAL(activated(const QString &)),
            this, SIGNAL(pieceThemeChanged(const QString &)));

    connect(ui_squareTheme, SIGNAL(activated(const QString &)),
            this, SIGNAL(squareThemeChanged(const QString &)));

    fillPieceThemeList(defaultPieceTheme);
    fillSquareThemeList(defaultSquareTheme);
}

ChangeTheme::~ChangeTheme()
{
}

void ChangeTheme::fillPieceThemeList(const QString &theme)
{
    QDir dir(Theme::pieceThemePath());
    QStringList pieceThemes = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    ui_pieceTheme->addItems(pieceThemes);
    ui_pieceTheme->setCurrentIndex(ui_pieceTheme->findText(theme));
}

void ChangeTheme::fillSquareThemeList(const QString &theme)
{
    QDir dir(Theme::squareThemePath());
    QStringList humanReadable;
    QStringList squareThemes = dir.entryList(QDir::Files);
    foreach (QString theme, squareThemes) {
        humanReadable << theme.replace(".castle", "");
    }
    ui_squareTheme->addItems(humanReadable);
    ui_squareTheme->setCurrentIndex(ui_squareTheme->findText(theme));
}

ChangeThemeDialog::ChangeThemeDialog(const QString &defaultPieceTheme, const QString &defaultSquareTheme, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Change theme for board"));
    ChangeTheme *changeTheme = new ChangeTheme(defaultPieceTheme, defaultSquareTheme, this);
    connect(changeTheme, SIGNAL(pieceThemeChanged(const QString &)),
            this, SIGNAL(pieceThemeChanged(const QString &)));

    connect(changeTheme, SIGNAL(squareThemeChanged(const QString &)),
            this, SIGNAL(squareThemeChanged(const QString &)));

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(changeTheme);
    layout->addWidget(buttonBox);
    setLayout(layout);
    setMinimumSize(300, 200);
}

ChangeThemeDialog::~ChangeThemeDialog()
{
}
