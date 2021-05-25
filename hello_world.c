/*
 * "Hello World" example.
 *
 * This example prints 'Hello from Nios II' to the STDOUT stream. It runs on
 * the Nios II 'standard', 'full_featured', 'fast', and 'low_cost' example
 * designs. It runs with or without the MicroC/OS-II RTOS and requires a STDOUT
 * device in your system's hardware.
 * The memory footprint of this hosted application is ~69 kbytes by default
 * using the standard reference design.
 *
 * For a reduced footprint version of this template, and an explanation of how
 * to reduce the memory footprint for a given application, see the
 * "small_hello_world" template.
 *
 */


#include "sys/alt_stdio.h"
#include <stdio.h>
#include "sys/alt_irq.h"
#include "system.h"

unsigned char	seven_seg_table[] =
			{0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7C, 0x07,
		  	 0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71 };
volatile int edge_capture;
    volatile int * timer_ptr = (int *) TIMER_0_BASE;
	volatile int * ledr_ptr = (int *) LEDR_BASE;
	volatile int * ledg_ptr = (int *) LEDG_BASE;
	volatile int * key_ptr = (int *) KEY_BASE;
	volatile int * hex_ptr        = (int *) HEX_BASE;
	volatile int * sw_ptr        = (int *) SW_BASE;
	int sec_counter_watch = 0;
	int diz_sec = 0;
	int unite_sec = 0;
	int press = 0;
	int unite_min_stwatch = 0;
	int diz_min_stwatch = 0;
	int unite_min_watch = 0;
	int diz_min_watch = 0;
	int unite_h = 0;
	int diz_h = 0;
	int sec_counter_chrono = 0;
	int key1_basc = 0;
	int pause_chrono = 0;
	int g = 0;
	int reg = 0;
	void handle_timer_interrupts(void* context, alt_u32 id)
	{
	    /* Reset the Button's edge capture register. */
		*(timer_ptr) = 0; 				// Clear the interrupt
		sec_counter_watch++;
		sec_counter_chrono++;


	}
	void handle_key_interrupts(void* context, alt_u32 id)
	{
		press = * (key_ptr +3) ;
		if (press&0x2){
			key1_basc = !key1_basc ;

		}
		else if (press&0x4) {
			pause_chrono = !pause_chrono;


		}
		else if (press&0x8){
			reg = !reg;
		}
		* (key_ptr + 3) = 0;

	}
	void init_timer()
	{
	    /* Recast the edge_capture pointer to match the alt_irq_register() function
	     * prototype. */
	    void* edge_capture_ptr = (void*) &edge_capture;
	    /* set the interval timer period for scrolling the HEX displays */
		*(timer_ptr + 1) = 0x7;	// STOP = 0, START = 1, CONT = 1, ITO = 1
	    alt_irq_register( TIMER_0_IRQ, edge_capture_ptr,handle_timer_interrupts );
	}

	void init_key()
	{
		void* edge_capture_ptr = (void*) &edge_capture;
		* (key_ptr + 2) = 0xe;
	    alt_irq_register( KEY_IRQ, edge_capture_ptr,handle_key_interrupts );
	}
	void affi_montre(){

			* hex_ptr = ((seven_seg_table[diz_h]<<24 &0xFF000000)) |((seven_seg_table[unite_h]<<16 &0x00FF0000)) |((seven_seg_table[diz_min_watch]<<8&0x0000FF00)) | (seven_seg_table[unite_min_watch]);
			init_key();

	}
	void stopwatch(){
		sec_counter_chrono = 0;
		while (diz_min_stwatch  <  9){
	    	unite_sec =  sec_counter_chrono % 10;
	     	diz_sec = ( sec_counter_chrono%60 ) / 10;
			unite_min_stwatch = ( sec_counter_chrono / 60 ) % 10;
			diz_min_stwatch = sec_counter_chrono / 600;
		              	* hex_ptr = ((seven_seg_table[diz_min_stwatch]<<24 &0xFF000000)) |((seven_seg_table[unite_min_stwatch]<<16 &0x00FF0000)) |((seven_seg_table[diz_sec]<<8&0x0000FF00)) | (seven_seg_table[unite_sec]);
		              	init_key();
		              	g = sec_counter_chrono;
		              	while (pause_chrono){
		              		init_key();
		              		sec_counter_chrono = g;
		              		if (!key1_basc){
		              			break;
		              		}

						}
		              	if (!key1_basc){
		              		break;
		              	}
		}
	}
	void watch(){
		sec_counter_watch %= 86400;
		unite_min_watch = ( sec_counter_watch / 60 ) % 10;
		diz_min_watch = ( sec_counter_watch / 60) / 10;
		unite_h = ((unite_min_watch + diz_min_watch*10) / 60) % 10;
		diz_h = ((unite_min_watch + diz_min_watch*10) / 60 ) / 10;
		diz_min_watch %= 6;
	}
	int main()
	{
		init_key();
			init_timer();
			while (1){

				watch();
				if (key1_basc){
					stopwatch();
				}

				else affi_montre();

				while (reg){
					pause_chrono = 0;
					init_key();
					if (key1_basc){
						sec_counter_watch = sec_counter_watch + 60;
						watch();
						affi_montre();
						key1_basc = !key1_basc;

					}
					if (pause_chrono){
						sec_counter_watch = sec_counter_watch + 3600;
						watch();
						affi_montre();
						pause_chrono = !pause_chrono;

					}
				}

		}


	  return 0;
	}


