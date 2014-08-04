#pragma once

#define FramesPerSecond 25
#define DrawingDelay ((int)(1000.0f / FramesPerSecond))

/*!
 * \file common.view.h
 *
 * This header is meant to be pre-compiled to speed up the build process.
 */

#define Pi       3.141592653589793
#define PiTimes2 6.283185307179586
#define ArcJump 0.06283185307179586

#define FracPiOver180 0.01745329252
#define Frac180OverPi 57.295779513

#ifdef WINDOWS
// This disables warnings after using std::copy, which was declared unsafe in MSVC compiler,
// but std::copy is used in this project only by boost ublas matrix,
// so I pretty much redard it as safe.
#pragma warning(push)
#pragma warning(disable:4996)
#endif

#include <algorithm>
#ifdef WINDOWS
#include <xutility>
#endif

#ifdef WINDOWS
#pragma warning(pop)
#endif

#include <cmath>
#include <string>
#include <iomanip>
#include <fstream>
#include <ostream>
#include <sstream>
#include <vector>
#include <limits>
#include <unordered_map>
#include <utility>
#include <stdexcept>

#ifdef DEBUG
#include <QtDebug>
#endif

#include <QTimer>
#include <QThread>
#include <QDialog>
#include <QFrame>
#include <QLabel>
#include <QFont>
#include <QFileDialog>
#include <QMainWindow>
#include <QtOpenGL>
#include <QtGui>
#include <QGLWidget>

#include <qwt_mathml_text_engine.h>

#include <GL/glu.h>
#include <FTGL/ftgl.h>
//#include <glm/glm.hpp>

#ifdef WINDOWS
#include <win32config.h>
#include <libxml/xmlwin32version.h>
#endif

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xmlversion.h>
#include <libxml/xmlmemory.h>
#include <libxml/encoding.h>
#include <libxml/globals.h>

#define TO_XML BAD_CAST
#define FROM_XML (const char*)

#ifndef Q_MOC_RUN

#ifdef RELEASE
#ifndef NDEBUG
#define NDEBUG
#endif

#ifndef BOOST_UBLAS_NDEBUG
#define BOOST_UBLAS_NDEBUG
#endif
#endif

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>

#endif
