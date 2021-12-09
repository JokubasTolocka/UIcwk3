#include "playbackspeedselector.h"

void PlaybackSpeedSelector::emitSpeedChanged(int index) {
    qreal speed;
    if (index == 0)
        speed = 0.5;
    else if (index == 1)
        speed = 1.0;
    else if (index == 2)
        speed = 1.5;
    else if (index == 3)
        speed = 2.0;

    emit(speedChanged(speed));
}
