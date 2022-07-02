#include <FramelessManager>

#include "QsApplication.h"
#include "WindowManager.h"

#include "Import/QWaveInfo.h"
#include "wave/file.h"

int main(int argc, char *argv[]) {
    // Not necessary, but better call this function, before the construction
    // of any Q(Core|Gui)Application instances.
    FRAMELESSHELPER_NAMESPACE::FramelessHelper::Core::initialize();

    // Create Application
    QsApplication a(argc, argv);

    // Create Window
    WindowManager::instance()->newWindow();

    return a.exec();
}
