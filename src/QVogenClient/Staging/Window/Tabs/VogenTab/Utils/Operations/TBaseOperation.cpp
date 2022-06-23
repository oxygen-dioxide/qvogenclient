#include "TBaseOperation.h"

TBaseOperation::TBaseOperation(int type) : t(type) {
}

TBaseOperation::~TBaseOperation() {
}

int TBaseOperation::type() const {
    return t;
}

bool TBaseOperation::differ() const {
    return false;
}
