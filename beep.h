#ifndef __BEEP_H
#define __BEEP_H

void beep_init(void);
void beep_on(void);
void beep_off(void);

void beep_set_force(uint8_t en);
uint8_t get_force_state(void);
void light_servo_beep_ctrl(uint16_t val, uint16_t TH, uint8_t *p_run);

#endif
