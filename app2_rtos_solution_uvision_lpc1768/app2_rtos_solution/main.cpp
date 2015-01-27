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

typedef struct 
{
	bool isNum;
	int timestamp;	
} mail_t;

int data_digital=1;

Mail<mail_t, 16> mail_box;

// ** Traitement signal digital ** //

void digital_signal(void const *args)
	{
	Thread* t = (Thread *) args;
	t->signal_set(DIGITAL_SIGNAL);
}
	
//void digital_sampling(void const *args) {
//	data_digital = digitalIn1.read();
//	set_digital_signal(args);
//}

void digital_read(void const *args)
	{
	// Sync at 100ms (stabilisation 50ms)
//		int state[];
//		int isFirst = 1;
//		int evt_detection = 0;
//		time_t stamp;
//		int i = 0;
//		
//		while (true){
//			
//		Thread::signal_wait(DIGITAL_SIGNAL);
//			i = i%2;
//		if(isFirst) {
//			state[i] = data_digital;
//			i++;
//			
//			
//		// lecture de l'étampe temporelle 
//		stamp = time(NULL);

//		if (state != digitalIn1.read())
//		{ 
//		}
//		// prise en charge du phénomène de rebond
//		// génération éventuelle d'un événement
//				}
//		}
	}


void analog_signal(void const *args){
	Thread* t = (Thread *) args;
	t->signal_set(ANALOG_SIGNAL);
}

void analog_read(void const *args)
{
	const int bufferSize = 5;
	const unsigned short maxVoltage = 0xff;
	
	unsigned short buffer[bufferSize] = {0,0,0,0,0}; 
	time_t stamp;
	
	while (true)
	{
		// synchronisation sur la période d'échantillonnage
		Thread::signal_wait(ANALOG_SIGNAL);
		// lecture de l'étampe temporelle
		stamp = time(NULL);
		// lecture des échantillons analogiques
		buffer[4] = buffer[3];
		buffer[3] = buffer[2];
		buffer[2] = buffer[1];
		buffer[1] = buffer[0];
		buffer[0] = analogIn1.read_u16();
		// calcul de la nouvelle moyenne courante
		if (abs(buffer[0] - buffer[4]) > bufferSize*(maxVoltage >> 3))	//12.5% de la tension max = >> 3
		{
			// génération d'un événement
			mutex.lock();
			mail_t *mail = mail_box.alloc();
			mutex.unlock();
			
			mail->isNum = false; 
			mail->timestamp = stamp;
			
			mutex.lock();
			mail_box.put(mail);
			mutex.unlock();
		}
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
}

int main()
{
	init();
	// initialisation du RTC
	
	// Thread init.
	//Thread digital_thread(digital_read);
	Thread analog_thread(analog_read);
	
	//RtosTimer readNumTimer(digital_signal, osTimerPeriodic, &digital_thread);
	RtosTimer readAnalTimer(analog_signal, osTimerPeriodic, &analog_thread);
	
	readAnalTimer.start(250);
	Thread::wait(osWaitForever);
}
