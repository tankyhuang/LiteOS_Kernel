#ifndef __GPIO_DEFINE__H__
#define __GPIO_DEFINE__H__

// Determine if a C++ compiler is being used.  If so, ensure that standard
// C is used to process the API information.
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    I43_GPIO_CONFIG_OUT             = 0x00,
    I43_GPIO_CONFIG_IN              = 0x80
} I43_GPIO_CONFIG_TYPE;


typedef enum {
    ST_GPIOA                       = 0x00,
    ST_GPIOB                       = 0x01,
    ST_GPIOC                       = 0x02,
    ST_GPIOD                       = 0x03,
    ST_GPIOE                       = 0x04,
    ST_GPIOF                       = 0x05,
    ST_GPIOG                       = 0x06,
}ST_GPIO_GROUP_TYPE;


#define GPIO_LED_STATUS_GREEN_GROUP             GPIOC
#define GPIO_LED_STATUS_GREEN_BIT               GPIO_Pin_13
#define GPIO_LED_STATUS_GREEN_CLK               RCC_APB2Periph_GPIOC
#define GPIO_LED_STATUS_GREEN_FUNC              I43_GPIO_CONFIG_OUT
#define GPIO_LED_STATUS_GREEN_LOGIC             1

//电机驱动IO定义 
/* 
FRONT_LEFT_F_PIN	PG13	左前前进IO
FRONT_LEFT_B_PIN	PG11	左前后退IO

FRONT_RIGHT_F_PIN	PC11	右前前进IO
FRONT_RIGHT_B_PIN	PD0	  右前后退IO

BEHIND_LEFT_F_PIN	PD6	    左后前进IO
BEHIND_LEFT_B_PIN	PG9	    左后后退IO

右后电机的两个控制IO这里改为两路使能EN1、EN2，高电平有效
BEHIND_RIGHT_F_PIN	PD4	    右电机使能IO
BEHIND_RIGHT_B_PIN	PD2	    左电机使能IO
*/

#define GPIO_L293D_IN1_PORT                     GPIOC
#define GPIO_L293D_IN1_PIN                      GPIO_Pin_11
#define GPIO_L293D_IN1_CLK                      RCC_APB2Periph_GPIOC
#define GPIO_L293D_IN1_FUNC                     1

#define GPIO_L293D_IN2_PORT                     GPIOD
#define GPIO_L293D_IN2_PIN                      GPIO_Pin_0
#define GPIO_L293D_IN2_CLK                      RCC_APB2Periph_GPIOD
#define GPIO_L293D_IN2_FUNC                     1

#define GPIO_L293D_IN3_PORT                     GPIOD
#define GPIO_L293D_IN3_PIN                      GPIO_Pin_6
#define GPIO_L293D_IN3_CLK                      RCC_APB2Periph_GPIOD
#define GPIO_L293D_IN3_FUNC                     1

#define GPIO_L293D_IN4_PORT                     GPIOG
#define GPIO_L293D_IN4_PIN                      GPIO_Pin_9
#define GPIO_L293D_IN4_CLK                      RCC_APB2Periph_GPIOG
#define GPIO_L293D_IN4_FUNC                     1

#define GPIO_L293D_ENA_PORT                     GPIOD
#define GPIO_L293D_ENA_PIN                      GPIO_Pin_4
#define GPIO_L293D_ENA_CLK                      RCC_APB2Periph_GPIOD
#define GPIO_L293D_ENA_FUNC                     1

#define GPIO_L293D_ENB_PORT                     GPIOD
#define GPIO_L293D_ENB_PIN                      GPIO_Pin_2
#define GPIO_L293D_ENB_CLK                      RCC_APB2Periph_GPIOD
#define GPIO_L293D_ENB_FUNC                     1

#define GPIO_L293D_CLK_ALL                      GPIO_L293D_IN1_CLK | \
                                                GPIO_L293D_IN2_CLK | \
                                                GPIO_L293D_IN3_CLK | \
                                                GPIO_L293D_IN4_CLK

void initializeGPIO(void);

#ifdef __cplusplus
}
#endif

#endif // end of __GPIO_DEFINE__H__

