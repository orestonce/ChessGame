#ifndef POINT_H
#define POINT_H

namespace chess
{
/**
 * @brief The CPoint class 点类型
 */
class CPoint
{
public:
    /**
     * @brief CPoint 构造函数, 构造一个line, row的点
     * @param line 行数
     * @param row 列数
     */
    CPoint(int line, int row);

    /**
     * @brief GetLine 获取行数
     * @return 行数
     */
    int GetLine() const;
    /**
     * @brief GetRow 获取列数
     * @return 列数
     */
    int GetRow() const;

    /**
     * @brief SetLine 设置行
     * @param line 新的行数
     */
    void SetLine(int line);
    /**
     * @brief SetRow 设置列
     * @param row 新的列数
     */
    void SetRow(int row);

    /**
     * @brief operator == 判断两个点是否相等
     * @param p 另一个点
     * @return true -> 相等, false -> 不等
     */
    bool operator== (const CPoint& p) const;
private:
    int m_iLine; // 行
    int m_iRow; // 列
};

struct SIncrease // 增量
{
    int m_iLineIncrease; // 行上的增量
    int m_iRowIncrease; // 列上的增量
};

inline SIncrease operator* (SIncrease const& xInc, int iTimes)
{
    SIncrease inc;

    inc.m_iLineIncrease = xInc.m_iLineIncrease* iTimes;
    inc.m_iRowIncrease = xInc.m_iRowIncrease*iTimes;

    return inc;
}

inline CPoint operator + (CPoint const& xPoint, SIncrease const& xInc)
{
    return CPoint(xPoint.GetLine () + xInc.m_iLineIncrease , xPoint.GetRow () + xInc.m_iRowIncrease);
}

inline SIncrease operator - (CPoint const& p1, CPoint const& p2)
{
    SIncrease inc;

    inc.m_iLineIncrease = p1.GetLine () - p2.GetLine ();
    inc.m_iRowIncrease = p1.GetRow () - p2.GetRow ();

    return inc;
}

}

#endif // POINT_H
