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
        elevonRight{"Elevon Right"},
        throttleRaw{"Throttle-Raw"},
        pitchRaw{"Pitch-Raw"},
        rollRaw{"Roll-Raw"},
        throttleMixed{"Throttle Mixed"},
        elevonLeftMixed{"Elevon Left Mixed"},
        elevonRightMixed{"Elevon Right Mixed"},
        isArmed{"Armed"},
        manualOverrideActive{"Manual Override Active"} {

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

        mainLayout.addWidget(&seperator0);

        mainLayout.addWidget(&motor);
        mainLayout.addWidget(&elevonLeft);
        mainLayout.addWidget(&elevonRight);

        mainLayout.addWidget(&seperator1);

        mainLayout.addWidget(&throttleRaw);
        mainLayout.addWidget(&pitchRaw);
        mainLayout.addWidget(&rollRaw);
        mainLayout.addWidget(&throttleMixed);
        mainLayout.addWidget(&elevonLeftMixed);
        mainLayout.addWidget(&elevonRightMixed);
        mainLayout.addWidget(&isArmed);
        mainLayout.addWidget(&manualOverrideActive);

        seperator0.setFrameShape(QFrame::HLine);
        seperator1.setFrameShape(QFrame::HLine);
    }

    void FcWidget::fcDataReceived(const messages::types::FlightControllerData &package) {
        roll.set(package.imu.roll);
        pitch.set(package.imu.pitch);
        yaw.set(package.imu.yaw);
        dRoll.set(package.imu.dRoll);
        dPitch.set(package.imu.dPitch);
        dYaw.set(package.imu.dYaw);
        accX.set(package.imu.accX);
        accY.set(package.imu.accY);
        accZ.set(package.imu.accZ);
        motor.set(package.motor);
        elevonLeft.set(package.elevonLeft);
        elevonRight.set(package.elevonRight);
        throttleRaw.set(package.remote.throttleRaw);
        pitchRaw.set(package.remote.pitchRaw);
        rollRaw.set(package.remote.rollRaw);
        throttleMixed.set(package.remote.throttleMixed);
        elevonLeftMixed.set(package.remote.elevonLeftMixed);
        elevonRightMixed.set(package.remote.elevonRightMixed);
        isArmed.set(package.remote.isArmed);
        manualOverrideActive.set(package.remote.manualOverrideActive);
    }
} // namespace ground_station
