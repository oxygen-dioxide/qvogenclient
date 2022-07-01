#include "TChangeVoiceEvent.h"

TChangeVoiceEvent::TChangeVoiceEvent(int type) : TPianoRollEvent(ChangeVoice), vt(type) {
}


TChangeVoiceEvent::~TChangeVoiceEvent() {
}
