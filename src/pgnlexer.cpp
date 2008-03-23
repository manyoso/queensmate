#include "pgnlexer.h"

#include <QDebug>

PgnToken::PgnToken()
    : start(0), length(0), type(Unknown)
{
}

PgnToken::PgnToken(int _start, int _length, Type _type)
    : start(_start), length(_length), type(_type)
{
}

PgnToken::~PgnToken()
{
}

QString PgnToken::toString() const
{
    const QString s = QString::number(start);
    const QString l = QString::number(length);

    QString t;
    switch (type) {
    case Unknown: t = "Unknown"; break;
    case String: t = "String"; break;
    case Period: t = "Period"; break;
    case Asterisk: t = "Asterisk"; break;
    case LeftBrack: t = "LeftBrack"; break;
    case RightBrack: t = "RightBrack"; break;
    case LeftParen: t = "LeftParen"; break;
    case RightParen: t = "RightParen"; break;
    case LeftAngle: t = "LeftAngle"; break;
    case RightAngle: t = "RightAngle"; break;
    case NAG: t = "NAG"; break;
    case Integer: t = "Integer"; break;
    case Symbol: t = "Symbol"; break;
    default: break;
    }

    return QString("start: %1 length: %2 type: %3").arg(s).arg(l).arg(t);
}

QDebug operator<<(QDebug debug, const PgnToken &token)
{
    debug.nospace() << "PgnToken(" << token.toString() << ")\n";
    return debug.space();
}

PgnTokenStream::PgnTokenStream(const QByteArray &text, const QVector<PgnToken> &tokens)
    : m_pos(0),
      m_text(text),
      m_tokens(tokens)
{
}

PgnTokenStream::~PgnTokenStream()
{
}

int PgnTokenStream::pos()
{
    return m_pos;
}

void PgnTokenStream::next()
{
    if (m_pos < m_tokens.count())
        ++m_pos;
}

void PgnTokenStream::seek(int pos)
{
    if (pos >= 0 && pos <= m_tokens.count())
        m_pos = pos;
}

bool PgnTokenStream::atEnd()
{
    return m_pos == m_tokens.count();
}

PgnToken PgnTokenStream::token()
{
   if (m_pos >= 0 && m_pos <= m_tokens.count() - 1)
       return m_tokens.at(m_pos);
   else
       return PgnToken();
}

PgnToken::Type PgnTokenStream::lookAhead(int pos)
{
    int look = m_pos + pos;
    if (look >= 0 && look <= m_tokens.count() - 1)
        return m_tokens.at(look).type;
    else
        return PgnToken::Unknown;
}

QByteArray PgnTokenStream::text()
{
    if (m_pos >= 0 && m_pos <= m_tokens.count() - 1)
        return m_text.mid(m_tokens.at(m_pos).start, m_tokens.at(m_pos).length).trimmed();
    else
        return QByteArray();
}

PgnLexer::PgnLexer()
{
}

PgnLexer::~PgnLexer()
{
}

PgnTokenStream PgnLexer::lex(const QByteArray &text)
{
    m_tokens.clear();

    QTextStream stream(text);

    while (!stream.atEnd()) {

        stream.skipWhiteSpace();
        if (stream.atEnd())
            break;

        QChar c;
        stream >> c;
        if (c == '"')
            scanString(&stream);
        else if (c == '.')
            scanPeriod(&stream);
        else if (c == '*')
            scanAsterisk(&stream);
        else if (c == '[')
            scanLeftBrack(&stream);
        else if (c == ']')
            scanRightBrack(&stream);
        else if (c == '(')
            scanLeftParen(&stream);
        else if (c == ')')
            scanRightParen(&stream);
        else if (c == '<')
            scanLeftAngle(&stream);
        else if (c == '>')
            scanRightAngle(&stream);
        else if (c == '$')
            scanNAG(&stream);
        else if (c.isDigit())
            scanIntegerOrSymbol(&stream, true);
        else if (c.isLetter())
            scanIntegerOrSymbol(&stream, false);
        else
            qDebug() << "Unknown char:at" << stream.pos() << endl;
    }

    return PgnTokenStream(text, m_tokens);
}

