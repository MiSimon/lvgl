/**
 * @file lv_uefi_private_timer.h
 *
 */

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

typedef enum {
    LV_UEFI_TIME_SOURCE_UNKNOWN = 0,    /**< Timer is not initialized */
    LV_UEFI_TIME_SOURCE_TIMESTAMP_PROTOCOL, /**< The timer source is EFI_TIMESTAMP_PROTOCOL */
    LV_UEFI_TIME_SOURCE_HPET,   /**< The timer source is the HPET */
    LV_UEFI_TIME_SOURCE_ACPI,   /**< The timer source is the ACPI timer */
    LV_UEFI_TIME_SOURCE_CPU,    /**< The timer source is a timer inside the CPU */
} lv_uefi_time_source_t;

typedef struct _lv_uefi_timer_context_t {
    lv_uefi_time_source_t source;   /**< If source == LV_UEFI_TIME_SOURCE_UNKNOWN all other fields are undefined */
    uint64_t tick_value;    /**< The current tick value */
    uint64_t last_value;    /**< The current raw tick value */
    struct {
        uint64_t max_value; /**< Timer source will roll over when it reaches this value */
        uint64_t frequency; /**< Frequency of the timer source in Hz */
    } meta;
    union {
        struct {
            EFI_TIMESTAMP_PROTOCOL * interface;
        } timestamp_protocol;
        struct {
            uint64_t * main_counter_value_register; /**< Address of the HPET Main Counter Value Register */
        } hpet;
        struct {
            uint32_t * timer_address;   /**< Address of the X_PM_TMR_BLK */
        } acpi;
    } source_meta;
} lv_uefi_timer_context_t;

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