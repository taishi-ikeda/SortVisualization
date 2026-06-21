#include <QApplication>
#include "sortingvisualization.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    
    SortingVisualizer w;
    w.show();
    
    return a.exec();
}