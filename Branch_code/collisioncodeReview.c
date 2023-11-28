#include <khepera/khepera.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

static knet_dev_t *dsPic;
char Buffer[100];
int i;
short sensors[12];	//array for Ir sensor values
int lpos = 0;		//variables for wheel positions
int rpos = 0;
int length = 6; 	//Set the travel distance for rotations at 6cm
double fpos;


void drehlinks(){ //counterclockwise
	kh4_SetMode( kh4RegPosition,dsPic );
	kh4_get_position(&lpos,&rpos,dsPic);
	fpos = length*(10.0/KH4_PULSE_TO_MM);
	kh4_set_position(lpos-(long)fpos,rpos+(long)fpos,dsPic);
	sleep(1);
}
void drehrechts(){ //clockwise
		kh4_SetMode( kh4RegPosition,dsPic );
		kh4_get_position(&lpos,&rpos,dsPic);
		fpos = length*(10.0/KH4_PULSE_TO_MM);
		kh4_set_position(lpos+(long)fpos,rpos-(long)fpos,dsPic);
		sleep(1);
	}
void drehlinksleicht(){ //turn left slightly
		kh4_SetMode( kh4RegPosition,dsPic );
		kh4_get_position(&lpos,&rpos,dsPic);
		fpos = length*(5.0/KH4_PULSE_TO_MM);
		kh4_set_position(lpos-(long)fpos,rpos+(long)fpos,dsPic);
		sleep(1);
	}
	void drehrechtsleicht(){  //turn slightly to the right
		kh4_SetMode( kh4RegPosition,dsPic );
		kh4_get_position(&lpos,&rpos,dsPic);
		fpos = length*(5.0/KH4_PULSE_TO_MM);
		kh4_set_position(lpos+(long)fpos,rpos-(long)fpos,dsPic);
		sleep(1);
	}

int main(int argc, char *argv[]) {

	  if ( kh4_init(argc ,argv)!=0)
	  {
	  	printf("\nERROR: could not initiate the libkhepera!\n\n");
	  	return -1;
	  }
	  dsPic  = knet_open( "Khepera4:dsPic" , KNET_BUS_I2C , 0 , NULL );
		if ( dsPic==NULL)
	  {
	  	printf("\nERROR: could not initiate communication with Kh4 dsPic\n\n");
	  	return -2;
	  }

	int pmarg=20;
	kh4_SetPositionMargin(pmarg,dsPic);

	int kp=10;
	int ki=5;
	int kd=1;
	kh4_ConfigurePID(kp, ki, kd, dsPic); //PID Regler konfigurieren

	int accinc=3;
	int accdiv=0;
	int minspacc=20;
	int minspdec=1;
	int maxsp=400;
	kh4_SetSpeedProfile(accinc,accdiv,minspacc,minspdec,maxsp,dsPic);

	kh4_SetMode(kh4RegIdle, dsPic);	//Switch robot to training mode

	kh4_SetRGBLeds(0,0,0,0,0,0,0,0,0,dsPic); // Led red
	printf("To start, please press any key...");
	getchar();
	kh4_SetMode( kh4RegSpeed,dsPic );

	while(!kb_kbhit())
	{
		kb_clrscr();
		printf("Press any key to exit!\n\n!\n\n");
		printf("IR proximity sensors:\n");
		kh4_proximity_ir(Buffer, dsPic);
		for (i=0;i<12;i++)
		{
			sensors[i]=(Buffer[i*2] | Buffer[i*2+1]<<8);
		}
		printf("Left: %d Front left: %d  Front: %d  Front Right: %d Right: %d \n",sensors[1], sensors[2],sensors[3],sensors[4], sensors[5]);

		if (((sensors[1]>500)||(sensors[2]>500))&&((sensors[4]>500)||(sensors[5]>500)))
		{
			drehrechts(); //clockwise
			drehrechts(); //clockwise
			kh4_SetMode( kh4RegPosition,dsPic );
			kh4_get_position(&lpos,&rpos,dsPic);
			fpos = length*(10.0/KH4_PULSE_TO_MM);
			kh4_set_position(lpos+(long)fpos,rpos+(long)fpos,dsPic);
			sleep(1);
		}
		if (sensors[3] > 300)
		{
			if(sensors[2]>sensors[4])
			{
				drehrechts(); //clockwise
			}
			else if(sensors[4]>sensors[2])
			{
				drehlinks(); //counterclockwise
			}
		}
		if (sensors[2] > 700)
		{
			drehrechtsleicht(); //turn slightly to the right
		}
		if (sensors[4] > 700)
		{
			drehlinksleicht(); //turn left slightly
		}
		if (sensors[1] > 750)
		{
			drehrechtsleicht();  //turn slightly to the right
		}
		if (sensors[5] > 750)
		{
			drehlinksleicht(); //turn left slightly
		}
		kh4_SetMode( kh4RegSpeed,dsPic );
		kh4_set_speed(200,200, dsPic);
		usleep(20000); // wait 20ms
	}

	kh4_set_speed(0,0, dsPic);

	while(1) {
		kh4_SetRGBLeds(63,0,0,0,63,0,0,0,63,dsPic); // Led rot
		usleep(100000); // 1 Sekunde warten
		kh4_SetRGBLeds(0,0,63,63,0,0,0,63,0,dsPic);
		usleep(100000);
		kh4_SetRGBLeds(0,63,0,0,0,63,63,0,0,dsPic);
		usleep(100000);
	}

	return 0;
}
