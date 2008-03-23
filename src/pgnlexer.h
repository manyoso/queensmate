#ifndef PGNLEXER_H
#define PGNLEXER_H

#include <QDebug>
#include <QVector>
#include <QByteArray>
#include <QTextStream>

//FIXME Comments!

class PgnToken
{
public:
    enum Type
    {
        Unknown,
        String,
        Period,
        Asterisk,
        LeftBrack,
        RightBrack,
        LeftParen,
        RightParen,
        LeftAngle,
        RightAngle,
        NAG,
        Integer,
        Symbol,
    };

    PgnToken();
    PgnToken(int _start, int _length, Type _type);
    ~PgnToken();

    int start;
    int length;
    Type type;

    QString toString() const;
};

QDebug operator<<(QDebug, const PgnToken &);

class PgnTokenStream {
public:
    PgnTokenStream(const QByteArray &text, const QVector<PgnToken> &tokens);
    ~PgnTokenStream();

    int pos();
    void next();
    void seek(int pos);
    bool atEnd();
    PgnToken token();
    PgnToken::Type lookAhead(int pos = 0);
    QByteArray text();

private:
    int m_pos;
    QByteArray m_text;
    QVector<PgnToken> m_tokens;
};

class PgnLexer {
public:
    PgnLexer();
    ~PgnLexer();

    PgnTokenStream lex(const QByteArray &text);

private:
    bool nextToken(PgnToken &tok);

    void scanString(QTextStream *stream);
    void scanPeriod(QTextStream *stream);
    void scanAsterisk(QTextStream *stream);
    void scanLeftBrack(QTextStream *stream);
    void scanRightBrack(QTextStream *stream);
    void scanLeftParen(QTextStream *stream);
    void scanRightParen(QTextStream *stream);
    void scanLeftAngle(QTextStream *stream);
    void scanRightAngle(QTextStream *stream);
    void scanNAG(QTextStream *stream);
    void scanIntegerOrSymbol(QTextStream *stream, bool integer);

private:
    QVector<PgnToken> m_tokens;
};

#endif
