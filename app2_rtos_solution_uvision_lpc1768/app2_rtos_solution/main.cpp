#include "mbed.h"
#include "rtos.h"

#define DIGITAL_SIGNAL 0x01
#define ANALOG_SIGNAL 0x02

DigitalIn digitalIn1(p15);
AnalogIn analogIn1(p19);
DigitalOut led1(LED1);
DigitalOut led2(LED2);
Serial pc(USBTX, USBRX);

Mutex mutex;

int data_digital=1;


typedef struct {
	int type;
	time_t time_stamp;
	int data;
	int event;
}Event;

Mail<Event, 16> mail_box;

// ** Traitement signal digital ** //

void set_digital_signal(void const *args)
	{
	Thread* t = (Thread *) args;
	t->signal_set(DIGITAL_SIGNAL);
}
	
void digital_sampling(void const *args) {
	data_digital = digitalIn1.read();
	set_digital_signal(args);
}

void digital_read(void const *args)
	{
	// Sync at 100ms (stabilisation 50ms)
	int state[2] = {0,0};
	int i = 0;
	time_t stamp;
	while (true){
		// Signal flags that are reported as event are automatically cleared.
    Thread::signal_wait(DIGITAL_SIGNAL);
		i = i%2;
		
		if(i==0){
			// store
		} else {
			// check data with last et si pareille enregistre 
		}
		
		state[i] = digitalIn1.read();
		i++;
		// lecture de l'étampe temporelle 
		stamp = time(NULL);
		printf("First : %d    Second: %d                                               \r", state[0], state[1]);
		// lecture des échantillons numériques
		//if (state != digitalIn1.read())
		//{}
		// prise en charge du phénomène de rebond
		// génération éventuelle d'un événement
	}
}

// ** Traitement signal analog ** //
void analog_sampling(void const *args) {
	//analog.time_stamp = time(NULL);
	//data_digital = analogIn1.read();
}

void analog_signal(void const *args){
	Thread* t = (Thread *) args;
	t->signal_set(ANALOG_SIGNAL);
}

void analog_read(void const *args){
	while (true){
		// synchronisation sur la période d'échantillonnage
		// lecture de l'étampe temporelle
		// lecture des échantillons analogiques
		// calcul de la nouvelle moyenne courante
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

void init() {
	// initialisation du RTC
	set_time(1422222222);
	// speed up serial
	pc.baud(19200);
	led1 = 0;
}

int main()
{
	init();
	time_t seconds = time(NULL);
	//printf("Time as a basic string = %s                 \n\r", ctime(&seconds));
	
	// Thread init.
	Thread digital_thread(digital_read);
	//Thread analog_thread(analog_read);
	
	RtosTimer readNumTimer(digital_sampling, osTimerPeriodic, &digital_thread);
	//RtosTimer readAnalTimer(analog_sampling, osTimerPeriodic);
	readNumTimer.start(1000);
	Thread::wait(osWaitForever);
}
