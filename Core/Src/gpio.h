#ifndef __GPIO_H
#define __GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "main.h"

typedef enum{
    BUTTON_RECT_PRESS,
    BUTTON_RECT_REL,
    BUTTON_CIRCLE_PRESS,
    BUTTON_CIRCLE_REL,
    BUTTON_UP_PRESS,
    BUTTON_UP_REL,
    BUTTON_DOWN_PRESS,
    BUTTON_DOWN_REL
}buttons_enum;

typedef struct
{
    buttons_enum button;
    uint32_t timestamp;
}button_event_t;


/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
void MX_GPIO_Init(void);

/**
 * @brief Set the Laser Output state
 * 
 * @param state 
 */
void setLaserOutput(GPIO_PinState state);

/**
 * @brief Get the Laser Output state
 * 
 * @return GPIO_PinState 
 */
GPIO_PinState getLaserOutput(void);

/**
 * @brief Toggle the laser
 * 
 */
void toggleLaserOutput(void);

/**
 * @brief Enable the analog switch for the adc reading.
 * 
 */
void enableADCinput(void);



/**
 * @brief Disable the analog switch for the adc reading
 * 
 */
void disableADCinput(void);

#ifdef __cplusplus
}
#endif

#endif /* __GPIO_H */