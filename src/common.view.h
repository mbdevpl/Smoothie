#pragma once

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

#include <cmath>
#include <string>
#include <iomanip>
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

#include <gl/GLU.h>
#include <FTGL/ftgl.h>
//#include <glm/glm.hpp>

#ifdef WINDOWS
#include <win32config.h>
#include <libxml/xmlwin32version.h>
#endif

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
