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
        void fcDataReceived(const messages::types::FlightControllerPackage &package);

      private:
        QVBoxLayout mainLayout;
        DataField roll, pitch, yaw, dRoll, dPitch, dYaw;
        DataField accX, accY, accZ;
        QFrame seperator;
        DataField motor, elevonLeft, elevonRight;
    };

} // namespace ground_station

#endif // GROUNDSTATION_FCWIDGET_HPP
