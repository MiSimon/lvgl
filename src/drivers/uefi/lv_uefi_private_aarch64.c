/*********************
 *      INCLUDES
 *********************/

#include "lv_uefi_private.h"
#if LV_USE_UEFI
#if defined(__LV_UEFI_ARCH_AARCH64__)

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

#if defined(__LV_UEFI_ARCH_X64__) || defined(__LV_UEFI_ARCH_X86__)
    #if !defined(__clang__) && defined(_MSC_VER)
        __int64 _ReadStatusReg(int);
        #pragma intrinsic(_ReadStatusReg)
    #endif
#endif

static uint64_t lv_uefi_cpu_read_cntvct_el0();
static uint64_t lv_uefi_cpu_read_cntfrq_el0();

/**********************
 *  GOLBAL VARIABLES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief Check if there is a cpu counter available that can be used as time source
 * @return True if such a counter exists, false if not
*/
bool lv_uefi_cpu_timer_is_supported()
{
    return TRUE;
}

/**
 * @brief Get the current value of the cpu counter
 * @return The current value
*/
uint64_t lv_uefi_cpu_counter_get_value()
{
    return lv_uefi_cpu_read_cntvct_el0();
}

/**
 * @brief Get the frequency in Hz
 * @return The frequency in Hz
*/
uint64_t lv_uefi_cpu_timer_get_frequency()
{
    return lv_uefi_cpu_read_cntfrq_el0();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#if defined(__clang__) || defined(__GNUC__)
static uint64_t lv_uefi_cpu_read_cntvct_el0()
{
    uint64_t cntvct_el0;
    __asm__ __volatile__("mrs %0, cntvct_el0" : "=r"(cntvct_el0));
    return cntvct_el0;
}

static uint64_t lv_uefi_cpu_read_cntfrq_el0()
{
    uint64_t cntvtel0;
    __asm__ __volatile__("mrs %0, cntfrq_el0" : "=r"(cntvtel0));
    return cntvtel0;
}

#else
static uint64_t lv_uefi_cpu_read_cntvct_el0()
{
    return _ReadStatusReg(0x5F10);
}

static uint64_t lv_uefi_cpu_read_cntfrq_el0()
{
    return _ReadStatusReg(0x5F00);
}
#endif

#endif
#endif