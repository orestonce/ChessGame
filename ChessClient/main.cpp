#include "login.h"
#include "gamepanel.h"
#include <QApplication>

int main(int argc, char *argv[]){
    if(argc >= 2 && QString(argv[1]) == "test-startup")
    {
        //用于测试编译出来的二进制是否可以运行
        std::cerr << "The test of startup.\n";
        return 0;
    }
    QApplication a(argc, argv);

    Login w;
    w.show();

    return a.exec();
}


//#include <QDebug>
//#include <functional>

//class A
//{
//public :
//    A()
//    {
//        qDebug() << "A " << this;
//    }
//    A(const A&a)
//    {
//        qDebug() << "A(&a)" << this << &a;
//        this->value = a.value;
//    }

//    ~A()
//    {
//        qDebug() << "~A"<< this;
//    }
//public :
//    int value;
//};

//std::function<void ()> retFunction()
//{
//    A a;
//    a.value = 14;
//    qDebug() << "1";
//    return [=]() {
//        qDebug() << "2";
//        qDebug() << a.value;
//    };
//}

//int main() {
//    {
//        auto f1 = retFunction();
//        qDebug() << "3";
//        f1();
//        qDebug() << "5";
//        f1();
//        qDebug() << "6";
//    }
//    qDebug() << "4";
//    qDebug() << "xxxx";
//}
