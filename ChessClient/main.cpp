#include "login.h"
#include "setserver.h"
#include "register.h"
#include "playgame.h"
#include <QApplication>
int main(int argc, char *argv[]){
    QApplication a(argc, argv);
    Login w;
    w.show();

    return a.exec();
}
