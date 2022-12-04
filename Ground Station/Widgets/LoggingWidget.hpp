/**
 * @file LoggingWidget.hpp
 * @author paul
 * @date 24.11.22
 * Description here TODO
 */
#ifndef GROUNDSTATION_LOGGINGWIDGET_HPP
#define GROUNDSTATION_LOGGINGWIDGET_HPP

#include <QTextBrowser>
#include <sstream>

namespace ground_station {

    class LoggingWidget : public QTextBrowser {
        Q_OBJECT
      public:
        explicit LoggingWidget(QWidget *parent = nullptr);

        void logInfo(const std::string &msg);
        void logWarn(const std::string &msg);
        void logError(const std::string &msg);

      private:
        std::stringstream logStream;
    };

} // namespace ground_station

#endif // GROUNDSTATION_LOGGINGWIDGET_HPP
