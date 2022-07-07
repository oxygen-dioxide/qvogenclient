#include "QsApplication.h"
#include "WindowManager.h"

#ifndef Q_OS_MAC
#include <FramelessManager>
#endif

int main(int argc, char *argv[]) {
#ifndef Q_OS_MAC
    // Not necessary, but better call this function, before the construction
    // of any Q(Core|Gui)Application instances.
    FRAMELESSHELPER_NAMESPACE::FramelessHelper::Core::initialize();
#endif

    // Create Application
    QsApplication a(argc, argv);

    // Create Window
    WindowManager::instance()->newWindow();

    return a.exec();
}
