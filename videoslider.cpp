#include "videoslider.h"

void VideoSlider::emitPositionChanged(int pos) {
    emit(positionChanged((qint64) pos));
}

void VideoSlider::setMaxRange(qint64 max) {
    this->setRange(0, (int) max);
}

void VideoSlider::setPosition(qint64 pos) {
    this->setValue((int) pos);
}
