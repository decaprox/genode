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

#ifndef TSCALIBRATE_H
#define TSCALIBRATE_H

#include <QMainWindow>

class QTimer;

#include <base/env.h>
#include <base/printf.h>
#include <ts_input_session/client.h>
#include <ts_input_session/connection.h>

#include <input/event.h>

typedef struct {
    int x[5], xfb[5];
    int y[5], yfb[5];
    int a[7];
} calibration;

namespace Ui {
class TSCalibrate;
}

class TSCalibrate : public QMainWindow
{
    Q_OBJECT

public:
    explicit TSCalibrate(QWidget *parent = 0);
    ~TSCalibrate();

protected:
    void paintEvent(QPaintEvent *event);

private:
    void updateWindow(int x=0, int y=0);

    void drawCursor(int x, int y);
    void clearCursor();

    int perform_calibration();

private slots:
    void updateConfirmTimer();
	void processInput();

    void on_butConfirm_clicked();

private:
    enum State {
        ST_START,
        ST_TAP1,
        ST_TAP2,
        ST_TAP3,
        ST_TAP4,
        ST_TAP5,
        ST_CONFIRM,
        ST_END,
        ST_EXIT
    };

private:
    Ui::TSCalibrate *ui;

	QTimer  *_timer;
	QTimer  *_input_timer;
    int     _confirm_sec;

    int     _cursor_x;
    int     _cursor_y;
    State   _state;

    calibration _cal;

	Ts_input::Connection ts_input_srv;
	Input::Event         *_ev_buf;
};

#endif // TSCALIBRATE_H
