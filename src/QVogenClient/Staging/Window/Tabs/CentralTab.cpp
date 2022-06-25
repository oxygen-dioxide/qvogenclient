#include "CentralTab.h"
#include "MainWindow.h"
#include "Managers/DataManager.h"

#include "CentralTab_p.h"

Q_UNIQUE_ID_DECLARE(CentralTab)

CentralTab::CentralTab(QWidget *parent) : CentralTab(*new CentralTabPrivate(), parent) {
}

CentralTab::~CentralTab() {
    destructId();
}

CentralTab::Type CentralTab::type() const {
    return NoTab;
}

QString CentralTab::filename() const {
    Q_D(const CentralTab);
    return d->filename;
}

void CentralTab::setFilename(const QString &filename) {
    Q_D(CentralTab);
    d->filename = filename;
    filenameSet(filename);
}

bool CentralTab::isEdited() const {
    Q_D(const CentralTab);
    return d->edited;
}

bool CentralTab::canUndo() const {
    Q_D(const CentralTab);
    return !d->earliest();
}

bool CentralTab::canRedo() const {
    Q_D(const CentralTab);
    return !d->latest();
}

int CentralTab::selectionFlags() const {
    Q_D(const CentralTab);
    return d->selectionFlags;
}

void CentralTab::setTabName(const QString &tabName) {
    Q_D(CentralTab);
    BaseTab::setTabName(d->setTabNameProxy(tabName));
}

void CentralTab::updateTabName() {
    Q_D(CentralTab);
    setTabName(d->tabText());
}

void CentralTab::setEdited(bool edited) {
    Q_D(CentralTab);
    d->edited = edited;
}

void CentralTab::filenameSet(const QString &filename) {
    Q_UNUSED(filename)
}

bool CentralTab::load() {
    return false;
}

bool CentralTab::save() {
    return false;
}

bool CentralTab::saveAs(const QString &filename) {
    Q_UNUSED(filename)
    return false;
}

bool CentralTab::restore() {
    return false;
}

void CentralTab::undo() {
}

void CentralTab::redo() {
}

void CentralTab::selectAll() {
}

void CentralTab::deselect() {
}

void CentralTab::awake() {
}

void CentralTab::sleep() {
}

void CentralTab::enter() {
}

void CentralTab::leave() {
}

void CentralTab::change() {
}

bool CentralTab::acceptClose() {
    return true;
}

void CentralTab::initAfterLayout() {
}

void CentralTab::handleSpecificAction(ActionImpl::Action a) {
    Q_UNUSED(a);
}

QString CentralTab::TypeToString(CentralTab::Type type) {
    QString res;
    switch (type) {
    case Welcome: {
        res = "welcome";
        break;
    }
    case Tuning: {
        res = "tuning";
        break;
    }
    case Voice: {
        res = "voice";
        break;
    }
    case Keyboard: {
        res = "keyboard";
        break;
    }
    case Setting: {
        res = "setting";
        break;
    }
    default: {
        res = "unknown";
        break;
    }
    }
    return res;
}

CentralTab::CentralTab(CentralTabPrivate &d, QWidget *parent) : BaseTab(d, parent) {
    initId();

    d.init();
}
