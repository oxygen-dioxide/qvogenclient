#ifndef GLOBAL_H
#define GLOBAL_H

#include <QtGlobal>

#ifdef RENDER_HOST_BUILD_STATIC
    #define RENDER_HOST_API
#else
    #ifdef RENDER_HOST_LIBRARY
        #define RENDER_HOST_API Q_DECL_EXPORT
    #else
        #define RENDER_HOST_API Q_DECL_IMPORT
    #endif
#endif

#define RENDER_HOST_BEGIN_NAMESPACE namespace RH {
#define RENDER_HOST_END_NAMESPACE }
#define RENDER_HOST_USE_NAMESPACE using namespace RH;

#endif // GLOBAL_H
