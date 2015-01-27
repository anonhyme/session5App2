#include "mbed.h"
#include "rtos.h"

#define DIGITAL_SIGNAL 0x01
#define ANALOG_SIGNAL 0x02

DigitalIn digitalIn1(p15);
AnalogIn analogIn1(p19);
BusOut leds(LED4, LED3, LED2, LED1);
Serial pc(USBTX, USBRX);

Mutex mutex;

// VARIABLES GLOBALES
unsigned short readAnalog;


typedef struct 
{
	bool isNum;
	int timestamp;
} mail_t;


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
			int lastEventState = 1;
			int state[2] = {1,1};
			int current_state = 1;
			int isEvent = 0;
			int i = 0;
			
//			time_t stamp;


		while (true){
			i = i%2;
			Thread::signal_wait(DIGITAL_SIGNAL);
			time_t stamp;
			state[i] = digitalIn1.read();
			current_state = state[i];
			i++;
			
//			if((current_state != lastEventState) && !isEvent) {
//				isEvent = 1
//			}
			// verification event et prise en charge rebond
			if((state[0] != lastEventState) && (state[0] == state[1])) {
				//printf("\rEvent 	=>	lastEventState:	%d		state1:	%d		state2:	%d							\n\r", lastEventState, state[0], state[1] );
				led4 = 1;
				led3 = lastEventState;
				led2 = state[0];
				led1 = state[1];
				lastEventState = state[0];
				mutex.lock();
				mail_t *mail = mail_box.alloc();
				mutex.unlock();
			
			mail->isNum = true;
			mail->timestamp = time(NULL);
			
		mutex.lock();
		mail_box.put(mail);
		mutex.unlock();
			}
			else{
				led4 = 0;
			}

			
//		// lecture de l'étampe temporelle 
//		stamp = time(NULL);

	}
}


void analog_signal(void const *args)
{
	Thread* t = (Thread *) args;
	readAnalog = analogIn1.read_u16();
	t->signal_set(ANALOG_SIGNAL);
}

void analog_read(void const *args)
{
	const int bufferSize = 5;
	const unsigned short maxVoltage = 0xFFFF;
	unsigned short buffer[bufferSize] = {0,0,0,0,0}; 
	unsigned short oldValue;
	time_t stamp;
	
	while (true)
	{
		// synchronisation sur la période d'échantillonnage
		Thread::signal_wait(ANALOG_SIGNAL);
		// lecture de l'étampe temporelle
		stamp = time(NULL);
		// lecture des échantillons analogiques
		oldValue = buffer[4];
		buffer[4] = buffer[3];
		buffer[3] = buffer[2];
		buffer[2] = buffer[1];
		buffer[1] = buffer[0];
		buffer[0] = readAnalog;
				
		//printf("\nBuffer[0]: %x 	\n\r", buffer[0]);
		//printf("\nBuffer[0] = %d \n\r", abs(buffer[0]));
		
		// calcul de la nouvelle moyenne courante
		if (abs(buffer[0] - oldValue) > bufferSize*(maxVoltage >> 3))	//12.5% de la tension max = >> 3
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
			time_t timeStamp = (mail->timestamp);
				pc.printf("Source: %s                 \n\r"   , mail->isNum ? "Numerique" : "Analogique");
			  pc.printf("Time: %s										\n\r" , ctime(&timeStamp));
				
				mutex.lock();
				mail_box.free(mail);
				mutex.unlock();
		}
	}
}

void init() {
	// initialisation du RTC
	// 1422222222
	set_time(0);
	// speed up serial
	pc.baud(19200);
	// set default analog value
	
}

int main()
{
	init();
	// initialisation du RTC
	
	Thread digital_thread(digital_read);
	Thread analog_thread(analog_read);
	Thread collection_thread(collection);
	
	RtosTimer readNumTimer(digital_signal, osTimerPeriodic, &digital_thread);
	RtosTimer readAnalTimer(analog_signal, osTimerPeriodic, &analog_thread);
	
	
	readNumTimer.start(50);
	readAnalTimer.start(250);
	while(1) {
	}
	//Thread::wait(osWaitForever);
}
