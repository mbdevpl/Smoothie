#-------------------------------------------------
# Project created by QtCreator 2013-08-15 15:45:09
#-------------------------------------------------

QT += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = smoothie
TEMPLATE = app

CONFIG(debug, debug|release) {
	win32:DESTDIR = $$PWD/../bin/debug
	else:unix:DESTDIR = $$PWD/../bin/debug
} else {
	win32:DESTDIR = $$PWD/../bin/release
	else:unix:DESTDIR = $$PWD/../bin/release
}

# platform specific defines
win32:DEFINES += WINDOWS
else:unix:DEFINES += LINUX

# debug/release defines
CONFIG(debug, debug|release): DEFINES += DEBUG
else:CONFIG(release, debug|release): DEFINES += RELEASE

win32 {
	DEPENDPATH += "D:/Projects/Cpp/Libraries/Includes/Boost"
	INCLUDEPATH += "D:/Projects/Cpp/Libraries/Includes/Boost"

	DEPENDPATH += "D:/Projects/Cpp/Libraries/Includes/OpenGL"
	INCLUDEPATH += "D:/Projects/Cpp/Libraries/Includes/OpenGL"

	DEPENDPATH += "D:/Projects/Cpp/Libraries/Includes/LibXML"
	INCLUDEPATH += "D:/Projects/Cpp/Libraries/Includes/LibXML"

	LIBS += -L$$quote(D:/Projects/Cpp/Libraries) -lfreetype -lftgl -llibxml2
} else {
	LIBS += -lfreetype -lftgl -llibxml2
}

CONFIG += precompile_header

PRECOMPILED_HEADER = common.view.h

SOURCES += \
	main.cpp \
	MainWindow.cpp \
	PointFrame.cpp \
	NewMeshDialog.cpp \
	LoadExampleDialog.cpp \
	MatricesDialog.cpp \
	Point2D.cpp \
	Point3D.cpp \
	Plane.cpp \
	Mesh.cpp \
	MeshView.cpp \
	FluidFlowView.cpp \
	RectangularMesh.cpp \
	TriangularMesh.cpp \
	MeshPoint.cpp \
	MeshElement.cpp \
	FluidFlowCalculator.cpp \
	TriangukarFluidFlowCalculator.cpp \
	RectangularFluidFlowCalculator.cpp \
	MeshExamples.cpp \
	CalculatorThread.cpp \

HEADERS += \
	common.view.h \
	MainWindow.hpp \
	PointFrame.hpp \
	NewMeshDialog.hpp \
	LoadExampleDialog.hpp \
	MatricesDialog.hpp \
	Point2D.hpp \
	Point3D.hpp \
	Plane.hpp \
	Mesh.hpp \
	MeshView.hpp \
	FluidFlowView.hpp \
	RectangularMesh.hpp \
	TriangularMesh.hpp \
	IrregularMesh.hpp \
	MeshPoint.hpp \
	MeshElement.hpp \
	FluidFlowCalculator.hpp \
	TriangukarFluidFlowCalculator.hpp \
	RectangularFluidFlowCalculator.hpp \
	MeshExamples.hpp \
	CalculatorThread.hpp \

FORMS += MainWindow.ui \
	PointFrame.ui \
	NewMeshDialog.ui \
	LoadExampleDialog.ui \
	MatricesDialog.ui \

OTHER_FILES += \
	../.gitignore \
	../README.md \
	../LICENSE.md \
	../doc/Smoothie.doxyfile \
