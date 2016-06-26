#include "Point.h"

namespace chess
{
CPoint::CPoint(int line, int row)
    : m_iLine(line), m_iRow(row)
{

}

int CPoint::GetLine() const
{
    return m_iLine;
}

int CPoint::GetRow() const
{
    return m_iRow;
}

void CPoint::SetLine(int line)
{
    m_iLine = line;
}

void CPoint::SetRow(int row)
{
    m_iRow = row;
}

bool CPoint::operator==(const CPoint &p) const
{
    return m_iLine == p.m_iLine && m_iRow == p.m_iRow;
}

}
