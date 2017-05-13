#ifndef __GPIO_DEFINE__H__
#define __GPIO_DEFINE__H__

// Determine if a C++ compiler is being used.  If so, ensure that standard
// C is used to process the API information.
#ifdef __cplusplus
extern "C" {
#endif

#define GPIO_LED_STATUS_GREEN_GROUP             GPIOB
#define GPIO_LED_STATUS_GREEN_BIT               GPIO_Pin_12
#define GPIO_LED_STATUS_GREEN_CLK               RCC_APB2Periph_GPIOB
#define GPIO_LED_STATUS_GREEN_FUNC              I43_GPIO_CONFIG_OUT
#define GPIO_LED_STATUS_GREEN_LOGIC             1


#ifdef __cplusplus
}
#endif

#endif // end of __GPIO_DEFINE__H__

