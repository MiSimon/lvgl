#ifndef __LV_UEFI_PRIVATE_TIMER_H__
#define __LV_UEFI_PRIVATE_TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lvgl.h"

#if LV_USE_UEFI

#include "lv_uefi.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct _lv_uefi_timer_state_t {
    uint64_t counter_value;
    uint64_t frequency;
} lv_uefi_timer_state_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * @brief Get the state of the timer
 * @param state Pointer to store the state
 * @return True if the timer has a valid state, false if not
*/
bool lv_uefi_timer_get_state(lv_uefi_timer_state_t * state);

/**
 * @brief Initialize the timer context
*/
void lv_uefi_timer_init();

/**
 * @brief Cleanup the timer context
*/
void lv_uefi_timer_deinit();

/**********************
 *      MACROS
 **********************/

#endif

#ifdef __cplusplus
}
#endif

#endif //__LV_UEFI_PRIVATE_TIMER_H__