/* Target.cpp
FUNCTIONS RELATING TO THE TARGET SENSOR
*  Author: Alex Desborough 01 May 2024
*/

#include "target.h"
#include <utility>

AnalogIn Pin_LightSensor0(PC_0);        //define analogue input to read potentiometer voltage
AnalogIn Pin_LightSensor1(PC_1);
AnalogIn Pin_LightSensor2(PB_0);
AnalogIn Pin_LightSensor3(PA_1);
AnalogIn Pin_LightSensorX(PA_0);

float LightSensor_val_0 = 0;            // Create & initialise float variable to store the current LDR val 
float LightSensor_val_1 = 0; 
float LightSensor_val_2 = 0; 
float LightSensor_val_3 = 0; 
float LightSensor_val_X = 0; 

float LightSensor_hist_val_0 = 0;       // Create & initialise float variable to store the historic LDR val 
float LightSensor_hist_val_1 = 0; 
float LightSensor_hist_val_2 = 0; 
float LightSensor_hist_val_3 = 0; 
float LightSensor_hist_val_X = 0;

float LightSensor0_offset = 0;          // Create & initialise float variable to store the LDR offset 
float LightSensor1_offset = 0;
float LightSensor2_offset = 0;
float LightSensor3_offset = 0;
float LightSensorX_offset = 0;

float zero_offset = 0;                  // var to bring ambient to zero

float LightSensor0_diff = 0;            // Create & initialise float variable to store the LDR difference
float LightSensor1_diff = 0;
float LightSensor2_diff = 0;
float LightSensor3_diff = 0;
float LightSensorX_diff = 0;

float target_av_val = 0;                // var to store calibration value





