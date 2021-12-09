#ifndef VIDEOSLIDER_H
#define VIDEOSLIDER_H

#include <QSlider>
class VideoSlider : public QSlider {
    Q_OBJECT

public:
     VideoSlider(Qt::Orientation orientation, QWidget *parent = nullptr) : QSlider(orientation, parent) {
      connect(this, SIGNAL(valueChanged(int)), this, SLOT(emitPositionChanged(int)));
      this->setTickInterval(1);
     }

public slots:
    void setMaxRange(qint64);

    void emitPositionChanged(int);

    void setPosition(qint64);

signals:
    void positionChanged(qint64 pos);

};

#endif // VIDEOSLIDER_H
