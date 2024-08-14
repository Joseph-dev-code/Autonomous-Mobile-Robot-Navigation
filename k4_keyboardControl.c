#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <math.h>
#include <khepera/khepera.h>
#include <signal.h>
// #define for driver mode
#define BIG_SPEED_FACTOR 25
#define SPEED_FACTOR 1
#define MAX_SPEED 1500
#define MIN_SPEED 15
#define DEFAULT_SPEED 200
#define ROTATE_HIGH_SPEED_FACT 0.5
#define ROTATE_LOW_SPEED_FACT 0.75
#define ROT_SPEED_HIGH_TRESH 300
#define STOP_TIME 100000 // us

#define SIGN(x) ((x)>0?1:((x)<0?-1:0))  // sign or zero

static knet_dev_t *dsPic;
//static int quitReq=0;
/*!
 * Drive the robot with the keyboard
 *
 * \param none
 *
 * \return an error code:
 *         - <0  standard error code. See errno.h
 *         - >=0 on success
 *
 */
long timeval_diff(struct timeval *difference, struct timeval *end_time, struct timeval *start_time) {
    struct timeval temp_diff;

    if (difference == NULL) {
        difference = &temp_diff;
    }

    difference->tv_sec = end_time->tv_sec - start_time->tv_sec;
    difference->tv_usec = end_time->tv_usec - start_time->tv_usec;

    /* Using while instead of if below makes the code slightly more robust. */

    while (difference->tv_usec < 0) {
        difference->tv_usec += 1000000;
        difference->tv_sec -= 1;
    }

    return difference->tv_sec * 1000000 + difference->tv_usec;
}


