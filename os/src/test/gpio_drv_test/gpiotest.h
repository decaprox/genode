#ifndef _GPIOTEST_H_
#define _GPIOTEST_H_

#include <gpio_session/connection.h>
#include <base/signal.h>
#include <util/mmio.h>
#include <base/printf.h>

using namespace Genode;

class GpioTest
{

public:
	enum {
		LED1_GPIO        = 7,
		LED2_GPIO        = 8,
		BUTTON_GPIO     = 121,
		GPIO4_IRQ       = 32 + 32,
	};

private:

	Gpio::Connection _gpio;

	Signal_receiver sig_rec;
	Signal_context  sig_ctx;

public:
	GpioTest();
	~GpioTest();

	void wait_for_signal()
	{
		sig_rec.wait_for_signal();
	}

	bool polling_test();
	bool irq_test();
};

GpioTest::GpioTest()
{
	// initialize GPIO_121
	_gpio.set_debouncing_time(BUTTON_GPIO, 31*100);
	_gpio.set_debounce_enable(BUTTON_GPIO, 1);

	_gpio.register_signal(sig_rec.manage(&sig_ctx), BUTTON_GPIO);
}

GpioTest::~GpioTest()
{
	_gpio.unregister_signal(BUTTON_GPIO);
}

bool GpioTest::polling_test()
{
	printf("---------- Polling test ----------\n");

	printf("\nPush and hold button...\n");
	_gpio.set_dataout(LED1_GPIO, true);
	_gpio.set_dataout(LED2_GPIO, false);

	volatile int gpio_state;

	do {
		gpio_state = _gpio.get_datain(BUTTON_GPIO);
	} while (gpio_state);

	printf("OK\n");

	_gpio.set_dataout(LED1_GPIO, false);
	_gpio.set_dataout(LED2_GPIO, true);

	printf("\nRelease button...\n");
	do {
		gpio_state = _gpio.get_datain(BUTTON_GPIO);
	} while (!gpio_state);

	printf("OK\n");

	return true;
}

bool GpioTest::irq_test()
{
	printf("---------- IRQ test ----------\n");

	_gpio.set_falling_detect(BUTTON_GPIO, 1);
	_gpio.set_irq_enable(BUTTON_GPIO, 1);

	_gpio.set_dataout(LED1_GPIO, true);
	_gpio.set_dataout(LED2_GPIO, false);

	printf("\nPush and hold button...\n");

	wait_for_signal();

	_gpio.set_irq_enable(BUTTON_GPIO, 0);
	printf("OK\n");

	_gpio.set_falling_detect(BUTTON_GPIO, 0);
	_gpio.set_rising_detect(BUTTON_GPIO, 1);
	_gpio.set_irq_enable(BUTTON_GPIO, 1);

	_gpio.set_dataout(LED1_GPIO, false);
	_gpio.set_dataout(LED2_GPIO, true);

	printf("\nRelease button...\n");

	wait_for_signal();

	_gpio.set_irq_enable(BUTTON_GPIO, 0);
	printf("OK\n");

	_gpio.set_falling_detect(BUTTON_GPIO, 0);
	_gpio.set_rising_detect(BUTTON_GPIO, 0);
	return true;
}

#endif // _GPIOTEST_H_