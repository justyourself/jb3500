


#if (ARCH_TYPE == ARCH_T_STM32F10X) || \
	(ARCH_TYPE == ARCH_T_LM3S5X) || (ARCH_TYPE == ARCH_T_LM3S9X) || \
	(ARCH_TYPE == ARCH_T_LPC176X)

//#include <arch/CMSIS/CM3/CoreSupport/core_cm3.c>
#include <arch/cm3/fault.c>
#include <arch/cm3/stack.c>

#elif ARCH_TYPE == ARCH_T_M051X
#include <arch/cm0/fault.c>
#include <arch/cm0/stack.c>

#endif





#if ARCH_TYPE == ARCH_T_STM32F10X
//#include <arch/CMSIS/CM3/DeviceSupport/ST/STM32F10x/system_stm32f10x.c>
#ifdef RT_USING_CONSOLE
#include <arch/cm3/stm32f10x/serial.c>
#endif
#include <arch/cm3/stm32f10x/arch.c>
#include <arch/cm3/stm32f10x/arch_adc.c>
#include <arch/cm3/stm32f10x/arch_bkp.c>
#include <arch/cm3/stm32f10x/arch_flash.c>
#include <arch/cm3/stm32f10x/arch_fsmc.c>
#include <arch/cm3/stm32f10x/arch_gpio.c>
#include <arch/cm3/stm32f10x/arch_i2c.c>
#include <arch/cm3/stm32f10x/arch_it.c>
#include <arch/cm3/stm32f10x/arch_iwdg.c>
#include <arch/cm3/stm32f10x/arch_nand.c>
#include <arch/cm3/stm32f10x/arch_rtc.c>
#include <arch/cm3/stm32f10x/arch_serial.c>
#include <arch/cm3/stm32f10x/arch_spi.c>
#include <arch/cm3/stm32f10x/arch_timer.c>
#include <arch/cm3/stm32f10x/arch_uart.c>
#include <arch/cm3/stm32f10x/arch_retarget.c>

#elif ARCH_TYPE == ARCH_T_LPC176X
#ifdef RT_USING_CONSOLE
#include <arch/cm3/lpc176x/serial.c>
#endif
#include <arch/cm3/lpc176x/system_LPC17xx.c>
#include <arch/cm3/lpc176x/arch.c>
#include <arch/cm3/lpc176x/arch_emac.c>
#include <arch/cm3/lpc176x/arch_flash.c>
#include <arch/cm3/lpc176x/arch_gpio.c>
#include <arch/cm3/lpc176x/arch_iwdg.c>
#include <arch/cm3/lpc176x/arch_it.c>
#include <arch/cm3/lpc176x/arch_pwm.c>
#include <arch/cm3/lpc176x/arch_serial.c>
#include <arch/cm3/lpc176x/arch_timer.c>
#include <arch/cm3/lpc176x/arch_uart.c>



#elif (ARCH_TYPE == ARCH_T_LM3S9X) || (ARCH_TYPE == ARCH_T_LM3S5X)
#ifdef RT_USING_CONSOLE
#include <arch/cm3/lm3s/serial.c>
#endif
#if 0
#include <arch/cm3/lm3s/driverlib/adc.c>
#include <arch/cm3/lm3s/driverlib/cpu.c>
#include <arch/cm3/lm3s/driverlib/epi.c>
#include <arch/cm3/lm3s/driverlib/ethernet.c>
#include <arch/cm3/lm3s/driverlib/flash.c>
#include <arch/cm3/lm3s/driverlib/gpio.c>
#include <arch/cm3/lm3s/driverlib/interrupt.c>
#include <arch/cm3/lm3s/driverlib/sysctl.c>
#include <arch/cm3/lm3s/driverlib/systick.c>
#include <arch/cm3/lm3s/driverlib/timer.c>
#include <arch/cm3/lm3s/driverlib/uart.c>
#include <arch/cm3/lm3s/driverlib/usb.c>
#include <arch/cm3/lm3s/driverlib/watchdog.c>
#endif
#include <arch/cm3/lm3s/arch.c>
#include <arch/cm3/lm3s/arch_emac.c>
#include <arch/cm3/lm3s/arch_flash.c>
#include <arch/cm3/lm3s/arch_gpio.c>
#include <arch/cm3/lm3s/arch_iwdg.c>
#include <arch/cm3/lm3s/arch_it.c>
#include <arch/cm3/lm3s/arch_pwm.c>
#include <arch/cm3/lm3s/arch_serial.c>
#include <arch/cm3/lm3s/arch_timer.c>
#include <arch/cm3/lm3s/arch_uart.c>



#elif ARCH_TYPE == ARCH_T_M051X
#include <arch/cm0/m051/arch.c>
#include <arch/cm0/m051/arch_gpio.c>
#include <arch/cm0/m051/arch_it.c>
#include <arch/cm0/m051/arch_serial.c>
#include <arch/cm0/m051/arch_timer.c>
#include <arch/cm0/m051/arch_uart.c>







#elif ARCH_TYPE == ARCH_T_STR71X
#include <arch/arm7/str71x/arch.c>
#include <arch/arm7/str71x/arch_emi.c>
#include <arch/arm7/str71x/arch_gpio.c>
#include <arch/arm7/str71x/arch_it.c>
#include <arch/arm7/str71x/arch_rtc.c>
#include <arch/arm7/str71x/arch_serial.c>
#include <arch/arm7/str71x/arch_uart.c>
#include <arch/arm7/str71x/arch_retarget.c>

#endif


/* exception and interrupt handler table */
uint32_t rt_interrupt_from_thread, rt_interrupt_to_thread;
uint32_t rt_thread_switch_interrupt_flag;

#if OS_TYPE
void rt_hw_timer_handler(void)
{
	/* enter interrupt */
	rt_interrupt_enter();

	rt_tick_increase();

	/* leave interrupt */
	rt_interrupt_leave();
}

/* write one character to serial, must not trigger interrupt */
static void rt_hw_console_putc(const char c)
{
	extern int sendchar(int ch);
	/*
		to be polite with serial console add a line feed
		to the carriage return character
	*/
	if (c == '\n') sendchar('\r');

	sendchar(c);
}
#endif


