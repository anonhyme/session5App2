#include "mbed.h"
#include "rtos.h"


DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalIn sw1(p15);
DigitalIn sw2(p16);

// Si le digital input sw1 est a 1 on eteint la led 1
void led1_thread(void const *args) {
    while (true) {
        if(sw1) {
            led1 = 0;
        } else {
            led1 = 1;
        }
    }
}
// Si le digital input sw2 est a 1 on eteint la led 2
void led2_thread(void const *args) {
    while (true) {
        if(sw2) {
            led2 = 0;
        } else {
            led2 = 1;
        }
    }
}
int main() {
	// on regarde en continue pour l'état des inputs sw1 et sw2 
    Thread thread1(led1_thread);
    Thread thread2(led2_thread);
    while (true) {
    }
}
