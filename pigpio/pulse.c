/*
   pulse.c

   gcc -Wall -o pulse pulse.c -lpigpio -lrt -lpthread

   sudo ./pulse
   
   Note: executable needs root priveleges so can run with 'sudo' or
   after compiling 
   
   sudo chown root:root pulse 
   sudo chmod 4755  pulse      
   
   //4 sets 'special' id bit that makes executable run with owner (i.e.
   root) priveleges 
*/
#include <stdio.h>    	//printf, etc.
#include <stdlib.h>  	//atoi
#include <signal.h>  	//handle SIGINT  i.e. CTRL-C
#include <unistd.h>  	//porcess ID  and usleep

#include <pigpio.h>


#define DELAY 1  //rampup/down period

#define MAX_DUTY 1000000
const int GPIO_PIN_PWM  = 18; //options for hardware PWM: 12,13,18,19



struct PWMparams{
	uint32_t freq;
	uint32_t duty;
	float pct_duty;
	uint16_t duration_s;
};



struct PWMparams pwm; 





int verifyUserInput(int nparams, char* args[], struct PWMparams* mp){

		int f,s,success;
		float d;		
	
		if(nparams != 3)
		{
			success = 0;
			printf("\nError: 3 parameters required: \
					\n\t1.duty cycle (%%) [0.0f - 100.0f] \
					\n\t2.Frequency (Hz)[1-30M] \
					\n\t3.Duration (s) [0-3600]\n\n");
	
		}else{		
			success = 1;
		
			d = atof(args[0]);
			f = atoi(args[1]);
			s = atoi(args[2]);
			
			mp->freq = f;
			mp->duty = (uint32_t)( d*(MAX_DUTY/100) ); //dutyCycle(%) * 1M/100
			mp->pct_duty = d;   //duty cycle as a percentage [0.0-100.0]
			mp->duration_s = s;

			if(!(s>=0 && s<=3600))
			{	
				success = 0;
				printf("\n\tError: duration(s): %d out of range \n",s);
			}
			if(!(f>0 && f<=30000000)){

				success = 0;
				printf("\n\tError: frequency(Hz): %d out of range \n",f);
			}
			if(!(d>=0 && d<=100)){
				
				success = 0;
				printf("\n\tError: duty cyle [0-100]: %f out of range \n",d);
			}			
		}

		return success;
}

void rampDuty(uint32_t freq, float start_d, float end_d, float dt, float T){
	
	float dc;
	uint32_t n,d,dt_us;
	
	dt_us = (uint32_t)(dt*1000000);
	uint32_t N = (uint32_t)(T/dt);
		
	/*set initial start state*/
	d = (uint32_t)( start_d*(MAX_DUTY/100) ); 
	gpioHardwarePWM(GPIO_PIN_PWM,freq,d);
	usleep(dt_us);
	
	for(n=1; n<=N; n++)
	{
		dc =start_d +(end_d - start_d)*n*dt/T;
		
		d = (uint32_t)( dc*(MAX_DUTY/100) ); 
		gpioHardwarePWM(GPIO_PIN_PWM,freq,d);
		usleep(dt_us);
	}
	
	/*set final end state*/
	d = (uint32_t)( end_d*(MAX_DUTY/100) ); 
	gpioHardwarePWM(GPIO_PIN_PWM,freq,d);
	
		
}


void sig_handler(int signo)
{
	if(signo==SIGINT)
	{	
		printf("\treceived SIGINT\n");
		
		rampDuty(pwm.freq,pwm.pct_duty,0, 0.01 , DELAY);
		/*Reset PWM to 0*/
	    gpioWrite(GPIO_PIN_PWM, 0);
		/* Stop DMA, release resources */
		gpioTerminate();				
		/*exit program*/
		exit(0);	
	}
}


int main(int argc, char *argv[])
{
	/* Check: display uid and effective uid */	
	printf("\nProcess running UID:%d  EUID:%d\n",getuid(),geteuid());
	
	/* Check: display process info*/
	printf("\nTerminate by Ctl+c or \'sudo pkill -2 %d\' (or pkill -2 pulse)\n",getpid());

	
	
	
	
	if( !verifyUserInput(argc-1,argv+1,&pwm) )
	return 1;

	if (gpioInitialise() < 0)
	{
	  fprintf(stderr, "pigpio initialisation failed\n");
	  return 1;
	}
	
	/*Set signal AFTER gpioInit as that resets signal handlers*/
	if(signal(SIGINT,sig_handler)==SIG_ERR)
	printf("\ncan't catch SIGINT\n");
	
	/* Set GPIO modes */
	gpioSetMode(GPIO_PIN_PWM, PI_OUTPUT);
   
			//	gpioHardwarePWM(GPIO_PIN_PWM,pwm.freq,pwm.duty);
	/*Ramps up to selected duty cycle over 1 sec (resoltion 0.01s). Note:extra time added to duration*/
	rampDuty(pwm.freq,0,pwm.pct_duty, 0.01 , DELAY);
	
	
	/*print out actual set parameters*/
	printf("\n\tDuty cycle: %f %%",100*(float)gpioGetPWMdutycycle(GPIO_PIN_PWM)/MAX_DUTY);	
	printf("\n\tFrequency: %d (Hz)",gpioGetPWMfrequency(GPIO_PIN_PWM));
	printf("\n\tDuration: %d seconds\n",pwm.duration_s);	
	
	
	
	
	time_sleep(pwm.duration_s);

	/*ramp down*/
	rampDuty(pwm.freq,pwm.pct_duty,0, 0.01 , DELAY);

	time_sleep(1);
	/*Reset PWM to 0*/
	gpioWrite(GPIO_PIN_PWM, 0);
	/* Stop DMA, release resources */
	gpioTerminate();

   return 0;
}
