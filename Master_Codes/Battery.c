//code to get robot current/reamaining battery capacity.

#include <khepera/khepera.h>
#include <signal.h>

static knet_dev_t *dsPic; // robot pic microcontroller access
static int quitReq = 0;   // quit variable for loop

// Ctrl+C handler
static void ctrlc_handler(int sig)
{
  quitReq = 1;
  kh4_set_speed(0, 0, dsPic); // stop robot
  kh4_SetMode(kh4RegIdle, dsPic);
  kb_change_term_mode(0);    // revert to original terminal if called
  exit(0);
}

int main()
{
  char Buffer[100];
  int sensors[12];

  printf("\nKhepera 4 small example program\n");

  if (kh4_init(0, NULL) != 0)
  {
    printf("\nERROR: could not initiate the libkhepera!\n\n");
    return -1;
  }

  dsPic = knet_open("Khepera4:dsPic", KNET_BUS_I2C, 0, NULL);

  if (dsPic == NULL)
  {
    printf("\nERROR: could not initiate communication with Kh4 dsPic\n\n");
    return -2;
  }

  // Motors controllers initialization
  // ... (skipping the motor control initialization part)

  signal(SIGINT, ctrlc_handler);

  // IR proximity sensors example

 // while (!kb_kbhit())
 // {
	  kh4_battery_status(Buffer,dsPic);
	  printf("\nBattery:\n status (DS2781) : 0x%x\n",Buffer[0]);
	  printf("remaining capacity: %4.0f mAh\n",(Buffer[1] | Buffer[2]<<8)*1.6);
	  usleep(200000); // wait 200ms
//  }

  return 0;
}

