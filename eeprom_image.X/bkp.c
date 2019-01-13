#include <xc.h>
#include <p18f4520.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "conbits.h"
#include "uart_layer.h"

uint8_t data = 0;
bool got_data_bool = false;
uint8_t print_buffer[256] = {0};

uint8_t Eeprom_read(uint8_t addr){
    EEADR = addr;
    EECON1bits.EEPGD = 0;
    EECON1bits.CFGS = 0;
    EECON1bits.RD = 1;
    while(EECON1bits.RD);
    return EEDATA;
}

void Eeprom_write(uint8_t addr,uint8_t data){
    EEADR = addr;
    EEDATA = data;
    EECON1bits.EEPGD = 0;
    EECON1bits.CFGS = 0;
    EECON1bits.WREN = 1;
    INTCONbits.GIEH = 0; 
    
    EECON2 = 0x55;
    EECON2 = 0xAA;
    
    EECON1bits.WR = 1;
    while(EECON1bits.WR);
    EECON1bits.WREN = 0;
    INTCONbits.GIEH = 1;
    
     
}

void interrupt high_isr(void);
void interrupt low_priority low_isr(void);

void main(void) {
    bool start_read_bool = false;
    uint16_t imge_size_counter = 0;
    ADCON1bits.PCFG = 0x0E;
    OSCCONbits.IRCF = 0x07;
    OSCCONbits.SCS = 0x03;
    while(OSCCONbits.IOFS!=1);
    
    TRISB=0;
    LATB=0;
    uart_init(51,0,1,0);//baud 9600

    RCONbits.IPEN = 1;
    INTCONbits.GIEH = 1; 
    INTCONbits.GIEL = 1;
    
    sprintf(print_buffer,"program start\n\r");
    uart_send_string(print_buffer);
    
    while(1){
        
        if(got_data_bool){
            Eeprom_write(imge_size_counter,data);
            uart_send('\r');
            imge_size_counter++;
            
            if(imge_size_counter >= 256){
                imge_size_counter=0;
            }
            if(data == 'x'){
                start_read_bool = true;
            }
            got_data_bool = false;
        }
        
        if(start_read_bool){
            sprintf(print_buffer,"print loop\n\r");
            uart_send_string(print_buffer);
            uint8_t res_counter = 0;
            for(uint16_t i = 0 ; i < 256 ; i++){
                
               uint8_t read_data = Eeprom_read(i);
               
              // sprintf(print_buffer,"%d ",read_data);
               //uart_send_string(print_buffer);
               if(read_data == 0){
                   uart_send(' ');
               }
               if(read_data == 255){
                   uart_send(178);
               }
               res_counter++;
               if(res_counter==16){
                   uart_send('\n');
                   uart_send('\r');
                   res_counter=0;
               }
            }
            start_read_bool=false;
        }
    } 
}

void interrupt high_isr(void){
    INTCONbits.GIEH = 0;
    if(PIR1bits.RCIF){
        uart_receiver(&data,&got_data_bool);
       PIR1bits.RCIF=0;
    }
    
    INTCONbits.GIEH = 1;
}

void interrupt low_priority low_isr(void){
    INTCONbits.GIEH = 0;
    
    INTCONbits.GIEH = 1;
}