int drive_robot()
{
	int out=0,speed=DEFAULT_SPEED,vsl,vsr,anymove=0;
	char c;
	struct timeval startt,endt;


	kb_clrscr(); // erase screen

	printf("Drive the robot with the keyboard:\n  's' for stop\n  arrows (UP, DOWN, LEFT , RIGHT) for direction\n  PAGE UP/DOWN for changing speed  by small increments\n  Home/End for changing speed by big increments\n  'q' for going back to main menu\n");


	printf("\ndefault parameters:\n  robot speed %d  (%5.1f mm/s)  (min %d, max %d)\n\n",DEFAULT_SPEED,DEFAULT_SPEED*KH4_SPEED_TO_MM_S,MIN_SPEED,MAX_SPEED);

	kb_change_term_mode(1); // change terminal mode for kbhit and getchar to return immediately


	kh4_SetMode(kh4RegSpeed,dsPic );

	gettimeofday(&startt,0x0);

	// loop until 'q' is pushed
	while(!out)
	{
		if(kb_kbhit())
		{
			c=getchar();


			// get special keys
			if (c== 27  )
			{

			 if ((c=getchar())==91) // escape with [
			 {
				 c = getchar();

				 switch(c)
				 {
					case 65: // UP arrow = forward
							 kh4_set_speed(speed ,speed,dsPic );
							anymove=1;
					break;
					case 66: // DOWN arrow = backward
							 kh4_set_speed(-speed ,-speed,dsPic  );
							anymove=1;
					break;

					case 68: // LEFT arrow = left
							if (speed > ROT_SPEED_HIGH_TRESH) // at high speed, rotate too fast
								 kh4_set_speed(-speed*ROTATE_HIGH_SPEED_FACT ,speed*ROTATE_HIGH_SPEED_FACT ,dsPic );
							else
								 kh4_set_speed(-speed*ROTATE_LOW_SPEED_FACT ,speed*ROTATE_LOW_SPEED_FACT ,dsPic );
							anymove=1;
					break;

					case 67: // RIGHT arrow = right
							if (speed > ROT_SPEED_HIGH_TRESH) // at high speed, rotate too fast
								 kh4_set_speed(speed*ROTATE_HIGH_SPEED_FACT ,-speed*ROTATE_HIGH_SPEED_FACT ,dsPic );
							else
								 kh4_set_speed(speed*ROTATE_LOW_SPEED_FACT ,-speed*ROTATE_LOW_SPEED_FACT ,dsPic );
							anymove=1;
					break;

					case 53: // PAGE UP  = speed up
						speed+=SPEED_FACTOR;
				 		if (speed>MAX_SPEED)
				 		{
							speed=MAX_SPEED;
				 		};
				 		c = getchar(); // get last character

				 		 kh4_get_speed(&vsl,&vsr,dsPic );
				 		 kh4_set_speed(SIGN(vsl)*speed ,SIGN(vsr)*speed ,dsPic ); // set new speed, keeping direction with sign
				 		printf("\033[1\033[Krobot speed: %d (%5.1f mm/s)",speed,speed*KH4_SPEED_TO_MM_S); // move cursor to first column, erase line and print info
				 		fflush(stdout); // make the display refresh
				 		anymove=1;
					break;

					case 54: // PAGE DOWN = speed down
						speed-=SPEED_FACTOR;
				 		if (speed<MIN_SPEED)
				 		{
							speed=MIN_SPEED;
				 		};
				 		c = getchar(); // get last character

				 		kh4_get_speed(&vsl,&vsr,dsPic );
				 		kh4_set_speed(SIGN(vsl)*speed ,SIGN(vsr)*speed,dsPic  ); // set new speed, keeping direction with sign
				 		printf("\033[1\033[Krobot speed: %d (%5.1f mm/s)",speed,speed*KH4_SPEED_TO_MM_S); // move cursor to first column, erase line and print info
				 		fflush(stdout); // make the display refresh
				 		anymove=1;
					break;


					default:
					break;
					} // switch(c)
				} // escape with [
				else
				{ // other special key code

					 c = getchar();

					switch(c){

						case 72: // Home  = speed up
							speed+=BIG_SPEED_FACTOR;
					 		if (speed>MAX_SPEED)
					 		{
								speed=MAX_SPEED;
					 		};
					 		//c = getchar(); // get last character

					 		 kh4_get_speed(&vsl,&vsr,dsPic );
					 		 kh4_set_speed(SIGN(vsl)*speed ,SIGN(vsr)*speed ,dsPic ); // set new speed, keeping direction with sign
					 		printf("\033[1\033[Krobot speed: %d (%5.1f mm/s)",speed,speed*KH4_SPEED_TO_MM_S); // move cursor to first column, erase line and print info
					 		fflush(stdout); // make the display refresh
					 		anymove=1;
						break;

						case 70: // End = speed down
							speed-=BIG_SPEED_FACTOR;
					 		if (speed<MIN_SPEED)
					 		{
								speed=MIN_SPEED;
					 		};
					 		//c = getchar(); // get last character

					 		kh4_get_speed(&vsl,&vsr,dsPic );
					 		kh4_set_speed(SIGN(vsl)*speed ,SIGN(vsr)*speed,dsPic  ); // set new speed, keeping direction with sign
					 		printf("\033[1\033[Krobot speed: %d (%5.1f mm/s)",speed,speed*KH4_SPEED_TO_MM_S); // move cursor to first column, erase line and print info
					 		fflush(stdout); // make the display refresh
					 		anymove=1;
						break;

						default:
						break	;

					}

				} // ether special key code


			} // if (c== '\027')
			else
			{
				switch(c)
				{
				 	case 'q': // quit to main menu
				 		out=1;
				   	break;
					case 's': // stop motor
						 kh4_set_speed(0,0,dsPic);
					break;

				 	default:
				   break;
				}
		  }

		  gettimeofday(&startt,0x0);
		} else
		{

			gettimeofday(&endt,0x0);;
			// stop when no key is pushed after some time

			if (anymove &&  (timeval_diff(NULL,&endt,&startt)>STOP_TIME))
			{
				 kh4_set_speed(0 ,0,dsPic );
				anymove=0;
			}

		}


		usleep(10000); // wait some ms
	} // while

	kb_change_term_mode(0); // switch to normal key input mode
	kh4_set_speed(0,0,dsPic );	 // stop robot
	kh4_SetMode(kh4RegIdle,dsPic );
	return 0;
}

int main() {
    if (kh4_init(0, NULL)!= 0) {
        fprintf(stderr, "Failed to initialize khepera.\n");
        return -1;
    }

    dsPic = knet_open("Khepera4:dsPic", KNET_BUS_I2C, 0, NULL);
    if (dsPic == NULL) {
        fprintf(stderr, "Failed to open dsPic device.\n");
        return -1;
    }

    drive_robot();

    knet_close(dsPic);
    //khepera_finalize();

    return 0;
}
