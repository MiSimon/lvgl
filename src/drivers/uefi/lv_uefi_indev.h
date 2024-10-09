/**
 * @file lv_uefi_indev.h
 *
 */

#ifndef __LV_UEFI_INDEV_H__
#define __LV_UEFI_INDEV_H__

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../indev/lv_indev.h"

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
 * @brief Create a LVGL indev object.
 * @param handle The handle on which an instance of the EFI_SIMPLE_POINTER_PROTOCOL protocol is installed.
 * @param display_res The resolution of the display in pixels, needed to scale the input.
 * @return The created LVGL indev object.
*/
lv_indev_t * lv_uefi_create_simple_pointer_indev(
    void * handle,
    lv_point_t * display_res);

/**
 * @brief Create a LVGL indev object.
 * @param handle The handle on which an instance of the EFI_ABSOLUTE_POINTER_PROTOCOL protocol is installed.
 * @param display_res The resolution of the display in pixels, needed to scale the input.
 * @return The created LVGL indev object.
*/
lv_indev_t * lv_uefi_create_absolute_pointer_indev(
    void * handle,
    lv_point_t * display_res);

/**
 * @brief Create a LVGL indev object.
 * @param handle The handle on which an instance of the EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL or EFI_SIMPLE_TEXT_INPUT_PROTOCOL protocol is installed.
 * @return The created LVGL indev object.
*/
lv_indev_t * lv_uefi_create_keyboard_indev(
    void * handle);

/**********************
 *      MACROS
 **********************/

#endif

#ifdef __cplusplus
}
#endif

#endif //__LV_UEFI_INDEV_H__