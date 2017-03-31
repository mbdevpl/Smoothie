#!/usr/bin/env bash

cd ..
svn export svn://svn.code.sf.net/p/qwt/code/branches/qwt-6.1
cd qwt-6.1

echo "* Patching the QWT build config"

# enable QwtMathML
grep "#QWT_CONFIG     += QwtMathML" qwtconfig.pri
sed -i'' "s|#QWT_CONFIG     += QwtMathML|QWT_CONFIG     += QwtMathML|" qwtconfig.pri
# change install dir
grep "QWT_INSTALL_PREFIX    = /usr/local/qwt-\$\$QWT_VERSION-svn" qwtconfig.pri
sed -i'' "s|QWT_INSTALL_PREFIX    = /usr/local/qwt-\$\$QWT_VERSION-svn|QWT_INSTALL_PREFIX    = /usr/local|" qwtconfig.pri

echo "* building and installing QWT"

source /opt/qt56/bin/qt56-env.sh
#source /opt/qt58/bin/qt58-env.sh

qmake --version
qmake qwt.pro
make
sudo make install
