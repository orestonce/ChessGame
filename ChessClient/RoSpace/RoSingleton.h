#ifndef ROSINGLETON_H
#define ROSINGLETON_H

namespace RoSpace
{
// 单例模式的一种实现
// 注意: 这里的单例模式并不是指不能new 只能Instance()获得,
//           而是一种库程序员与库用户的一种约定, 普通用户只通过 Instance() 函数获得
//           全局唯一的实例化对象
template <typename T>
class CSingleton
{
public:
    static T* Instance()
    {
        static T x;

        return &x;
    }
};
}
#endif // ROSINGLETON_H
