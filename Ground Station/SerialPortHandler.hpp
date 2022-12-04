/**
 * @file ConnectionHandler.hpp
 * @author paul
 * @date 27.11.22
 * Description here TODO
 */
#ifndef GROUNDSTATION_ABSTRACTSERIALPORTHANDLER_HPP
#define GROUNDSTATION_ABSTRACTSERIALPORTHANDLER_HPP

#include <Messages/MessageConversion.hpp>
#include <Messages/MessageDecoding.h>
#include <QDebug>
#include <QSerialPort>
#include <functional>

namespace ground_station {
    template<typename T>
    class SerialPortHandler : public QObject {
      public:
        using Callback = std::function<void(T)>;
        explicit SerialPortHandler(const std::string &path, const Callback &callback);

      private:
        using Conversion = messages::Conversion<T>;
        message_decoding_data_t messageDecodingData;

        QSerialPort port;
        Callback callback;
    };

    template<typename T>
    SerialPortHandler<T>::SerialPortHandler(const std::string &path, const Callback &callback) :
        messageDecodingData{.id = Conversion::ID, .decodingState = DECODING_INITIAL, .len = 0, .buf = {}},
        port{path.c_str()},
        callback{callback} {
        port.setBaudRate(QSerialPort::Baud115200);
        // port.setParity(QSerialPort::OddParity);
        // port.setStopBits(QSerialPort::OneStop);

        QObject::connect(&port, &QSerialPort::readyRead, [this]() {
            pb_istream_t istream;
            QByteArray data = port.readAll();
            for (auto byte : data) {
                if (message_decode(&this->messageDecodingData, byte, &istream)) {
                    typename Conversion::ProtobufType protobufType;
                    pb_decode(&istream, Conversion::description, &protobufType);

                    this->callback(Conversion::fromProtobuf(protobufType));
                }
            }
        });
        QObject::connect(&port, &QSerialPort::errorOccurred,
                         [](QSerialPort::SerialPortError error) { qDebug() << error; });

        if (!port.open(QSerialPort::ReadWrite)) {
            qDebug() << "Error opening " << path.c_str();
        }
    }

} // namespace ground_station


#endif // GROUNDSTATION_ABSTRACTSERIALPORTHANDLER_HPP
