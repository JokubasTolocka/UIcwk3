#ifndef PLAYBACKSPEEDSELECTOR_H
#define PLAYBACKSPEEDSELECTOR_H

#include <QComboBox>
class PlaybackSpeedSelector : public QComboBox {
    Q_OBJECT

private:
    QStringList options = {
        "0.5",
        "1.0",
        "1.5",
        "2.0"
    };

public:
     PlaybackSpeedSelector(QWidget *parent = nullptr) : QComboBox(parent) {

         this->insertItems(0, options);
         this->setCurrentIndex(1);

         connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(emitSpeedChanged(int)));
     }

private slots:
    void emitSpeedChanged(int);

signals:
    void speedChanged(qreal);
};

#endif // PLAYBACKSPEEDSELECTOR_H
