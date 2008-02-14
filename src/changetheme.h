#ifndef CHANGETHEME_H
#define CHANGETHEME_H

#include <QWidget>
#include <QDialog>

#include "ui_changetheme.h"

class ChangeTheme : public QWidget, public Ui::ChangeTheme {
    Q_OBJECT
public:
    ChangeTheme(const QString &defaultPieceTheme, const QString &defaultSquareTheme, QWidget *parent);
    ~ChangeTheme();

Q_SIGNALS:
    void pieceThemeChanged(const QString &theme);
    void squareThemeChanged(const QString &theme);

private Q_SLOTS:
    void fillPieceThemeList(const QString &theme);
    void fillSquareThemeList(const QString &theme);
};

class ChangeThemeDialog : public QDialog {
    Q_OBJECT
public:
    ChangeThemeDialog(const QString &defaultPieceTheme, const QString &defaultSquareTheme, QWidget *parent);
    ~ChangeThemeDialog();

Q_SIGNALS:
    void pieceThemeChanged(const QString &theme);
    void squareThemeChanged(const QString &theme);
};

#endif
