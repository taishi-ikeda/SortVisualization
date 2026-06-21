#include "sortingvisualization.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QRandomGenerator>
#include <QEventLoop>
#include <QTimer>

SortingVisualizer::SortingVisualizer(QWidget *parent)
    : QWidget(parent), stepCount_(0), isSorting_(false) {
    
    setWindowTitle("Sort Visualizer");
    resize(800, 500);

    auto *mainLayout = new QVBoxLayout(this);
    auto *controlLayout = new QHBoxLayout();

    controlLayout->addWidget(new QLabel("Number of data (n):"));
    countSpinBox_ = new QSpinBox();
    countSpinBox_->setRange(10, 300);
    countSpinBox_->setValue(50);
    controlLayout->addWidget(countSpinBox_);

    algoComboBox_ = new QComboBox();
    for(size_t i = 0; i < algoList_.size(); ++i) {
        algoComboBox_->addItem(algoList_[i]);
    }
    controlLayout->addWidget(algoComboBox_);

    generateButton_ = new QPushButton("Generate Data");
    controlLayout->addWidget(generateButton_);

    startButton_ = new QPushButton("Start");
    controlLayout->addWidget(startButton_);

    stepLabel_ = new QLabel("Step Count: 0");
    controlLayout->addWidget(stepLabel_);
    controlLayout->addStretch();

    mainLayout->addLayout(controlLayout);
    mainLayout->addStretch(1);

    connect(generateButton_, &QPushButton::clicked, this, &SortingVisualizer::generateData);
    connect(startButton_, &QPushButton::clicked, this, &SortingVisualizer::startSorting);

    generateData();
}

void SortingVisualizer::generateData() {
    if (isSorting_) return;

    int n = countSpinBox_->value();
    data_.clear();
    data_.reserve(n);

    for (int i = 0; i < n; ++i) {
        data_.append(QRandomGenerator::global()->generateDouble());
    }

    stepCount_ = 0;
    stepLabel_->setText("Step Count: 0");
    update();
}

void SortingVisualizer::startSorting() {
    if (isSorting_ || data_.isEmpty()) return;

    isSorting_ = true;
    generateButton_->setEnabled(false);
    startButton_->setEnabled(false);
    countSpinBox_->setEnabled(false);
    algoComboBox_->setEnabled(false);

    QString algo = algoComboBox_->currentText();

    sortTimer_ = new QTimer(this);
    if(algo == algoList_[0]) {
        sortConnection_ = connect(sortTimer_, &QTimer::timeout, this, &SortingVisualizer::bubbleSortStep);
    } else if (algo == algoList_[1]) {
        sortConnection_ = connect(sortTimer_, &QTimer::timeout, this, &SortingVisualizer::selectionSortStep);
    } else if (algo == algoList_[2]) {
        sortConnection_ = connect(sortTimer_, &QTimer::timeout, this, &SortingVisualizer::insertionSortStep);
    } else {
        sortConnection_ = connect(sortTimer_, &QTimer::timeout, this, &SortingVisualizer::shellSortStep);
    }
    sortTimer_->start(10);

    isSorting_ = false;
    generateButton_->setEnabled(true);
    startButton_->setEnabled(true);
    countSpinBox_->setEnabled(true);
    algoComboBox_->setEnabled(true);
}

void SortingVisualizer::incrementStep() {
    stepCount_++;
    stepLabel_->setText(QString("Step Count: %1").arg(stepCount_));
}

void SortingVisualizer::waitAndRefresh(int ms) {
    update();
    
    //emscripten_sleep(ms);  // Use emscripten_sleep for non-blocking sleep in Emscripten
    
    QEventLoop loop;
    QTimer::singleShot(ms, &loop, &QEventLoop::quit);
    loop.exec();
}

void SortingVisualizer::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    if (data_.isEmpty()) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int topOffset = 60;
    int canvasWidth = width();
    int canvasHeight = height() - topOffset;

    double barWidth = static_cast<double>(canvasWidth) / data_.size();

    for (int i = 0; i < data_.size(); ++i) {
        int barHeight = static_cast<int>(data_[i] * canvasHeight);
        
        int x = static_cast<int>(i * barWidth);
        int y = height() - barHeight;

        painter.setBrush(QColor(100, 149, 237)); 
        painter.setPen(Qt::NoPen);

        painter.drawRect(x, y, qMax(1, static_cast<int>(barWidth) - 1), barHeight);
    }
}

