
/*
Define which board this is
1. Controlboard v1.0
*/
#define CONTROLBOARD 1

/* Display settings */


#define CB_BUF_SIZE 5
#define CB_PRINTER_NR 10

/*
Display controller
1. glcd
*/
#define DISPLAY_CONTROLLER 1

#define SCREEN_CLASS glcd_screen
#define SCREEN_CLASS_HEADER "glcd_screen.h"
#define DISPLAY_CONTROL_CLASS glcd_control
#define DISPLAY_CONTROL_CLASS_HEADER "glcd_control.h"

#define CONTROLBOARD_SW_VERSION "v1.0"
#define CONTROLBOARD_SW_TITLE "3D Control board"
#define CONTROLBOARD_SW_LINE1 "reprapworld.com"

#define STRING_CONNECTING "Connecting"


/*

2. General settings
*/

#define CFG_STARTUP_DELAY 0
#define NUM_PRINTERS 5
#define CFG_REFRESH_STATUS 1000 // frequesncy to poll status in milliseconds
//#define CRASH_ON_COMM_ERROR  1
/*

3. Board settings
*/

#define MAX_MENU_BUTTONS 5
#define BUTTON_TITLE_LENGTH 10
#define MENU_TITLE_LENGTH 26

