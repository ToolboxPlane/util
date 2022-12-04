/**
 * @file LoggingWidget.cpp
 * @author paul
 * @date 24.11.22
 * Description here TODO
 */
#include "LoggingWidget.hpp"

namespace ground_station {
    LoggingWidget::LoggingWidget(QWidget *parent) : QTextBrowser{parent} {
    }
    void LoggingWidget::logInfo(const std::string &msg) {
        logStream << "<p style=\"color: blue\">" << msg << "</p>";
        this->setHtml(logStream.str().c_str());
    }
    void LoggingWidget::logWarn(const std::string &msg) {
        logStream << "<p style=\"color: orange\">" << msg << "</p>";
        this->setHtml(logStream.str().c_str());
    }
    void LoggingWidget::logError(const std::string &msg) {
        logStream << "<p style=\"color: red\">" << msg << "</p>";
        this->setHtml(logStream.str().c_str());
    }
} // namespace ground_station
