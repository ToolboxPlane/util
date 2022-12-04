/**
 * @file ConnectionWidget.cpp
 * @author paul
 * @date 24.11.22
 * Description here TODO
 */
#include "ConnectionWidget.hpp"

namespace ground_station {
    ConnectionWidget::ConnectionWidget(QWidget *parent) : QHBoxLayout{parent} {
        this->addWidget(&fcButton);
        this->addWidget(&pdbButton);

        QObject::connect(&fcButton, &QPushButton::clicked, this, &ConnectionWidget::fcButtonClicked);
        QObject::connect(&pdbButton, &QPushButton::clicked, this, &ConnectionWidget::pdbButtonClicked);
    }

    void ConnectionWidget::fcButtonClicked() {
    }

    void ConnectionWidget::pdbButtonClicked() {
    }
} // namespace ground_station
