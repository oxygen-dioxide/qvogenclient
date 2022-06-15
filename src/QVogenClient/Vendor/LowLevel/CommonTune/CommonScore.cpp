#include "CommonScore.h"
#include "CommonTuneStd.h"

CommonScore::CommonScore() : tempo(CommonTuneStd::DEFAULT_VALUE_TEMPO), beat(QPoint(4, 4)) {
}

CommonScore::~CommonScore() {
}
