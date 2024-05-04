/*	halfBaked Laser Game
*	===================================
*
*	Function:               A laser target game
*	                        
*                           
*   Circuit Schematic No.:  1 : 
*	Required Libraries:     mbed.h - for compiling code to the Nucleo
*
*	Authored by:            Alex Desborough 
*                           with code adapted from code from ELEC2645 Labs
*	Date:                   23/03/24
*	Version:                1.0
*	Revision Date:          -
*	MBED Studio Version:    1.4.5
*	MBED OS Version:        -
*	Board:	                NUCLEO L476RG	*/

#include "mbed.h"
#include "N5110.h"                              // SCREEN   //
#include <cstdio>                               // SCREEN   //
#include <string>                               // SCREEN   //
#include <sstream>                              // SCREEN   //
#include "bond.h"                               // IMAGE    //
#include "spriteArrayFull.h"                    // IMAGE    //
#include "targets.h"                            // IMAGE    //
#include "cursor.h"                             // IMAGE    //
#include "target.h"                             // TARGET FUNCTIONS  //
#include "Joystick.h"                           // JOYSTICK FUNCTIONS   //

// ISR
//------------------------------------------------------------------------------------------------------------------
InterruptIn start_button(PB_8);                 // declare start button INTERRUPT pin
InterruptIn select_button(PB_9);                // declare select button INTERRUPT pin

volatile int g_start_button_flag = 0;           // flag 'start status'   ('g_' to distinguish it as global)
volatile int g_select_button_flag = 0;          // flag 'select status'  ('g_' to distinguish it as global)

void start_button_isr();                        // ISR 'start button press' detect and flag,    function prototype
void select_button_isr();                       // ISR 'select button press' detect and flag,   function prototype
void isr_setup();                               // interrupt pins and buttons setup,            function prototype
int isr_button_cases();                         // ISR routine to return case (for switch case statement) dependant on button press
//------------------------------------------------------------------------------------------------------------------

// SCREEN
//------------------------------------------------------------------------------------------------------------------
N5110 lcd_screen(PC_7, PA_9, PB_10, PB_5, PB_3, PA_10);     //Create LCD object                 function prototype
void screen_setup(float setB = 0.6, float setC = 0.49);     // LCD screen setup,                function prototype
void target_calibration();                                  // calibrate target sensor array    function prototype
void set_brightness();                                      // manual adjust via options        function prototype
void set_contrast();                                        // manual adjust via options        function prototype
//------------------------------------------------------------------------------------------------------------------

// JOYSTICK
//------------------------------------------------------------------------------------------------------------------
Joystick joystick(PC_3, PC_2);                  // attach and create joystick object (y, x, button)
DigitalIn joy_button(PC_12);                    // declare joystick button pin

float y_pos = 24;                               // set y coord (initially as centre of LCD screen 42,24)
float x_pos = 42;                               // set x coord (initially as centre of LCD screen 42,24)
int joy_button_press = 0;                       // holds joy button status

int cursorpos();                                // returns cursor menu position as int
int joy_press();                                // detect joy button press return int,          function prototype
void boundary(int x, int y, int x_min = 1,      // set limits of joystick coords,               function prototype
              int x_max = 83, int y_min = 1, 
              int y_max = 47);              
//------------------------------------------------------------------------------------------------------------------

// GAME
//------------------------------------------------------------------------------------------------------------------
int target_select = 0;                          // var to indicate when a target has been selected
int run = 1;
int num_game_targets;
char buffer1[14] = {0};                         // buffer for holding values
char buffer2[14] = {0};                         // buffer for holding values

void startup_ani();                             // Startup animation,                           function prototype
void main_menu();                               // Main menu,                                   function prototype
void play_game();                               // plays the game,                              function prototype
void option_select();                           // goes to options menu (calibrate target),     function prototype
void number_of_game_targets();                  // option to set number of targets in a round,  function prototype
//------------------------------------------------------------------------------------------------------------------


