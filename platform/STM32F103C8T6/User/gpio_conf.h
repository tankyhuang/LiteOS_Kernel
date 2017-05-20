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
#define GPIO_LED_STATUS_GREEN_GROUP             sti_GPIOB
#define GPIO_LED_STATUS_GREEN_BIT               12
#define GPIO_LED_STATUS_GREEN_CLK               RCC_APB2Periph_GPIOB
#define GPIO_LED_STATUS_GREEN_FUNC              I43_GPIO_CONFIG_OUT
#define GPIO_LED_STATUS_GREEN_LOGIC             0

void initializeGPIO(void);
    
#ifdef __cplusplus
}
#endif

#endif // end of __GPIO_DEFINE__H__

