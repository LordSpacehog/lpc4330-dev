/*
 * This file is part of OpenDAC.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include <libopencm3/lpc43xx/gpio.h>
#include <libopencm3/lpc43xx/scu.h>
#include <libopencm3/lpc43xx/usb.h>
#include "libopencm3/lpc43xx/m4/nvic.h"

#include "opendac_core.h"

uint32_t boot0, boot1, boot2, boot3;

usb_queue_head_t endpoints[2];

void reset_usb_phy_0(){
    // reset usb phy
    USB0_USBCMD_D |= USB0_USBCMD_D_RST;
    // wait for reset to finish
    while ( USB0_USBCMD_D & USB0_USBCMD_D_RST );
    return;
}

void usb0_isr(){
    const uint32_t status = USB0_USBSTS_D & USB0_USBINTR_D;

    if(status==0) {
        return;
    }

    if(status & USB0_USBSTS_D_UI) {
        // do shit (deal with setup packets)
        
        const uint32_t setup_status = USB0_ENDPTSETUPSTAT;

        if (setup_status & USB0_ENDPTSETUPSTAT_ENDPTSETUPSTAT(0)) {
            //endpoints[0]->k;

            //endpoints[1];
        }
        //setup
        

        //config
        
    }

    if(status & USB0_USBSTS_D_UEI) {

    }

    if (status & USB0_USBSTS_D_PCI){
        
    }

    if (status & USB0_USBSTS_D_URI){
        // disable all endpoints
        USB0_ENDPTCTRL1 &= ~(USB0_ENDPTCTRL1_RXE | USB0_ENDPTCTRL1_TXE);
        USB0_ENDPTCTRL2 &= ~(USB0_ENDPTCTRL2_RXE | USB0_ENDPTCTRL2_TXE);
        USB0_ENDPTCTRL3 &= ~(USB0_ENDPTCTRL3_RXE | USB0_ENDPTCTRL3_TXE);
        USB0_ENDPTCTRL4 &= ~(USB0_ENDPTCTRL4_RXE | USB0_ENDPTCTRL4_TXE);
        USB0_ENDPTCTRL5 &= ~(USB0_ENDPTCTRL5_RXE | USB0_ENDPTCTRL5_TXE);

        //clear pending endpoint interrupts
        USB0_ENDPTNAK       = 0xFFFFFFFF;
        USB0_ENDPTNAKEN     = 0xFFFFFFFF;
        USB0_USBSTS_D       = 0xFFFFFFFF;
        USB0_ENDPTSETUPSTAT = USB0_ENDPTSETUPSTAT & 0xFFFFFFFF;
        USB0_ENDPTCOMPLETE  = USB0_ENDPTCOMPLETE & 0xFFFFFFFF;

        //flush primed enpoints
        while(USB0_ENDPTPRIME & 0xFFFFFFFF);
        USB0_ENDPTFLUSH = 0xFFFFFFFF;
        while(USB0_ENDPTFLUSH & 0xFFFFFFFF);

        //
        USB0_DEVICEADDR = USB0_DEVICEADDR_USBADR(0);
    }
    
    if (status & USB0_USBSTS_D_SLI){
        
    }
    
    if (status & USB0_USBSTS_D_NAKI){
        
    }

    USB0_USBSTS_D = status;
}

void usb_init(){
    
    // Initialize queue head for receive
    /*endpoints[0] = {
        .capabilities        = 0,
        .current_dtd_pointer = 0,
        .next_dtd_pointer    = 0,
        .total_bytes         = 0,
        .buffer_pointer_page = { 0, 0, 0, 0, 0 },
        ._reserved_0         = 0,
        .setup               = { 0, 0, 0, 0, 0, 0, 0, 0 },
        ._reserved_1         = { 0, 0, 0, 0 }
    };
    
    // Initialize queue head for transmit
    endpoints[1] = {
        .capabilities        = 0,
        .current_dtd_pointer = 0,
        .next_dtd_pointer    = 0,
        .total_bytes         = 0,
        .buffer_pointer_page = { 0, 0, 0, 0, 0 },
        ._reserved_0         = 0,
        .setup               = { 0, 0, 0, 0, 0, 0, 0, 0 },
        ._reserved_1         = { 0, 0, 0, 0 }
    };*/


    //Set USB0 to device mode
    USB0_USBMODE_D = (2); 

    // Set the endpoint list address
    USB0_ENDPOINTLISTADDR = endpoints;

    // Enable interrupts
    USB0_USBINTR_D =
          USB0_USBSTS_D_UI
        | USB0_USBSTS_D_UEI
        | USB0_USBSTS_D_PCI
        | USB0_USBSTS_D_URI
        | USB0_USBSTS_D_SLI;

    nvic_enable_irq(NVIC_USB0_IRQ);
    // Set the RUN/STOP bit to Run
    // USB0_USBCMD_D_RS
    USB0_USBCMD_D |= USB0_USBCMD_D_RS;
}

int main(void)
{
    int i;
    pin_setup();

    /* Set up USB device */
    cpu_clock_init();
    
    reset_usb_phy_0();
    usb_init();

    //cpu_clock_pll1_max_speed();
    
    /* Blink LED1/2/3 on the board and Read BOOT0/1/2/3 pins. */
    while (1) 
    {
        boot0 = BOOT0_STATE;
        boot1 = BOOT1_STATE;
        boot2 = BOOT2_STATE;
        boot3 = BOOT3_STATE;

        gpio_set(PORT_LED1_2, (PIN_LED1|PIN_LED2)); /* LEDs on */
        for (i = 0; i < 2000000; i++)   /* Wait a bit. */
            __asm__("nop");
        gpio_clear(PORT_LED1_2, (PIN_LED1|PIN_LED2)); /* LED off */
        for (i = 0; i < 2000000; i++)   /* Wait a bit. */
            __asm__("nop");
    }

    return 0;
}