int main(){

    isr_setup();                                // set up interrupt buttons
    screen_setup();                             // set up screen (brightness, contrast etc)
    startup_ani();                              // run the startup animation
    while(1){
    main_menu();                                // run main menu
    }
}
 
//FUNCTIONS:
//------------------------------------------------------------------------------------------------------------------

// start_button event-triggered interrupt 
void start_button_isr()     {       
    g_start_button_flag = 1;
}
// select_button event triggered interrupt
void select_button_isr()    {       
    g_select_button_flag = 1;
}
// interrupt pins and buttons setup (start, select, joybutton, shot)
void isr_setup()            {       
    start_button.mode(PullNone);                // ISR - disable internal pull-down (on start button interrupt pin)
    select_button.mode(PullNone);               // ISR - disable internal pull-down (on select button interrupt pin)
    //joy_button.mode(PullUp);                    // ISR - disable internal pull-down (on joystick button interrupt pin)

    start_button.rise(&start_button_isr);       // ISR - look for rising edge on start button to call isr
    select_button.rise(&select_button_isr);     // ISR - look for rising edge on select button to call isr
    //joy_button.rise(&joy_button_isr);           // ISR - look for rising edge on select button to call isr
}
int isr_button_cases()     {
            // (START BUTTON ISR) 
        if (g_start_button_flag) {              // ISR - check START flag (interrupt true when flag = '1') run code when true
            g_start_button_flag = 0;            // ISR - clear the interupt flag (set flag to '0')

            int select_case = 1;                // set case value to 1
            return select_case;
        }

        // (SELECT BUTTON ISR)
        else if (g_select_button_flag) {        // ISR - check SELECT flag (interrupt true when flag = '1') run code when true
            g_select_button_flag = 0;           // ISR - clear the interrupt flag (set flag to '0')

            int select_case = 2;                // set case value to 2
            return select_case;
        }
        else{
            return 0;                           // set case value to 0
            }
}
// LCD screen setup
void screen_setup(float setB, float setC)         {       
    lcd_screen.init(LCD_Type(1));               // SCREEN - intitialise
    lcd_screen.setBrightness(setB);              // SCREEN - set brightness
    lcd_screen.setContrast(setC);               // SCREEN - set contrast
}
// limit x, y coord limits (set limits inside this function here)
void boundary(int x, int y, int x_min, int x_max, int y_min, int y_max) {        
    if      ( x < x_min  )      { x_pos = x_min  ;}  //ensure that the point doesnt pass beyond the left hand side of the screen
    else if ( x > x_max  )      { x_pos = x_max  ;}  //ensure that the point doesnt pass beyond the right hand side of the screen-
        
    if      ( y < y_min  )      { y_pos = y_min  ;}  //ensure that the point doesnt pass beyond the top of the screen
    else if ( y > y_max  )      { y_pos = y_max  ;}  //ensure that the point doesnt pass beyond the bottom of the screen
}
// startup animation sequence
void startup_ani()          {       
    lcd_screen.clear();                                             // Display startup loading message
    lcd_screen.printString("Loading..", 20, 2);                     // Display startup loading message
    lcd_screen.refresh();                                           // Display startup loading message
    ThisThread::sleep_for(2000ms);                                  // Display startup loading message 

    for(int i = 0; i<49; i++){                                      // Display startup 007 animation (49 frames)
        lcd_screen.clear();                                         // Display startup 007 animation
        lcd_screen.drawSprite(0, 0, 48, 76, (int *)bond::bond[i]);  // Display startup 007 animation
        lcd_screen.refresh();                                       // Display startup 007 animation
        ThisThread::sleep_for(50ms);                                // Display startup 007 animation
    } 
    for(int i = 0; i<10; i++){                                      // Display startup strobe animation (10 frames)
        lcd_screen.clear();                                         // Display startup strobe animation
        lcd_screen.drawRect(0, 0, 84, 48, FILL_BLACK);              // Display startup strobe animation
        lcd_screen.refresh();                                       // Display startup strobe animation
        ThisThread::sleep_for(50ms);                                // Display startup strobe animation
        lcd_screen.drawRect(0, 0, 84, 48, FILL_WHITE);              // Display startup strobe animation
        lcd_screen.refresh();                                       // Display startup strobe animation
        ThisThread::sleep_for(50ms);                                // Display startup strobe animation
    }
    for(int i = 0; i<10; i++){                                      // Display startup title strobe animation (10 frames)
        lcd_screen.clear();                                         // Display startup title strobe animation
        lcd_screen.inverseMode();                                   // Display startup title strobe animation
        lcd_screen.drawSprite(0, 0, 48, 84, (int *)spriteArrayFull::spriteArrayFull[0]);  
        lcd_screen.refresh();                                       // Display startup title strobe animation
        ThisThread::sleep_for(100ms);                               // Display startup title strobe animation
        lcd_screen.normalMode();                                    // Display startup title strobe animation
        lcd_screen.drawSprite(0, 0, 48, 84, (int *)spriteArrayFull::spriteArrayFull[0]); 
        ThisThread::sleep_for(100ms);                               // Display startup title strobe animation
    }
    lcd_screen.clear();                                             // Display startup title animation
    lcd_screen.drawSprite(0, 0, 48, 84, (int *)spriteArrayFull::spriteArrayFull[0]);  
    ThisThread::sleep_for(2500ms);                                  // Display startup title animation

}
// main menu screen prompt
void main_menu()            {       
    int select_case = 0;  
    while (1) {                                 // loop
        
        select_case = isr_button_cases();

        switch (select_case) {
            case 1:
                play_game();
                break;
            case 2:
                option_select();
                break;
            default:
                break;
        }
    lcd_screen.clear();
    lcd_screen.printString("Press start", 5, 1);
    lcd_screen.printString("to play", 20, 2);
    lcd_screen.printString("Press select", 5, 4);
    lcd_screen.printString("for options", 10, 5);
    lcd_screen.refresh(); 
    sleep();
    }
}
// update [joy_button_press] 1 =press
int joy_press(){
    return joy_button.read();
}
// play game function info ...
void play_game(){
    int select_case = 0;
    int play_loop = num_game_targets;
    float total_score = 0;
    float average_shot = 0;

    for (int i= play_loop; i > 0; i--) {
            lcd_screen.clear();                              
            lcd_screen.printString("get ready..", 10, 1);   // Display shot cycle start sequence message
            for(int i = 3; i > 0 ; i--){                    // Display shot cycle countdown
                sprintf(buffer1, "%d", i);
                lcd_screen.printString(buffer1, 35, 2);
                lcd_screen.refresh();
                ThisThread::sleep_for(800ms);
            } 
            lcd_screen.clear();
            lcd_screen.printString("Fire", 30, 0);          //Display shot cycle start sequence message
            lcd_screen.printString("when ready..", 10, 1);
            lcd_screen.drawSprite(26, 18, 30, 30, (int *)targets::targets[13]);
            lcd_screen.refresh();
                                                    ThisThread::sleep_for(1000ms); ///testing  
            
            
            int shot = 0;
            int run = 1;
            float handicap = 0;
            float sensitivity = 5;

            read_target(handicap); // initialise historic values

            while (run) {
                read_target(handicap);
                print_vals();
                shot = shot_detect(sensitivity);
                printf("shot detected? = %d", shot);
                if (shot == 1) {break;}
            }

            if (shot == 1){   // pass the shot detect loop handicap and sensitivity arguments
                lcd_screen.clear();
                lcd_screen.printString("hit!", 35, 0);
                lcd_screen.refresh();
                ThisThread::sleep_for(1000ms);
                lcd_screen.printString("score:", 0, 2);
                sprintf(buffer1, "%.1f", shot_score().first);
                sprintf(buffer2, "%d", shot_score().second);
                lcd_screen.printString(buffer1, 0, 3);

                lcd_screen.drawSprite(54, 18, 30, 30, (int *)targets::targets[shot_score().second]);  
                lcd_screen.refresh();
                ThisThread::sleep_for(3000ms);

            }

            if (isr_button_cases() == 1) {
                lcd_screen.clear();
                lcd_screen.printString("woo!", 0, 4);
                lcd_screen.refresh();
            }

            total_score += shot_score().first;
            average_shot = (average_shot + shot_score().first)/2;

            lcd_screen.clear();
            lcd_screen.printString("Total Score:", 0, 0);
            sprintf(buffer1, "%.1f", total_score);
            lcd_screen.printString(buffer1, 35, 1);
            lcd_screen.printString("average shot:", 0, 2);
            sprintf(buffer2, "%.1f", average_shot);
            lcd_screen.printString(buffer2, 35, 3);
            lcd_screen.refresh();
            ThisThread::sleep_for(2000ms);


    }
}
// option select function info ...
void option_select(){
    Joystick joystick(PC_3, PC_2);  //attach and create joystick object
    joystick.init();                // initialise joystick
    joy_button.mode(PullUp);        // initialise joystick button

    while (1) {
        lcd_screen.clear();
        lcd_screen.printString("set target qty", 0, 0);
        lcd_screen.printString("calibrate", 0, 1);
        lcd_screen.printString("set brightness", 0, 2);
        lcd_screen.printString("set contrast", 0, 3);
        lcd_screen.printString("calibrate", 0, 4);
        lcd_screen.printString("main menu", 0, 5);
        lcd_screen.drawSprite(x_pos, y_pos, 5, 5, (int*)cursor::cursor[0]);


        if(!joy_press()){
            lcd_screen.drawSprite(x_pos, y_pos, 5, 5, (int*)cursor::cursor[1]);
                    int select_case = cursorpos();

            switch (select_case) {
                case 1:
                    number_of_game_targets();
                    break;
                case 2:
                    target_calibration();
                    break;
                case 3:
                    set_brightness();
                    break;
                case 4:
                    set_contrast();
                    break;
                case 5:
                    target_calibration();
                    break;
                case 6:
                    main_menu();
                    break;
            }
            
            lcd_screen.refresh();
            ThisThread::sleep_for(10ms);
        }

        if     (joystick.get_direction() == N)  {  y_pos--  ;             }
        else if(joystick.get_direction() == S)  {  y_pos++  ;             }
        else if(joystick.get_direction() == E)  {  x_pos++  ;             }
        else if(joystick.get_direction() == W)  {  x_pos--  ;             }
        else if(joystick.get_direction() == NE) {  y_pos--  ;    x_pos++; }
        else if(joystick.get_direction() == NW) {  y_pos--  ;    x_pos--; }
        else if(joystick.get_direction() == SE) {  y_pos++  ;    x_pos++; }
        else if(joystick.get_direction() == SW) {  y_pos++  ;    x_pos--; }

        boundary(x_pos, y_pos); //pass the new x,y position to function   
        lcd_screen.refresh();
        ThisThread::sleep_for(10ms);
    }
}
int cursorpos(){
    if      (y_pos > 0 & y_pos < 9) {return 1;}
    else if (y_pos > 8 & y_pos < 17) {return 2;}
    else if (y_pos > 16 & y_pos < 25) {return 3;}
    else if (y_pos > 24 & y_pos < 33) {return 4;}
    else if (y_pos > 32 & y_pos < 41) {return 5;}
    else if (y_pos > 40 & y_pos < 49) {return 6;}
    else return 0;
}
void target_calibration(){

    lcd_screen.clear();
    lcd_screen.printString("set up target", 0, 1);
    lcd_screen.printString("then", 0, 2);
    lcd_screen.printString("press select", 0, 3);
    lcd_screen.printString("to calibrate..", 0, 4);
    lcd_screen.refresh();

    while(1){
        if (isr_button_cases() == 2) {
            lcd_screen.clear();
            lcd_screen.printString("calibrating...", 0, 2);
            calibrate_target();
            lcd_screen.refresh();
            ThisThread::sleep_for(2000ms);
            lcd_screen.clear();
            lcd_screen.refresh();
            break;
        }
        sleep();
    }
    option_select();
    
}
void number_of_game_targets(){
        joystick.init();                // initialise joystick
        joy_button.mode(PullUp);        // initialise joystick button

    while(1){
        lcd_screen.clear();
        lcd_screen.printString("Targ. in round:", 0, 0);
        lcd_screen.printString("3", 0, 1);
        lcd_screen.printString("5", 0, 2);
        lcd_screen.printString("10", 0, 3);
        lcd_screen.printString("20", 0, 4);
        lcd_screen.printString("main menu", 0, 5);
        lcd_screen.drawSprite(x_pos, y_pos, 5, 5, (int*)cursor::cursor[0]);
        lcd_screen.refresh();

        if(!joy_press()){
            lcd_screen.drawSprite(x_pos, y_pos, 5, 5, (int*)cursor::cursor[1]);
            int select_case = cursorpos();

            switch (select_case) {
                case 1:
                    option_select();
                    break;
                case 2:
                    num_game_targets = 3;
                    option_select();
                    break;
                case 3:
                    num_game_targets = 5;
                    option_select();
                    break;
                case 4:
                    num_game_targets = 10;
                    option_select();
                    break;
                case 5:
                    num_game_targets = 20;
                    option_select();
                    break;
                case 6:
                    option_select();
                    break;
            }
            
            lcd_screen.refresh();
            ThisThread::sleep_for(10ms);
        }


        if     (joystick.get_direction() == N)  {  y_pos--  ;             }
        else if(joystick.get_direction() == S)  {  y_pos++  ;             }
        else if(joystick.get_direction() == E)  {  x_pos++  ;             }
        else if(joystick.get_direction() == W)  {  x_pos--  ;             }
        else if(joystick.get_direction() == NE) {  y_pos--  ;    x_pos++; }
        else if(joystick.get_direction() == NW) {  y_pos--  ;    x_pos--; }
        else if(joystick.get_direction() == SE) {  y_pos++  ;    x_pos++; }
        else if(joystick.get_direction() == SW) {  y_pos++  ;    x_pos--; }

        boundary(x_pos, y_pos); //pass the new x,y position to function   
        lcd_screen.refresh();
        ThisThread::sleep_for(10ms);

    
    }


}
void set_brightness(){
        joystick.init();                // initialise joystick
        joy_button.mode(PullUp);        // initialise joystick button
        int loop = 1;
    while(loop){
        lcd_screen.clear();
        lcd_screen.printString("Press Joy ", 0, 0);
        lcd_screen.printString("button when", 0, 1);
        lcd_screen.printString("brightness", 0, 2);
        lcd_screen.printString("correct", 0, 3);
        lcd_screen.printString("", 0, 4);
        lcd_screen.printString("", 0, 5);
        lcd_screen.drawSprite(45, 25, 30, 30, (int*)targets::targets[0]);
        lcd_screen.refresh();
        lcd_screen.setBrightness(joystick.get_mag());
        
        switch (joy_button.read()){
            case 0:
            loop = 0;
            break;
            case 1:
            loop = 1;
            break;
            break;
            default:
            break;
        }
        lcd_screen.refresh();
        ThisThread::sleep_for(100ms);

    
    }

}
void set_contrast(){
        joystick.init();                // initialise joystick
        joy_button.mode(PullUp);        // initialise joystick button
        int loop = 1;
    while(loop){
        lcd_screen.clear();
        lcd_screen.printString("Press Joy ", 0, 0);
        lcd_screen.printString("button when", 0, 1);
        lcd_screen.printString("contrast", 0, 2);
        lcd_screen.printString("correct", 0, 3);
        lcd_screen.printString("", 0, 4);
        lcd_screen.printString("", 0, 5);
        lcd_screen.drawSprite(45, 25, 30, 30, (int*)targets::targets[0]);
        lcd_screen.refresh();
        lcd_screen.setContrast(joystick.get_mag());
        
        switch (joy_button.read()){
            case 0:
            loop = 0;
            break;
            case 1:
            loop = 1;
            break;
            break;
            default:
            break;
        }
        lcd_screen.refresh();
        ThisThread::sleep_for(100ms);
}
}