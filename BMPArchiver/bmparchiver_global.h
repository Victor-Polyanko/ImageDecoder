#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(BMPARCHIVER_LIB)
#  define BMPARCHIVER_EXPORT Q_DECL_EXPORT
# else
#  define BMPARCHIVER_EXPORT Q_DECL_IMPORT
# endif
#else
# define BMPARCHIVER_EXPORT
#endif
