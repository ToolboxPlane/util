/**
 * @file Controller.hpp
 * @author paul
 * @date 03.12.22
 * Description here TODO
 */
#ifndef GROUNDSTATION_CONTROLLER_HPP
#define GROUNDSTATION_CONTROLLER_HPP

#include <SerialPortHandler.hpp>

namespace ground_station {
    class Controller : public QObject {
        Q_OBJECT

      signals:
        void fcDataReceived(const messages::types::FlightControllerPackage &package);

      public slots:
        void connectToFc();

      private:
        std::optional<SerialPortHandler<messages::types::FlightControllerPackage>> fcPortHandler;
    };

} // namespace ground_station

#endif // GROUNDSTATION_CONTROLLER_HPP