void SortingVisualizer::bubbleSort() {
    int n = data_.size();
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            if (data_[j] > data_[j + 1]) {
                qSwap(data_[j], data_[j + 1]);
            }
            incrementStep();
            waitAndRefresh(500 / n); 
        }
    }
}

void SortingVisualizer::bubbleSortStep() {
    static int i = 0, j = 0;
    int n = data_.size();
    if (i < n - 1) {
        if (j < n - i - 1) {
            if (data_[j] > data_[j + 1]) {
                qSwap(data_[j], data_[j + 1]);
            }
            incrementStep();
            j++;
        } else {
            j = 0;
            i++;
        }
    } else {
        i = 0;
        j = 0;
        disconnect(sortConnection_);
    }
    update();
}

void SortingVisualizer::selectionSort() {
    int n = data_.size();
    for (int i = 0; i < n - 1; ++i) {
        int minIdx = i;
        for (int j = i + 1; j < n; ++j) {
            if (data_[j] < data_[minIdx]) {
                minIdx = j;
            }
            incrementStep();
            waitAndRefresh(500 / n);
        }
        if (minIdx != i) {
            qSwap(data_[i], data_[minIdx]);
        }
    }
    update();
}

void SortingVisualizer::selectionSortStep() {
    static int i = 0, j = 0, minIdx = 0;
    int n = data_.size();
    if (i < n - 1) {
        if (j < n) {
            if (data_[j] < data_[minIdx]) {
                minIdx = j;
            }
            incrementStep();
            j++;
        } else {
            if (minIdx != i) {
                qSwap(data_[i], data_[minIdx]);
            }
            i++;
            j = i + 1;
            minIdx = i;
        }
    } else {
        i = 0;
        j = 0;
        minIdx = 0;
        disconnect(sortConnection_);
    }
    update();
}

void SortingVisualizer::insertionSort() {
    int n = data_.size();
    for (int i = 1; i < n; ++i) {
        double key = data_[i];
        int j = i - 1;

        while (j >= 0 && data_[j] > key) {
            data_[j + 1] = data_[j];
            j--;
            incrementStep();
            waitAndRefresh(500 / n);
        }
        data_[j + 1] = key;
    }
    update();
}

void SortingVisualizer::insertionSortStep() {
    static int i = 1, j = 0;
    int n = data_.size();
    if (i < n) {
        double key = data_[i];
        j = i - 1;

        while (j >= 0 && data_[j] > key) {
            data_[j + 1] = data_[j];
            j--;
            incrementStep();
        }
        data_[j + 1] = key;
        i++;
    } else {
        i = 1;
        j = 0;
        disconnect(sortConnection_);
    }
    update();
}

void SortingVisualizer::shellSort() {
    int n = data_.size();
    for (int gap = n / 2; gap > 0; gap /= 2) {
        for (int i = gap; i < n; ++i) {
            double temp = data_[i];
            int j;
            for (j = i; j >= gap && data_[j - gap] > temp; j -= gap) {
                data_[j] = data_[j - gap];
                incrementStep();
                waitAndRefresh(500 / n);
            }
            data_[j] = temp;
        }
    }
    update();
}

void SortingVisualizer::shellSortStep() {
    static int gap = data_.size() / 2;
    static int i = gap;
    static int j = 0;

    if (gap > 0) {
        if (i < data_.size()) {
            double temp = data_[i];
            j = i;
            while (j >= gap && data_[j - gap] > temp) {
                data_[j] = data_[j - gap];
                j -= gap;
                incrementStep();
            }
            data_[j] = temp;
            i++;
        } else {
            gap /= 2;
            i = gap;
        }
    } else {
        gap = data_.size() / 2;
        i = gap;
        j = 0;
        disconnect(sortConnection_);
    }
    update();
}

