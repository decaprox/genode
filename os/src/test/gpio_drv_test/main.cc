#include <base/printf.h>

#include "gpiotest.h"

int main(int, char **)
{

	printf("--- Pandaboard button (GPIO_121) test ---\n");

	GpioTest gpio_test;

	while(1)
	{
		gpio_test.polling_test();
		gpio_test.irq_test();
	}

	return 0;
}