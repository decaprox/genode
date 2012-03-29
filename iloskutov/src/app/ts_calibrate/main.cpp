/**
 * \brief  Application for calibration resestive touch screen
 * \author Ivan Loskutov
 * \date   2012-03-29
 */

/*
 * Copyright 2012 Ksys Labs LLC
 * Contact: <ivan.loskutov@ksyslabs.org>
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#include <QtGui/QApplication>
#include "tscalibrate.h"



int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
    TSCalibrate w;
    w.show();

    return a.exec();
}
