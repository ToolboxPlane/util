/**
 * @file ConnectionWidget.hpp
 * @author paul
 * @date 24.11.22
 * Description here TODO
 */
#ifndef GROUNDSTATION_CONNECTIONWIDGET_HPP
#define GROUNDSTATION_CONNECTIONWIDGET_HPP

#include <QHBoxLayout>
#include <QPushButton>

namespace ground_station {
    class ConnectionWidget : public QHBoxLayout {
      public:
        explicit ConnectionWidget(QWidget *parent = nullptr);

      private slots:

        void fcButtonClicked();
        void pdbButtonClicked();
      private:
        QPushButton fcButton{"Connect to FC"};
        QPushButton pdbButton{"Connect to PDB"};
    };

} // namespace ground_station

#endif // GROUNDSTATION_CONNECTIONWIDGET_HPP
