#include "mbed.h"
#include "rtos.h"

#define DIGITAL_SIGNAL 0x01
#define ANALOG_SIGNAL 0x02
#define DEFAULT_DIGITAL 1


DigitalIn digitalIn1(p15);
AnalogIn analogIn1(p19);
DigitalOut led1(LED1);
DigitalOut led2(LED2);
Serial pc(USBTX, USBRX);

Mutex mutex;

int data_digital=1;
int toCompare = 0; //1 if we need to check
int data[2] = {1, 1};
typedef struct 
{
	bool isNum;
	int timestamp;	
} mail_t;

Mail<mail_t, 16> mail_box;

// ** Traitement signal digital ** //

void set_digital_signal(void const *args)
	{
	Thread* t = (Thread *) args;
	t->signal_set(DIGITAL_SIGNAL);
}
	
void digital_sampling(void const *args) {
	int i = toCompare%2;
	data[i] = digitalIn1.read();
	set_digital_signal(args);
	
}

void digital_read(void const *args)
	{
	
	while (true){
		time_t stamp;
		
    Thread::signal_wait(DIGITAL_SIGNAL);
		if(toCompare && (data[0] == data[1])){
			printf("Un event");
		}
		
		stamp = time(NULL);
		mutex.lock();
		mail_t *mail = mail_box.alloc();
		mutex.unlock();
		
		mail->isNum = true; 
		mail->timestamp = stamp;
		
		mutex.lock();
		mail_box.put(mail);
		mutex.unlock();
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
		osEvent evt = mail_box.get();
		
		// écriture de l'événement en sortie (port série)
		if (evt.status == osEventMail) 
		{
				mail_t *mail = (mail_t*)evt.value.p;
				pc.printf("\nSource: %s  \n\r"   , mail->isNum ? "Numerique" : "Analogique");
			  //pc.printf("\nTime: %s" \n\r, ctime(mail->timestamp));
				mutex.lock();
				mail_box.free(mail);
				mutex.unlock();
		}
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
