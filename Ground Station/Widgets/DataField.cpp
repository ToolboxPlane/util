/**
 * @file DataField.cpp
 * @author paul
 * @date 04.12.22
 * Description here TODO
 */
#include "DataField.hpp"

namespace ground_station {
    DataField::DataField(const std::string &title) {
        this->setLayout(&layout);
        this->layout.addWidget(&name);
        this->layout.addWidget(&data);
        this->name.setText(title.c_str());
    }
} // namespace ground_station
