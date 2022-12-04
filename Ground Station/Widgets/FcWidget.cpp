/**
 * @file FcWidget.cpp
 * @author paul
 * @date 24.11.22
 * Description here TODO
 */
#include "FcWidget.hpp"

template<typename T>
auto siToQString(T t) -> QString {
    std::stringstream stringstream;
    stringstream << t;
    return stringstream.str().c_str();
}

namespace ground_station {
    FcWidget::FcWidget(QWidget *parent) :
        QWidget{parent},
        roll{"Roll:"},
        pitch{"Pitch:"},
        yaw{"Yaw:"},
        dRoll{"dRoll:"},
        dPitch{"dPitch:"},
        dYaw{"dYaw:"},
        accX{"Acc-X"},
        accY{"Acc-Y"},
        accZ{"Acc-Z"},
        motor{"Motor"},
        elevonLeft{"Elevon Left"},
        elevonRight{"Elevon Right"} {

        this->setLayout(&mainLayout);

        mainLayout.addWidget(&yaw);
        mainLayout.addWidget(&pitch);
        mainLayout.addWidget(&roll);
        mainLayout.addWidget(&dYaw);
        mainLayout.addWidget(&dPitch);
        mainLayout.addWidget(&dRoll);
        mainLayout.addWidget(&dYaw);
        mainLayout.addWidget(&dPitch);
        mainLayout.addWidget(&dRoll);
        mainLayout.addWidget(&accX);
        mainLayout.addWidget(&accY);
        mainLayout.addWidget(&accZ);

        mainLayout.addWidget(&seperator);

        mainLayout.addWidget(&motor);
        mainLayout.addWidget(&elevonLeft);
        mainLayout.addWidget(&elevonRight);

        seperator.setFrameShape(QFrame::HLine);
    }

    void FcWidget::fcDataReceived(const messages::types::FlightControllerPackage &package) {
        roll.set(package.roll);
        pitch.set(package.pitch);
        yaw.set(package.yaw);
        dRoll.set(package.dRoll);
        dPitch.set(package.dPitch);
        dYaw.set(package.dYaw);
        accX.set(package.accX);
        accY.set(package.accY);
        accZ.set(package.accZ);
        motor.set(package.motor);
        elevonLeft.set(package.elevonLeft);
        elevonRight.set(package.elevonRight);
    }
} // namespace ground_station
