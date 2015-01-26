#include "mbed.h"
#include "rtos.h"

#define DIGITAL_SIGNAL 0x01
#define ANALOG_SIGNAL 0x02

DigitalIn digitalIn1(p15);
AnalogIn analogIn1(p19);

Serial pc(USBTX, USBRX);

void digital_signal(void const *args)
{
	Thread* t = (Thread *) args;
	t->signal_set(DIGITAL_SIGNAL);
}

void analog_signal(void const *args)
{
	Thread* t = (Thread *) args;
	t->signal_set(ANALOG_SIGNAL);
}

void analog_read(void const *args)
{
	while (true)
	{
		// synchronisation sur la période d'échantillonnage
		// lecture de l'étampe temporelle
		// lecture des échantillons analogiques
		// calcul de la nouvelle moyenne courante
		// génération éventuelle d'un événement
	}
}

typedef struct {
	
} digital_struct;

void digital_read(void const *args)
{
	int state = digitalIn1.read();
	time_t stamp;
	while (true)
	{
		// synchronisation sur la période d'échantillonnage (lecture aux 100ms, stabilisation 50ms)
		// Signal flags that are reported as event are automatically cleared.
    Thread::signal_wait(DIGITAL_SIGNAL);
		
		// lecture de l'étampe temporelle 
		stamp = time(NULL);
		printf("HELLO ! Digital Read");
		// lecture des échantillons numériques
		//if (state != digitalIn1.read())
		//{}
		// prise en charge du phénomène de rebond
		// génération éventuelle d'un événement
	}
}

void collection(void const *args)
{
	while (true)
	{
		// attente et lecture d'un événement
		// écriture de l'événement en sortie (port série)
		//pc.printf("Time as a basic string = %s", ctime(&seconds));
	}
}

int main()
{
	// initialisation du RTC
	set_time(1422222222);
	
	time_t seconds = time(NULL);
  //printf("\rSeconds since January 1, 1970: %d           \n\r", seconds);
	//printf("Time as a basic string = %s                 \n\r", ctime(&seconds));
	
	// Thread init.
	Thread digital_thread(digital_read);
	Thread analog_thread(analog_read);
	
	RtosTimer readNumTimer(digital_signal, osTimerPeriodic, &digital_thread);
	RtosTimer readAnalTimer(analog_signal, osTimerPeriodic, &analog_thread);
	
	readNumTimer.start(200);
	
	while(1)
	{
	}
}