void calibrate_target() {
    target_av_val = ((Pin_LightSensor0.read() + Pin_LightSensor1.read() + Pin_LightSensor2.read() + Pin_LightSensor3.read() + Pin_LightSensorX.read())/ 5 * 50);  // set target av val
    
    LightSensor_val_0 = Pin_LightSensor0.read() * 50;       // update the LDR val 
    LightSensor_val_1 = Pin_LightSensor1.read() * 50; 
    LightSensor_val_2 = Pin_LightSensor2.read() * 50; 
    LightSensor_val_3 = Pin_LightSensor3.read() * 50; 
    LightSensor_val_X = Pin_LightSensorX.read() * 50;

    LightSensor0_offset = (LightSensor_val_0 - target_av_val);  // find the offsets of each LDR from the average
    LightSensor1_offset = (LightSensor_val_1 - target_av_val);
    LightSensor2_offset = (LightSensor_val_2 - target_av_val);
    LightSensor3_offset = (LightSensor_val_3 - target_av_val);
    LightSensorX_offset = (LightSensor_val_X - target_av_val);

    LightSensor_val_0 = (LightSensor_val_0 - LightSensor0_offset); // apply the offsets to calibrate LDRs
    LightSensor_val_1 = (LightSensor_val_1 - LightSensor1_offset);
    LightSensor_val_2 = (LightSensor_val_2 - LightSensor2_offset);
    LightSensor_val_3 = (LightSensor_val_3 - LightSensor3_offset);
    LightSensor_val_X = (LightSensor_val_X - LightSensorX_offset);

    zero_offset = LightSensor_val_0 ;  // use one of the values to set the zero offset

}
void read_target(float handicap)      {
    
    update_historic(); // load prevoius values to historic

    LightSensor_val_0 = Pin_LightSensor0.read() * 50;       // update the LDR val 
    LightSensor_val_1 = Pin_LightSensor1.read() * 50; 
    LightSensor_val_2 = Pin_LightSensor2.read() * 50; 
    LightSensor_val_3 = Pin_LightSensor3.read() * 50; 
    LightSensor_val_X = Pin_LightSensorX.read() * 50;

    LightSensor_val_0 = (LightSensor_val_0 - LightSensor0_offset - zero_offset); // apply the offsets to calibrate LDRs
    LightSensor_val_1 = (LightSensor_val_1 - LightSensor1_offset - zero_offset);
    LightSensor_val_2 = (LightSensor_val_2 - LightSensor2_offset - zero_offset);
    LightSensor_val_3 = (LightSensor_val_3 - LightSensor3_offset - zero_offset);
    LightSensor_val_X = (LightSensor_val_X - LightSensorX_offset - zero_offset + handicap); // 



}
void print_vals()       {
    printf("  val1 %f\t", LightSensor_val_0);
    printf("  val2 %f\t", LightSensor_val_1);
    printf("  val3 %f\t", LightSensor_val_2);
    printf("  val4 %f\t", LightSensor_val_3);
    printf("  BULL %f\n", LightSensor_val_X);
}
void update_historic()  {
    LightSensor_hist_val_0 = LightSensor_val_0; // update a historic value
    LightSensor_hist_val_1 = LightSensor_val_1;
    LightSensor_hist_val_2 = LightSensor_val_2;
    LightSensor_hist_val_3 = LightSensor_val_3;
    LightSensor_hist_val_X = LightSensor_val_X;
}
int shot_detect(float sensitivity)       {
    //sensitivity = 15; // set the sensitivity for shot detection

    int shot_detected = 0; // var to store a detected shot (1 or 0)

    // compare the current val with the historic to see if it is greater than the sensitivity amount
    if (abs(LightSensor_val_0 - LightSensor_hist_val_0) > sensitivity || abs(LightSensor_val_1 - LightSensor_hist_val_1) > sensitivity || 
        abs(LightSensor_val_2 - LightSensor_hist_val_2) > sensitivity || abs(LightSensor_val_3 - LightSensor_hist_val_3) > sensitivity || 
        abs(LightSensor_val_X - LightSensor_hist_val_X) > sensitivity) {
    shot_detected = 1;
    }
    return shot_detected;

}
int shot_detect_loop(float handicap, float sensitivity)  {
    int run = 1;
    int shot = 0;
    while (run) {
        read_target(handicap);
        print_vals();
        //printf("shot detected? = %d", shot_detect());
        if (shot_detect(sensitivity) == 1) {
        //run = 0;
        shot = 1;
        break;
        }
        ThisThread::sleep_for(10ms);
    }
    return shot;
}
float shot_av()         {
    
    float shot_av_val = ((LightSensor_val_0 + LightSensor_val_1 + 
                          LightSensor_val_2 + LightSensor_val_3 + 
                          LightSensor_val_X) / 5);          // get the average sensor magnitude of the shot
    return shot_av_val;
}



std::pair<float, int> shot_score() {

    std::pair<float, int> scoreinfo ; //  struct for holding shot data
    scoreinfo.first = 5; // calculated score
    scoreinfo.second = 7; // shot location (given as clockface, 0 for bullseye)

    float max_mag = 0;
    float bull_mag = 0;
    int max_sensor_num = 4;
    int clockface = 0;


    float sensorvals[5] = {LightSensor_val_0,               // array to hold the sensor vals
                           LightSensor_val_1, 
                           LightSensor_val_2, 
                           LightSensor_val_3,
                           LightSensor_val_X
                        };

    bull_mag = sensorvals[4];
    
    for(int i=0; i<5; i++){                                 // find the highest magnitude from the array
        if(sensorvals[i] > max_mag) {
             max_mag = sensorvals[i];
             max_sensor_num = i;
        }
    };

    switch (max_sensor_num) {
        case 0:
            clockface = 9;
            break;
        case 1:
            clockface = 6;
            break;
        case 2:
            clockface = 12;
            break;
        case 3:
            clockface = 3;
            break;
        case 4:
            clockface = 0;
            break;
    }

    // take the max value and subtract the average of the remaining 4 to get a score:
    //score = max_mag - low4_av(float mag1, float mag2, float mag3, float mag4)

    scoreinfo.first = bull_mag;
    scoreinfo.second = clockface;



    return scoreinfo;
}

float low4_av(float mag1, float mag2, float mag3, float mag4)          {
    
    float low_av_val = ((mag1 + mag2 + mag3 + mag4) / 4);          // get the average of the 4 lowest magnitudes of the shot
    return low_av_val;
}