/*********************
 *      INCLUDES
 *********************/

#include "lv_uefi_private.h"
#if LV_USE_UEFI
#if defined(__LV_UEFI_ARCH_X64__) || defined(__LV_UEFI_ARCH_X86__)

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
        unsigned __int64 __rdtsc();
        void __cpuid(int cpuInfo[4], int function_id);
        #pragma intrinsic(__rdtsc)
        #pragma intrinsic(__cpuid)
    #endif
#endif

static void lv_uefi_cpu_cpuid(uint32_t function, uint32_t * eax, uint32_t * ebx, uint32_t * ecx, uint32_t * edx);
static uint64_t lv_uefi_cpu_rdtsc();

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
    uint32_t eax, ebx, ecx, edx;
    lv_uefi_cpu_cpuid(0x80000000, &eax, &ebx, &ecx, &edx);
    if(eax < 0x80000007) {
        return FALSE;
    }
    // Check if rdtsc is available CPUID Fn0000_0001_EDX[TSC] = 1
    lv_uefi_cpu_cpuid(0x00000001, &eax, &ebx, &ecx, &edx);
    if((edx & (1 << 4)) == 0) {
        return FALSE;
    }
    // Check if rdtsc is invariant CPUID Fn8000_0007_EDX[TscInvariant] = 1
    lv_uefi_cpu_cpuid(0x80000007, &eax, &ebx, &ecx, &edx);
    if((edx & (1 << 8)) == 0) {
        return FALSE;
    }

    return TRUE;
}

/**
 * @brief Get the current value of the cpu counter
 * @return The current value
*/
uint64_t lv_uefi_cpu_counter_get_value()
{
    return lv_uefi_cpu_rdtsc();
}

/**
 * @brief Get the frequency in Hz
 * @return The frequency in Hz
*/
uint64_t lv_uefi_cpu_timer_get_frequency()
{
    EFI_TPL old_tpl;
    uint64_t start_value;
    uint64_t end_value;

    old_tpl = gLvEfiBS->RaiseTPL(TPL_NOTIFY);
    start_value = lv_uefi_cpu_rdtsc();
    gLvEfiBS->Stall(1000);
    end_value = lv_uefi_cpu_rdtsc();
    gLvEfiBS->RestoreTPL(old_tpl);

    return (end_value - start_value) * 1000;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#if defined(__clang__) || defined(__GNUC__)
static void lv_uefi_cpu_cpuid(uint32_t function, uint32_t * eax, uint32_t * ebx, uint32_t * ecx, uint32_t * edx)
{
    uint32_t _eax, _ebx, _ecx, _edx;
    __asm__("cpuid" : "=a"(_eax), "=b"(_ebx), "=c"(_ecx), "=d"(_edx) : "0"(function));
    if(eax != NULL) *eax = (uint32_t) _eax;
    if(ebx != NULL) *ebx = (uint32_t) _ebx;
    if(ecx != NULL) *ecx = (uint32_t) _ecx;
    if(edx != NULL) *edx = (uint32_t) _edx;
}

static uint64_t lv_uefi_cpu_rdtsc()
{
    uint32_t eax, edx;
    __asm__ __volatile__("rdtsc" : "=a"(eax), "=d"(edx));
    return (uint64_t)((uint64_t)eax) | (((uint64_t)edx) << 32);
}
#else
static void lv_uefi_cpu_cpuid(uint32_t function, uint32_t * eax, uint32_t * ebx, uint32_t * ecx, uint32_t * edx)
{
    int regs[4];
    __cpuid(regs, (int)function);
    if(eax != NULL) *eax = (uint32_t) regs[0];
    if(ebx != NULL) *ebx = (uint32_t) regs[1];
    if(ecx != NULL) *ecx = (uint32_t) regs[2];
    if(edx != NULL) *edx = (uint32_t) regs[3];
}

static uint64_t lv_uefi_cpu_rdtsc()
{
    return __rdtsc();
}
#endif

#endif
#endif