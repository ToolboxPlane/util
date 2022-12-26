/**
 * @file FcWidget.hpp
 * @author paul
 * @date 24.11.22
 * Description here TODO
 */
#ifndef GROUNDSTATION_FCWIDGET_HPP
#define GROUNDSTATION_FCWIDGET_HPP

#include <Messages/Types/Types.hpp>
#include <QVBoxLayout>

#include "DataField.hpp"

namespace ground_station {
    class FcWidget : public QWidget {
        Q_OBJECT
      public:
        explicit FcWidget(QWidget *parent = nullptr);

      public slots:
        void fcDataReceived(const messages::types::FlightControllerData &package);

      private:
        QVBoxLayout mainLayout;
        QFrame seperator0, seperator1;
        DataField roll, pitch, yaw, dRoll, dPitch, dYaw;
        DataField accX, accY, accZ;
        DataField motor, elevonLeft, elevonRight;
        DataField throttleRaw, pitchRaw, rollRaw;
        DataField throttleMixed, elevonLeftMixed, elevonRightMixed;
        DataField isArmed, manualOverrideActive;
    };

} // namespace ground_station

#endif // GROUNDSTATION_FCWIDGET_HPP
