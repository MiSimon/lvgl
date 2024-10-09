#ifndef __LV_UEFI_PRIVATE_CPU_H__
#define __LV_UEFI_PRIVATE_CPU_H__

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

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * @brief Check if there is a cpu counter available that can be used as time source
 * @return True if such a counter exists, false if not
*/
bool lv_uefi_cpu_timer_is_supported();

/**
 * @brief Get the current value of the cpu counter
 * @return The current valuea
*/
uint64_t lv_uefi_cpu_counter_get_value();

/**
 * @brief Get the frequency in Hz
 * @return The frequency in Hz
*/
uint64_t lv_uefi_cpu_timer_get_frequency();

/**********************
 *      MACROS
 **********************/

#endif

#ifdef __cplusplus
}
#endif

#endif //__LV_UEFI_PRIVATE_CPU_H__