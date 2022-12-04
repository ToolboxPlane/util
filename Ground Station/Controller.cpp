/**
 * @file Controller.cpp
 * @author paul
 * @date 03.12.22
 * Description here TODO
 */
#include "Controller.hpp"

namespace ground_station {
    void Controller::connectToFc() {
        fcPortHandler.emplace("/dev/ttyFC", [this](auto &&data) { emit fcDataReceived(std::forward<decltype(data)>(data)); });
    }

} // namespace ground_station
