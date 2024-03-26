#ifndef __LVGL_TESTS_MAKEFILE_UEFI_LV_EFI_TYPES_H__
#define __LVGL_TESTS_MAKEFILE_UEFI_LV_EFI_TYPES_H__

#include "uefi_min.h"

#ifdef __cplusplus
    #define extern "C" {
#endif

/*************************************
 * TYPES
*************************************/
typedef UINT8 uint8_t;
typedef UINT16 uint16_t;
typedef UINT32 uint32_t;
typedef UINT64 uint64_t;
typedef INT8 int8_t;
typedef INT16 int16_t;
typedef INT32 int32_t;
typedef INT64 int64_t;
typedef void VOID;

typedef uint32_t uint_fast32_t;
typedef UINTN uintptr_t;
typedef UINTN size_t;
typedef INTN intptr_t;
typedef INTMAX intmax_t;
typedef INTN ptrdiff_t;

typedef UINT8 bool;

/*************************************
 * DEFINES
*************************************/
#define false 0
#define true 1
#define NULL ((void*)0)

#define PRId8       "d"
#define PRId16      "d"
#define PRId32      "d"
#define PRId64      "d"

#define PRIu8       "u"
#define PRIu16      "u"
#define PRIu32      "u"
#define PRIu64      "u"

#define PRIx8       "x"
#define PRIx16      "x"
#define PRIx32      "x"
#define PRIx64      "x"

#define PRIX8       "X"
#define PRIX16      "X"
#define PRIX32      "X"
#define PRIX64      "X"

#define offsetof(TYPE, Field)     ((UINTN) __builtin_offsetof(TYPE, Field))

/*************************************
 * VA_ARG
*************************************/
typedef __builtin_va_list va_list;
#define va_start(Marker, Parameter)  __builtin_va_start (Marker, Parameter)
#define va_arg(Marker, TYPE)  ((sizeof (TYPE) < sizeof (UINTN)) ? (TYPE)(__builtin_va_arg (Marker, UINTN)) : (TYPE)(__builtin_va_arg (Marker, TYPE)))
#define va_end(Marker)  __builtin_va_end (Marker)
#define va_copy(Dest, Start)  __builtin_va_copy (Dest, Start)

#ifdef __cplusplus
}
#endif

#endif