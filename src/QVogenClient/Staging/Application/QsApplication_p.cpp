#include "QsApplication_p.h"

#include <QDebug>
#include <QDir>
#include <QFontDatabase>
#include <QMessageBox>
#include <QScreen>
#include <QTextCodec>

#include "Namespace.h"
#include "SystemHelper.h"

#include "Types/Events.h"

QsApplicationPrivate::QsApplicationPrivate() {
}

QsApplicationPrivate::~QsApplicationPrivate() {
}

void QsApplicationPrivate::init() {
    Q_Q(QsApplication);

    q->setApplicationName(Qs::AppName);
    q->setApplicationVersion(Qs::Version);

    QEventImpl::Register();

    initLocale();
    initFonts();

    // Create
    record = new CRecordHolder(q);

    data = new DataManager(q);
    themes = new ExtensionManager(q);
    windows = new WindowManager(q);

    initModules();

    q->connect(q->primaryScreen(), &QScreen::logicalDotsPerInchChanged, q,
               &QsApplication::q_screenRatioChanged);
}

void QsApplicationPrivate::deinit() {
    quitModules();

    // Destroy
    delete windows;
    delete themes;
    delete data;

    delete record;
}

void QsApplicationPrivate::initLocale() {
    QTextCodec *gbk = QTextCodec::codecForName("GBK");

#ifdef Q_OS_WINDOWS
    QTextCodec::setCodecForLocale(gbk);
#endif
}

void QsApplicationPrivate::initFonts() {
    Q_Q(QsApplication);
#if defined(Q_OS_WINDOWS)
    QFont f("Microsoft YaHei");
    f.setStyleStrategy(QFont::PreferAntialias);
    q->setFont(f);
#elif defined(Q_OS_LINUX)
    // ?
#endif
}

void QsApplicationPrivate::initModules() {
    if (!qData->load()) {
        ::exit(1);
    }
    if (!qTheme->load()) {
    }
}

void QsApplicationPrivate::quitModules() {
    if (!qTheme->save()) {
    }
    if (!qData->save()) {
    }
}