void PgnLexer::scanString(QTextStream *stream)
{
//     qDebug() << "scanString start pos:" << stream->pos() << endl;
    PgnToken token;
    token.start = stream->pos() - 1;

    while (!stream->atEnd() || stream->pos() - token.start == 255) {

        stream->skipWhiteSpace();
        if (stream->atEnd()) {
            break;
        }

        QChar c;
        *stream >> c;
        if (c == '\\') {
            stream->seek(stream->pos() + 1);
        } else if (c == '"') {
            break;
        }
    }

    token.length = stream->pos() - token.start;

    token.type = PgnToken::String;
    m_tokens << token;
//     qDebug() << "scanString end pos:" << stream->pos() << endl;
}

void PgnLexer::scanPeriod(QTextStream *stream)
{
    PgnToken token;
    token.start = stream->pos() - 1;
    token.length = 1;
    token.type = PgnToken::Period;
    m_tokens << token;
}

void PgnLexer::scanAsterisk(QTextStream *stream)
{
    PgnToken token;
    token.start = stream->pos() - 1;
    token.length = 1;
    token.type = PgnToken::Asterisk;
    m_tokens << token;
}

void PgnLexer::scanLeftBrack(QTextStream *stream)
{
//     qDebug() << "scanLeftBrack start pos:" << stream->pos() << endl;
    PgnToken token;
    token.start = stream->pos() - 1;
    token.length = 1;
    token.type = PgnToken::LeftBrack;
    m_tokens << token;
//     qDebug() << "scanLeftBrack end pos:" << stream->pos() << endl;
}

void PgnLexer::scanRightBrack(QTextStream *stream)
{
    PgnToken token;
    token.start = stream->pos() - 1;
    token.length = 1;
    token.type = PgnToken::RightBrack;
    m_tokens << token;
}

void PgnLexer::scanLeftParen(QTextStream *stream)
{
    PgnToken token;
    token.start = stream->pos() - 1;
    token.length = 1;
    token.type = PgnToken::LeftParen;
    m_tokens << token;
}

void PgnLexer::scanRightParen(QTextStream *stream)
{
    PgnToken token;
    token.start = stream->pos() - 1;
    token.length = 1;
    token.type = PgnToken::RightParen;
    m_tokens << token;
}

void PgnLexer::scanLeftAngle(QTextStream *stream)
{
    PgnToken token;
    token.start = stream->pos() - 1;
    token.length = 1;
    token.type = PgnToken::LeftAngle;
    m_tokens << token;
}

void PgnLexer::scanRightAngle(QTextStream *stream)
{
    PgnToken token;
    token.start = stream->pos() - 1;
    token.length = 1;
    token.type = PgnToken::RightAngle;
    m_tokens << token;
}

void PgnLexer::scanNAG(QTextStream *stream)
{
    PgnToken token;
    token.start = stream->pos() - 1;

    while (!stream->atEnd()) {

        stream->skipWhiteSpace();
        if (stream->atEnd()) {
            break;
        }

        QChar c;
        *stream >> c;
        if (!c.isDigit()) {
            stream->seek(stream->pos() - 1);
            break;
        }
    }

    token.length = stream->pos() - token.start;

    token.type = PgnToken::NAG;
    m_tokens << token;
}

void PgnLexer::scanIntegerOrSymbol(QTextStream *stream, bool integer)
{
//     qDebug() << "scanIntegerOrSymbol start pos:" << stream->pos() << endl;
    PgnToken token;
    token.start = stream->pos() - 1;

    while (!stream->atEnd() || stream->pos() - token.start == 255) {

        if (stream->atEnd()) {
            break;
        }

        QChar c;
        *stream >> c;
        if (c == '_') {
            integer = false;
        } else if (c == '+') {
            integer = false;
        } else if (c == '#') {
            integer = false;
        } else if (c == '=') {
            integer = false;
        } else if (c == ':') {
            integer = false;
        } else if (c == '-') {
            integer = false;
        } else if (c == '/') {
            integer = false;
        } else if (c.isLetter()) {
            integer = false;
        } else if (c.isDigit()) {
            ; //do nothing
        } else {
            stream->seek(stream->pos() - 1);
            break;
        }
    }

    token.length = stream->pos() - token.start;

    if (integer)
        token.type = PgnToken::Integer;
    else
        token.type = PgnToken::Symbol;
    m_tokens << token;

//     qDebug() << "scanIntegerOrSymbol end pos:" << stream->pos() << endl;
}
