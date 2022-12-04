/**
 * @file DataField.hpp
 * @author paul
 * @date 04.12.22
 * Description here TODO
 */
#ifndef GROUNDSTATION_DATAFIELD_HPP
#define GROUNDSTATION_DATAFIELD_HPP

#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>
#include <sstream>

namespace ground_station {

    class DataField : public QWidget {
        Q_OBJECT

      public:
        explicit DataField(const std::string &title);

        template<typename T>
        void set(T t) {
            std::stringstream stringstream;
            stringstream << t;
            data.setText(stringstream.str().c_str());
        }

      private:
        QHBoxLayout layout;
        QLabel name, data;
    };

} // namespace ground_station

#endif // GROUNDSTATION_DATAFIELD_HPP
