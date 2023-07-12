#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(UPDATE_LIB)
#  define UPDATE_EXPORT Q_DECL_EXPORT
# else
#  define UPDATE_EXPORT Q_DECL_IMPORT
# endif
#else
# define UPDATE_EXPORT
#endif
