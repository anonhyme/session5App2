#include "mbed.h"
#include "rtos.h"

#define DIGITAL_SIGNAL 0x01
#define ANALOG_SIGNAL 0x02

DigitalIn digitalIn1(p15);
AnalogIn analogIn1(p19);

Serial pc(USBTX, USBRX);

void generateSignalNum(Thread *t)
{
	t->signal_set(DIGITAL_SIGNAL);
}

void generateSignalAnal(Thread *t)
{
	t->signal_set(ANALOG_SIGNAL);
}

void lecture_analog(void const *args)
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

void lecture_num(void const *args)
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
		
		// lecture des échantillons numériques
		if (state != digitalIn1.read())
		{}
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
  printf("\rSeconds since January 1, 1970: %d           \n\r", seconds);
	printf("Time as a basic string = %s                 \n\r", ctime(&seconds));
	
	// Thread init.
	Thread readNum(lecture_num);
	Thread readAnal(lecture_analog);
	RtosTimer readNumTimer(lecture_num, osTimerPeriodic, (Thread *)&readNum);
	//RtosTimer readAnalTimer(lecture_analog, osTimerPeriodic, (Thread *)&readAnal);
	readNumTimer.start(200);
	
	while(1)
	{
	}
}
