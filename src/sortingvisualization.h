#ifndef SORTINGVISUALIZER_H
#define SORTINGVISUALIZER_H

#include <QWidget>
#include <QVector>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>

class SortingVisualizer : public QWidget {
    Q_OBJECT

public:
    SortingVisualizer(QWidget *parent = nullptr);
    ~SortingVisualizer() = default;

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void generateData();
    void startSorting();

private:
    std::vector<QString> algoList_ = {"Bubble Sort", 
                                      "Selection Sort",
                                      "Insertion Sort",
                                      "Shell Sort"};

    QSpinBox *countSpinBox_;
    QComboBox *algoComboBox_;
    QPushButton *generateButton_;
    QPushButton *startButton_;
    QLabel *stepLabel_;
    QTimer *sortTimer_;

    QVector<double> data_;
    int stepCount_;
    bool isSorting_;

    void incrementStep();
    void waitAndRefresh(int ms = 50);

    QMetaObject::Connection sortConnection_;

    void bubbleSort();
    void bubbleSortStep();
    void selectionSort();
    void selectionSortStep();
    void insertionSort();
    void insertionSortStep();
    void shellSort();
    void shellSortStep();
};

#endif // SORTINGVISUALIZER_H
