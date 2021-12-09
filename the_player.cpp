//
// Created by twak on 11/11/2019.
//

#include "the_player.h"
#include <cmath>

// all buttons have been setup, store pointers here
void ThePlayer::setContent(std::vector<TheButton*>* b, std::vector<TheButtonInfo>* i) {
    buttons = b;
    infos = i;
    jumpTo(buttons -> at(0) -> info);
}

// change the image and video for one button every one second
//void ThePlayer::shuffle() {
//    TheButtonInfo* i = & infos -> at (rand() % infos->size() );
//        setMedia(*i->url);
//    buttons -> at( updateCount++ % buttons->size() ) -> init( i );
//}

//self made signals for next/previous, if these are removed for some reason the program just doesnt work
//the only issues with next and previous is making a playlist that would fit the way the program
//is already implemented. otherwise the signals and slots in tomeo.cpp will work

void ThePlayer::next() {
    //TheButtonInfo* i = & infos -> at (1);
    //setMedia(*i->url);
}

void ThePlayer::previous() {

}

void ThePlayer::skip5seconds() {
   qint64 newPosition = position() + (qint64) 5000;
   if (newPosition > duration())
       newPosition = duration();

   setPosition(newPosition);
}

void ThePlayer::changePosition(qint64 pos) {
    setPosition(pos);
}

void ThePlayer::playStateChanged (QMediaPlayer::State ms) {
    switch (ms) {
        case QMediaPlayer::State::StoppedState:
            play(); // starting playing again...
            break;
    default:
        break;
    }
}

void ThePlayer::jumpTo (TheButtonInfo* button) {
    videoDuration = -1;
    setMedia( * button -> url);
    play();
}
