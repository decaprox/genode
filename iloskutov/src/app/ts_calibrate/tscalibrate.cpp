/**
 * \brief  Application for calibration resistive touch screen
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

/*
 * Used calibration algoritm from tslib
 */

#include "tscalibrate.h"
#include "ui_tscalibrate.h"

#include <QPainter>
#include <QMouseEvent>
#include <QTimer>


TSCalibrate::TSCalibrate(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::TSCalibrate),
	_timer(new QTimer),
	_input_timer(new QTimer),
	_cursor_x(-1), _cursor_y(-1),
	_state(ST_START),
	_ev_buf(Genode::env()->rm_session()->attach(ts_input_srv.dataspace()))
{
	ui->setupUi(this);

	connect(_timer, SIGNAL(timeout()), this, SLOT(updateConfirmTimer()));
	_timer->setInterval(1000);

	connect(_input_timer, SIGNAL(timeout()), this, SLOT(processInput()));
	_input_timer->setInterval(10);
	_input_timer->start();

	this->showFullScreen();
	this->setFocus();

	qDebug("window pos_x=%d pos_y=%d", pos().x(), pos().y());

	updateWindow();
}

TSCalibrate::~TSCalibrate()
{
	delete ui;
}

void TSCalibrate::processInput()
{
	if (ts_input_srv.is_pending())
	{
		int num_ev = ts_input_srv.flush();

		for (unsigned src_ev_cnt = 0; src_ev_cnt < num_ev; src_ev_cnt++) {

			Input::Event *e = &_ev_buf[src_ev_cnt];


			if ((_state != ST_END) && (e->type() == Input::Event::PRESS))
			{
				//qDebug("tscalibrate type=%d keycode=%d abs_x=%d abs_y=%d\n", e->type(), e->keycode(),  e->ax(),  e->ay());
				updateWindow(e->ax(),  e->ay());
			}
		}
	}
}

void TSCalibrate::paintEvent(QPaintEvent *event)
{
	if ( (_cursor_x > 0) && (_cursor_y > 0) )
	{
		QPainter painter(this);

		painter.drawLine(_cursor_x-10, _cursor_y, _cursor_x+10, _cursor_y);
		painter.drawLine(_cursor_x, _cursor_y-10, _cursor_x, _cursor_y+10);
	}
	QMainWindow::paintEvent(event);
}

void TSCalibrate::updateWindow(int x, int y)
{
	switch(_state)
	{
	case ST_START:
		// set raw mode ts_input
		_cal.a[0] = 1;
		_cal.a[1] = 0;
		_cal.a[2] = 0;
		_cal.a[3] = 0;
		_cal.a[4] = 1;
		_cal.a[5] = 0;
		_cal.a[6] = 1;
		ts_input_srv.set_calibration(_cal.a[0], _cal.a[1], _cal.a[2], _cal.a[3],
									_cal.a[4], _cal.a[5], _cal.a[6]);

		//
		ui->butConfirm->setVisible(false);
		ui->labText->setText("Tap the screen to start calibration");
		_state = ST_TAP1;
		break;
	case ST_TAP1:
		ui->labText->setText("Tap the crosshair");
		drawCursor( 50, 50 );
		_state = ST_TAP2;
		break;
	case ST_TAP2:
		_cal.x[0] = x; _cal.y[0] = y; _cal.xfb[0] = 50; _cal.yfb[0] = 50;
		qDebug("ST_TAP1 x=%d y=%d xfb=%d yfb=%d", _cal.x[0], _cal.y[0], _cal.xfb[0], _cal.yfb[0]);
		drawCursor( width() - 50, 50 );
		_state = ST_TAP3;
		break;
	case ST_TAP3:
		_cal.x[1] = x; _cal.y[1] = y; _cal.xfb[1] = width() - 50; _cal.yfb[1] = 50;
		qDebug("ST_TAP2 x=%d y=%d xfb=%d yfb=%d", _cal.x[1], _cal.y[1], _cal.xfb[1], _cal.yfb[1]);
		drawCursor( width() - 50, height() - 50 );
		_state = ST_TAP4;
		break;
	case ST_TAP4:
		_cal.x[2] = x; _cal.y[2] = y; _cal.xfb[2] = width() - 50; _cal.yfb[2] = height() - 50;
		qDebug("ST_TAP3 x=%d y=%d xfb=%d yfb=%d", _cal.x[2], _cal.y[2], _cal.xfb[2], _cal.yfb[2]);
		drawCursor( 50, height() - 50 );
		_state = ST_TAP5;
		break;
	case ST_TAP5:
		_cal.x[3] = x; _cal.y[3] = y; _cal.xfb[3] = 50; _cal.yfb[3] = height() - 50;
		qDebug("ST_TAP4 x=%d y=%d xfb=%d yfb=%d", _cal.x[3], _cal.y[3], _cal.xfb[3], _cal.yfb[3]);
		drawCursor( width()/2, height()/2 );
		_state = ST_CONFIRM;
		break;
	case ST_CONFIRM:
		_cal.x[4] = x; _cal.y[4] = y; _cal.xfb[4] = width()/2; _cal.yfb[4] = height()/2;
		qDebug("ST_TAP5 x=%d y=%d xfb=%d yfb=%d", _cal.x[4], _cal.y[4], _cal.xfb[4], _cal.yfb[4]);
		clearCursor();
		// calibrate
		if (!perform_calibration())
		{
			ui->labText->setText("Calibration error<br/> Tap to screen to repeat");
			_state = ST_START;
			break;
		}
		// set current calibration to ts_input
		qDebug("Calibration constants: ");
		qDebug("%d %d %d %d %d %d %d",
			 _cal.a[1], _cal.a[2], _cal.a[0],
			 _cal.a[4], _cal.a[5], _cal.a[3], _cal.a[6]);
		ts_input_srv.set_calibration(_cal.a[1], _cal.a[2], _cal.a[0],
									 _cal.a[4], _cal.a[5], _cal.a[3], _cal.a[6]);
		// check
		ui->labText->setText("Press button to confirm calibration.<br/> If you will don't push button then calibration will start again");
		ui->butConfirm->setText("[10] OK");
		ui->butConfirm->setVisible(true);

		_timer->start();
		_confirm_sec = 10;
		_state = ST_END;
		break;
	case ST_END:
		break;
	case ST_EXIT:
		qDebug("Exit !!!");
		close();
	}
}

