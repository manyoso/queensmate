#ifndef SQUARE_H
#define SQUARE_H

#include <QList>
#include <QString>

class Square {
public:
    Square();
    Square(int file, int rank);
    ~Square();

    int file() const { return m_file; }
    int rank() const { return m_rank; }

    int index() const;

    bool isValid() const { return m_file >= 0 && m_file <= 7 && m_rank >= 0 && m_rank <= 7; }

private:
    int m_file;
    int m_rank;
};

inline bool operator==(const Square &a, const Square &b) { return a.file() == b.file() && a.rank() == b.rank(); }
inline bool operator!=(const Square &a, const Square &b) { return a.file() != b.file() || a.rank() != b.rank(); }

typedef QList<Square> SquareList;

#endif
