/* Target.h
FUNCTIONS RELATING TO THE TARGET SENSOR
*  Author: Alex Desborough 01 May 2024
*/

#ifndef TARGET_H
#define TARGET_H

#include "mbed.h"

void calibrate_target();
void read_target(float handicap = 0);
void print_vals();
void update_historic();
float shot_av();
int shot_detect(float sensitivity = 30);
int shot_detect_loop(float handicap = 0, float sensitivity = 5);
std::pair<float, int> shot_score();
float low4_av(float mag1, float mag2, float mag3, float mag4);



#endif