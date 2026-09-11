#include "stm32f10x.h"
#include "delay.h"
#include "sg90.h"
#include "beep.h"
#include "blub.h"
#include "light.h"
#include <stdio.h>

#define LIGHT_THRESHOLD  1800
#define ANGLE_MIN 0
#define ANGLE_MAX 90

int main(void)
{
    delay_init();
    sg90_init();
    beep_init();
    bulb_init();
    light_init();
    uint8_t run_state = 0;
    uint8_t servo_angle = ANGLE_MIN;
    uint8_t dir = 1; //1????,0????
	


    while(1)
    {
        uint16_t light_data = get_light();
        light_servo_beep_ctrl(light_data, LIGHT_THRESHOLD, &run_state);

        if(run_state == 1)
        {
            if(dir == 1)
            {
                servo_angle += 1;
                if(servo_angle >= ANGLE_MAX)
                {
                    dir = 0;
                }
            }
            else
            {
                servo_angle -=1;
                if(servo_angle <= ANGLE_MIN)
                {
                    dir = 1;
                }
            }
            sg90_set_angle(servo_angle);
        }
        else
        {
            servo_angle = ANGLE_MIN;
            dir = 1;
            sg90_set_angle(ANGLE_MIN);
        }

        char adc_buf[40];
        sprintf(adc_buf,"ADC:%d, Run:%d, Angle:%d\r\n",light_data,run_state,servo_angle);
        bulb_send_str(adc_buf);
        delay_ms(500); 
    }
}