void TSCalibrate::drawCursor(int x, int y)
{
	_cursor_x = x; _cursor_y = y;
	update();
}

void TSCalibrate::clearCursor()
{
	_cursor_x = -1; _cursor_y = -1;
	update();
}

void TSCalibrate::updateConfirmTimer()
{
	_confirm_sec--;
	ui->butConfirm->setText(QString("[%1] OK").arg(_confirm_sec));
	if (_confirm_sec == 0)
	{
		_timer->stop();
		_state = ST_START;
		updateWindow();
	}
}

void TSCalibrate::on_butConfirm_clicked()
{
	_timer->stop();
	ui->butConfirm->setVisible(false);
	ui->labText->setText("Calibration complete. Tap the screen to exit");
	_state = ST_EXIT;
	// set calibration
}

int TSCalibrate::perform_calibration() {
	int j;
	float n, x, y, x2, y2, xy, z, zx, zy;
	float det, a, b, c, e, f, i;
	float scaling = 65536.0;

	// Get sums for matrix
	n = x = y = x2 = y2 = xy = 0;
	for(j=0;j<5;j++) {
		n += 1.0;
		x += (float)_cal.x[j];
		y += (float)_cal.y[j];
		x2 += (float)(_cal.x[j]*_cal.x[j]);
		y2 += (float)(_cal.y[j]*_cal.y[j]);
		xy += (float)(_cal.x[j]*_cal.y[j]);
	}

	// Get determinant of matrix -- check if determinant is too small
	det = n*(x2*y2 - xy*xy) + x*(xy*y - x*y2) + y*(x*xy - y*x2);
	if(det < 0.1 && det > -0.1) {
		qDebug("ts_calibrate: determinant is too small -- %f\n",det);
		return 0;
	}

	// Get elements of inverse matrix
	a = (x2*y2 - xy*xy)/det;
	b = (xy*y - x*y2)/det;
	c = (x*xy - y*x2)/det;
	e = (n*y2 - y*y)/det;
	f = (x*y - n*xy)/det;
	i = (n*x2 - x*x)/det;

	// Get sums for x calibration
	z = zx = zy = 0;
	for(j=0;j<5;j++) {
		z += (float)_cal.xfb[j];
		zx += (float)(_cal.xfb[j]*_cal.x[j]);
		zy += (float)(_cal.xfb[j]*_cal.y[j]);
	}

	// Now multiply out to get the calibration for framebuffer x coord
	_cal.a[0] = (int)((a*z + b*zx + c*zy)*(scaling));
	_cal.a[1] = (int)((b*z + e*zx + f*zy)*(scaling));
	_cal.a[2] = (int)((c*z + f*zx + i*zy)*(scaling));

	qDebug("%f %f %f\n",(a*z + b*zx + c*zy),
				(b*z + e*zx + f*zy),
				(c*z + f*zx + i*zy));

	// Get sums for y calibration
	z = zx = zy = 0;
	for(j=0;j<5;j++) {
		z += (float)_cal.yfb[j];
		zx += (float)(_cal.yfb[j]*_cal.x[j]);
		zy += (float)(_cal.yfb[j]*_cal.y[j]);
	}

	// Now multiply out to get the calibration for framebuffer y coord
	_cal.a[3] = (int)((a*z + b*zx + c*zy)*(scaling));
	_cal.a[4] = (int)((b*z + e*zx + f*zy)*(scaling));
	_cal.a[5] = (int)((c*z + f*zx + i*zy)*(scaling));

	qDebug("%f %f %f\n",(a*z + b*zx + c*zy),
				(b*z + e*zx + f*zy),
				(c*z + f*zx + i*zy));

	// If we got here, we're OK, so assign scaling to a[6] and return
	_cal.a[6] = (int)scaling;
	return 1;
}
