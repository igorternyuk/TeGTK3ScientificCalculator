#include <iostream>
#include "mainwindow.h"
#include "parser.h"
#include <vector>
#include <iostream>

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);
    MainWindow window;
    window.show();
    gtk_main();
    return 0;
}
