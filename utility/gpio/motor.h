#ifndef __MOTOR_H_
#define __MOTOR_H_

extern unsigned int speed_count;//占空比计数器 50次一周期
extern char front_left_speed_duty;
extern char front_right_speed_duty;
extern char behind_left_speed_duty;
extern char behind_right_speed_duty;

extern unsigned char tick_5ms;//5ms计数器，作为主函数的基本周期
extern unsigned char tick_1ms;//1ms计数器，作为电机的基本计数器
extern unsigned int speed_count;//占空比计数器 50次一周期
extern unsigned char bt_rec_flag;//蓝牙控制标志位

void CarMove(void);
void CarGo(void);
void CarBack(void);
void CarLeft(void);
void CarRight(void);
void CarStop(void);
void MotorInit(void);
#endif

