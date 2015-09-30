#ifndef PGNLEXER_H
#define PGNLEXER_H

#include <QDebug>
#include <QVector>
#include <QByteArray>
#include <QBuffer>

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

class PgnLexer : public QObject {
    Q_OBJECT
public:
    PgnLexer(QObject *parent = 0);
    ~PgnLexer();

    PgnTokenStream lex(const QByteArray &text);

Q_SIGNALS:
    void progress(qint64 pos, qint64 size);

private:
    bool nextToken(PgnToken &tok);

    void scanString(QBuffer *stream);
    void scanPeriod(QBuffer *stream);
    void scanAsterisk(QBuffer *stream);
    void scanLeftBrack(QBuffer *stream);
    void scanRightBrack(QBuffer *stream);
    void scanLeftParen(QBuffer *stream);
    void scanRightParen(QBuffer *stream);
    void scanLeftAngle(QBuffer *stream);
    void scanRightAngle(QBuffer *stream);
    void scanNAG(QBuffer *stream);
    void scanIntegerOrSymbol(QBuffer *stream, bool integer);

private:
    qint64 m_size;
    QVector<PgnToken> m_tokens;
};

#endif
