#ifndef __UEFI_MIN_H__
#define __UEFI_MIN_H__

#ifndef __clang__
#error This file is only for use with the clang compiler
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*************************************
 * TYPES
*************************************/
#if defined(__x86_64__)
typedef unsigned long long UINT64;
typedef long long INT64;
typedef unsigned int UINT32;
typedef int INT32;
typedef unsigned short UINT16;
typedef unsigned short CHAR16;
typedef short INT16;
typedef unsigned char BOOLEAN;
typedef unsigned char UINT8;
typedef char CHAR8;
typedef signed char INT8;
typedef UINT64 UINTN;
typedef INT64 INTN;
typedef INT64 INTMAX;
#define MAX_BIT (1ull<63)
#elif defined(__i386__)
typedef unsigned long long UINT64;
typedef long long INT64;
typedef unsigned int UINT32;
typedef int INT32;
typedef unsigned short UINT16;
typedef unsigned short CHAR16;
typedef short INT16;
typedef unsigned char BOOLEAN;
typedef unsigned char UINT8;
typedef char CHAR8;
typedef signed char INT8;
typedef UINT32 UINTN;
typedef INT32 INTN;
typedef INT64 INTMAX;
#define MAX_BIT (1ul<31)
#elif defined(__aarch64__)
typedef unsigned long long UINT64;
typedef long long INT64;
typedef unsigned int UINT32;
typedef int INT32;
typedef unsigned short UINT16;
typedef unsigned short CHAR16;
typedef short INT16;
typedef unsigned char BOOLEAN;
typedef unsigned char UINT8;
typedef char CHAR8;
typedef signed char INT8;
typedef UINT64 UINTN;
typedef INT64 INTN;
typedef INT64 INTMAX;
#define MAX_BIT (1ull<63)
#else 
#error Architecture is not supported
#endif

/*************************************
 * LIMITS
*************************************/
#define INT8_MAX    (0x7F)
#define UINT8_MAX   (0xFF)
#define INT16_MAX   (0x7FFF)
#define UINT16_MAX  (0xFFFF)
#define INT32_MAX   (0x7FFFFFFF)
#define UINT32_MAX  (0xFFFFFFFF)
#define INT64_MAX   (0x7FFFFFFFFFFFFFFFULL)
#define UINT64_MAX  (0xFFFFFFFFFFFFFFFFULL)

#if defined(__x86_64__) || defined(__aarch64__)
#define INT_MAX   (0x7FFFFFFFFFFFFFFFULL)
#define UINT_MAX  (0xFFFFFFFFFFFFFFFFULL)
#elif defined(__i386__)
#define INT_MAX   (0x7FFFFFFFUL)
#define UINT_MAX  (0xFFFFFFFFUL)
#endif

#define INT8_MIN   (( -127) - 1)
#define INT16_MIN  (( -32767) - 1)
#define INT32_MIN  (( -2147483647) - 1)
#define INT64_MIN  (( -9223372036854775807LL) - 1)

#define SIZE_MAX (0xFFFFFFFF)
#define LONG_MAX (0x7FFFFFFF)
#define CHAR_BIT 8

/*************************************
 * VERIFICATION
*************************************/
_Static_assert(sizeof(BOOLEAN)  == 1, "Size check for 'BOOLEAN' failed.");
_Static_assert(sizeof(INT8)     == 1, "Size check for 'INT8' failed.");
_Static_assert(sizeof(UINT8)    == 1, "Size check for 'UINT8' failed.");
_Static_assert(sizeof(INT16)    == 2, "Size check for 'INT16' failed.");
_Static_assert(sizeof(UINT16)   == 2, "Size check for 'UINT16' failed.");
_Static_assert(sizeof(INT32)    == 4, "Size check for 'INT32' failed.");
_Static_assert(sizeof(UINT32)   == 4, "Size check for 'UINT32' failed.");
_Static_assert(sizeof(INT64)    == 8, "Size check for 'INT64' failed.");
_Static_assert(sizeof(UINT64)   == 8, "Size check for 'UINT64' failed.");

/*************************************
 * EFI TYPES
*************************************/
typedef UINTN EFI_TPL;
typedef UINTN EFI_STATUS;
typedef void VOID;
typedef VOID* EFI_HANDLE;
typedef VOID* EFI_EVENT;
typedef UINT64 EFI_PHYSICAL_ADDRESS;
typedef UINT64 EFI_VIRTUAL_ADDRESS;

#define IN
#define OUT
#define OPTIONAL
#define EFIAPI
#define CONST const

typedef struct {
    UINT64 Signature;
    UINT32 Revision;
    UINT32 HeaderSize;
    UINT32 CRC32;
    UINT32 Reserved;
} EFI_TABLE_HEADER;

typedef struct {
    UINT32 Data1;
    UINT16 Data2;
    UINT16 Data3;
    UINT8 Data4[8];
} EFI_GUID;

typedef struct _EFI_SYSTEM_TABLE EFI_SYSTEM_TABLE;
typedef struct _EFI_RUNTIME_SERVICES EFI_RUNTIME_SERVICES;
typedef struct _EFI_BOOT_SERVICES EFI_BOOT_SERVICES;
typedef struct _EFI_CONFIGURATION_TABLE EFI_CONFIGURATION_TABLE;

typedef struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL EFI_SIMPLE_TEXT_INPUT_PROTOCOL;
typedef struct _EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL;
typedef struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;
typedef struct _EFI_SIMPLE_POINTER_PROTOCOL EFI_SIMPLE_POINTER_PROTOCOL;
typedef struct _EFI_ABSOLUTE_POINTER_PROTOCOL EFI_ABSOLUTE_POINTER_PROTOCOL;
typedef struct _EFI_GRAPHICS_OUTPUT_PROTOCOL EFI_GRAPHICS_OUTPUT_PROTOCOL;
typedef struct _EFI_EDID_ACTIVE_PROTOCOL EFI_EDID_ACTIVE_PROTOCOL;
typedef struct _EFI_EDID_DISCOVERED_PROTOCOL EFI_EDID_DISCOVERED_PROTOCOL;
typedef struct _EFI_EDID_OVERRIDE_PROTOCOL EFI_EDID_OVERRIDE_PROTOCOL;
typedef struct _EFI_DEVICE_PATH_PROTOCOL EFI_DEVICE_PATH_PROTOCOL;

// EFI_SYSTEM_TABLE

typedef struct _EFI_SYSTEM_TABLE {
    EFI_TABLE_HEADER Hdr;
    CHAR16 *FirmwareVendor;
    UINT32 FirmwareRevision;
    EFI_HANDLE ConsoleInHandle;
    EFI_SIMPLE_TEXT_INPUT_PROTOCOL *ConIn;
    EFI_HANDLE ConsoleOutHandle;
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut;
    EFI_HANDLE StandardErrorHandle;
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *StdErr;
    EFI_RUNTIME_SERVICES *RuntimeServices;
    EFI_BOOT_SERVICES *BootServices;
    UINTN NumberOfTableEntries;
    EFI_CONFIGURATION_TABLE *ConfigurationTable;
} EFI_SYSTEM_TABLE;

// EFI_BOOT_SERVICES

#define EVT_TIMER 0x80000000
#define EVT_RUNTIME 0x40000000
#define EVT_NOTIFY_WAIT 0x00000100
#define EVT_NOTIFY_SIGNAL 0x00000200
#define EVT_SIGNAL_EXIT_BOOT_SERVICES 0x00000201
#define EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE 0x60000202

typedef
VOID
(EFIAPI *EFI_EVENT_NOTIFY) (
    IN EFI_EVENT Event,
    IN VOID *Context
    );

typedef
EFI_STATUS
(EFIAPI *EFI_CREATE_EVENT) (
    IN UINT32 Type,
    IN EFI_TPL NotifyTpl,
    IN EFI_EVENT_NOTIFY NotifyFunction, OPTIONAL
    IN VOID *NotifyContext, OPTIONAL
    OUT EFI_EVENT *Event
    );

#define EFI_EVENT_GROUP_EXIT_BOOT_SERVICES \
    {0x27abf055, 0xb1b8, 0x4c26, { 0x80, 0x48, 0x74, 0x8f, 0x37,\
    0xba, 0xa2, 0xdf}}
#define EFI_EVENT_GROUP_BEFORE_EXIT_BOOT_SERVICES \
    { 0x8be0e274, 0x3970, 0x4b44, { 0x80, 0xc5, 0x1a, 0xb9, 0x50, 0x2f, 0x3b, 0xfc } }
#define EFI_EVENT_GROUP_VIRTUAL_ADDRESS_CHANGE \
    {0x13fa7698, 0xc831, 0x49c7, 0x87, 0xea, 0x8f, 0x43, 0xfc,\
    0xc2, 0x51, 0x96}
#define EFI_EVENT_GROUP_MEMORY_MAP_CHANGE \
    {0x78bee926, 0x692f, 0x48fd, 0x9e, 0xdb, 0x1, 0x42, 0x2e, \
    0xf0, 0xd7, 0xab}
#define EFI_EVENT_GROUP_READY_TO_BOOT \
    {0x7ce88fb3, 0x4bd7, 0x4679, 0x87, 0xa8, 0xa8, 0xd8, 0xde,\
    0xe5,0xd, 0x2b}
#define EFI_EVENT_GROUP_AFTER_READY_TO_BOOT \
    { 0x3a2a00ad, 0x98b9, 0x4cdf, { 0xa4, 0x78, 0x70, 0x27, 0x77, \
    0xf1, 0xc1, 0xb } }
#define EFI_EVENT_GROUP_RESET_SYSTEM \
    { 0x62da6a56, 0x13fb, 0x485a, { 0xa8, 0xda, 0xa3, 0xdd, 0x79, 0x12, 0xcb, 0x6b } }

typedef
EFI_STATUS
(EFIAPI *EFI_CREATE_EVENT_EX) (
    IN UINT32 Type,
    IN EFI_TPL NotifyTpl,
    IN EFI_EVENT_NOTIFY NotifyFunction OPTIONAL,
    IN CONST VOID *NotifyContext OPTIONAL,
    IN CONST EFI_GUID *EventGroup OPTIONAL,
    OUT EFI_EVENT *Event
    );

typedef
EFI_STATUS
(EFIAPI *EFI_CLOSE_EVENT) (
    IN EFI_EVENT Event
    );

typedef
EFI_STATUS
(EFIAPI *EFI_SIGNAL_EVENT) (
    IN EFI_EVENT Event
    );

typedef
EFI_STATUS
(EFIAPI *EFI_WAIT_FOR_EVENT) (
    IN UINTN NumberOfEvents,
    IN EFI_EVENT *Event,
    OUT UINTN *Index
    );

typedef
EFI_STATUS
(EFIAPI *EFI_CHECK_EVENT) (
    IN EFI_EVENT Event
    );

typedef enum {
    TimerCancel,
    TimerPeriodic,
    TimerRelative
} EFI_TIMER_DELAY;

typedef
EFI_STATUS
(EFIAPI *EFI_SET_TIMER) (
    IN EFI_EVENT Event,
    IN EFI_TIMER_DELAY Type,
    IN UINT64 TriggerTime
    );

#define TPL_APPLICATION 4
#define TPL_CALLBACK 8
#define TPL_NOTIFY 16
#define TPL_HIGH_LEVEL 31

typedef
EFI_TPL
(EFIAPI *EFI_RAISE_TPL) (
    IN EFI_TPL NewTpl
    );

typedef
VOID
(EFIAPI *EFI_RESTORE_TPL) (
    IN EFI_TPL OldTpl
    );

typedef enum {
    AllocateAnyPages,
    AllocateMaxAddress,
    AllocateAddress,
    MaxAllocateType
} EFI_ALLOCATE_TYPE;

typedef enum {
    EfiReservedMemoryType,
    EfiLoaderCode,
    EfiLoaderData,
    EfiBootServicesCode,
    EfiBootServicesData,
    EfiRuntimeServicesCode,
    EfiRuntimeServicesData,
    EfiConventionalMemory,
    EfiUnusableMemory,
    EfiACPIReclaimMemory,
    EfiACPIMemoryNVS,
    EfiMemoryMappedIO,
    EfiMemoryMappedIOPortSpace,
    EfiPalCode,
    EfiPersistentMemory,
    EfiUnacceptedMemoryType,
    EfiMaxMemoryType
} EFI_MEMORY_TYPE;

typedef
EFI_STATUS
(EFIAPI *EFI_ALLOCATE_PAGES) (
    IN EFI_ALLOCATE_TYPE Type,
    IN EFI_MEMORY_TYPE MemoryType,
    IN UINTN Pages,
    IN OUT EFI_PHYSICAL_ADDRESS *Memory
    );

typedef
EFI_STATUS
(EFIAPI *EFI_FREE_PAGES) (
    IN EFI_PHYSICAL_ADDRESS Memory,
    IN UINTN Pages
    );

typedef struct {
    UINT32 Type;
    EFI_PHYSICAL_ADDRESS PhysicalStart;
    EFI_VIRTUAL_ADDRESS VirtualStart;
    UINT64 NumberOfPages;
    UINT64 Attribute;
} EFI_MEMORY_DESCRIPTOR;

#define EFI_MEMORY_UC 0x0000000000000001
#define EFI_MEMORY_WC 0x0000000000000002
#define EFI_MEMORY_WT 0x0000000000000004
#define EFI_MEMORY_WB 0x0000000000000008
#define EFI_MEMORY_UCE 0x0000000000000010
#define EFI_MEMORY_WP 0x0000000000001000
#define EFI_MEMORY_RP 0x0000000000002000
#define EFI_MEMORY_XP 0x0000000000004000
#define EFI_MEMORY_NV 0x0000000000008000
#define EFI_MEMORY_MORE_RELIABLE 0x0000000000010000
#define EFI_MEMORY_RO 0x0000000000020000
#define EFI_MEMORY_SP 0x0000000000040000
#define EFI_MEMORY_CPU_CRYPTO 0x0000000000080000
#define EFI_MEMORY_RUNTIME 0x8000000000000000
#define EFI_MEMORY_ISA_VALID 0x4000000000000000
#define EFI_MEMORY_ISA_MASK 0x0FFFF00000000000

#define EFI_MEMORY_DESCRIPTOR_VERSION 1

typedef
EFI_STATUS
(EFIAPI *EFI_GET_MEMORY_MAP) (
    IN OUT UINTN *MemoryMapSize,
    OUT EFI_MEMORY_DESCRIPTOR *MemoryMap,
    OUT UINTN *MapKey,
    OUT UINTN *DescriptorSize,
    OUT UINT32 *DescriptorVersion
    );

typedef
EFI_STATUS
(EFIAPI *EFI_ALLOCATE_POOL) (
    IN EFI_MEMORY_TYPE PoolType,
    IN UINTN Size,
    OUT VOID **Buffer
    );

typedef
EFI_STATUS
(EFIAPI *EFI_FREE_POOL) (
    IN VOID *Buffer
    );

typedef enum {
    EFI_NATIVE_INTERFACE
} EFI_INTERFACE_TYPE;

typedef
EFI_STATUS
(EFIAPI *EFI_INSTALL_PROTOCOL_INTERFACE) (
    IN OUT EFI_HANDLE *Handle,
    IN EFI_GUID *Protocol,
    IN EFI_INTERFACE_TYPE InterfaceType,
    IN VOID *Interface
    );

typedef
EFI_STATUS
(EFIAPI *EFI_UNINSTALL_PROTOCOL_INTERFACE) (
    IN EFI_HANDLE Handle,
    IN EFI_GUID *Protocol,
    IN VOID *Interface
    );

typedef
EFI_STATUS
(EFIAPI *EFI_REINSTALL_PROTOCOL_INTERFACE) (
    IN EFI_HANDLE Handle,
    IN EFI_GUID *Protocol,
    IN VOID *OldInterface,
    IN VOID *NewInterface
    );

typedef
EFI_STATUS
(EFIAPI *EFI_REGISTER_PROTOCOL_NOTIFY) (
    IN EFI_GUID *Protocol,
    IN EFI_EVENT Event,
    OUT VOID **Registration
    );

typedef enum {
    AllHandles,
    ByRegisterNotify,
    ByProtocol
} EFI_LOCATE_SEARCH_TYPE;

typedef
EFI_STATUS
(EFIAPI *EFI_LOCATE_HANDLE) (
    IN EFI_LOCATE_SEARCH_TYPE SearchType,
    IN EFI_GUID *Protocol OPTIONAL,
    IN VOID *SearchKey OPTIONAL,
    IN OUT UINTN *BufferSize,
    OUT EFI_HANDLE *Buffer
    );

typedef
EFI_STATUS
(EFIAPI *EFI_HANDLE_PROTOCOL) (
    IN EFI_HANDLE Handle,
    IN EFI_GUID *Protocol,
    OUT VOID **Interface
    );

typedef
EFI_STATUS
(EFIAPI *EFI_LOCATE_DEVICE_PATH) (
    IN EFI_GUID *Protocol,
    IN OUT EFI_DEVICE_PATH_PROTOCOL **DevicePath,
    OUT EFI_HANDLE *Device
    );

#define EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL 0x00000001
#define EFI_OPEN_PROTOCOL_GET_PROTOCOL 0x00000002
#define EFI_OPEN_PROTOCOL_TEST_PROTOCOL 0x00000004
#define EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER 0x00000008
#define EFI_OPEN_PROTOCOL_BY_DRIVER 0x00000010
#define EFI_OPEN_PROTOCOL_EXCLUSIVE 0x00000020

typedef
EFI_STATUS
(EFIAPI *EFI_OPEN_PROTOCOL) (
    IN EFI_HANDLE Handle,
    IN EFI_GUID *Protocol,
    OUT VOID **Interface OPTIONAL,
    IN EFI_HANDLE AgentHandle,
    IN EFI_HANDLE ControllerHandle,
    IN UINT32 Attributes
    );

typedef
EFI_STATUS
(EFIAPI *EFI_CLOSE_PROTOCOL) (
    IN EFI_HANDLE Handle,
    IN EFI_GUID *Protocol,
    IN EFI_HANDLE AgentHandle,
    IN EFI_HANDLE ControllerHandle
    );

typedef struct {
    EFI_HANDLE AgentHandle;
    EFI_HANDLE ControllerHandle;
    UINT32 Attributes;
    UINT32 OpenCount;
} EFI_OPEN_PROTOCOL_INFORMATION_ENTRY;

typedef
EFI_STATUS
(EFIAPI *EFI_OPEN_PROTOCOL_INFORMATION) (
    IN EFI_HANDLE Handle,
    IN EFI_GUID *Protocol,
    OUT EFI_OPEN_PROTOCOL_INFORMATION_ENTRY **EntryBuffer,
    OUT UINTN *EntryCount
    );

typedef
EFI_STATUS
(EFIAPI *EFI_CONNECT_CONTROLLER) (
    IN EFI_HANDLE ControllerHandle,
    IN EFI_HANDLE *DriverImageHandle OPTIONAL,
    IN EFI_DEVICE_PATH_PROTOCOL *RemainingDevicePath OPTIONAL,
    IN BOOLEAN Recursive
    );

typedef
EFI_STATUS
(EFIAPI *EFI_DISCONNECT_CONTROLLER) (
    IN EFI_HANDLE ControllerHandle,
    IN EFI_HANDLE DriverImageHandle OPTIONAL,
    IN EFI_HANDLE ChildHandle OPTIONAL
    );

typedef
EFI_STATUS
(EFIAPI *EFI_PROTOCOLS_PER_HANDLE) (
    IN EFI_HANDLE Handle,
    OUT EFI_GUID ***ProtocolBuffer,
    OUT UINTN *ProtocolBufferCount
    );

typedef
EFI_STATUS
(EFIAPI *EFI_LOCATE_HANDLE_BUFFER) (
    IN EFI_LOCATE_SEARCH_TYPE SearchType,
    IN EFI_GUID *Protocol OPTIONAL,
    IN VOID *SearchKey OPTIONAL,
    OUT UINTN *NoHandles,
    OUT EFI_HANDLE **Buffer
    );

typedef
EFI_STATUS
(EFIAPI *EFI_LOCATE_PROTOCOL) (
    IN EFI_GUID *Protocol,
    IN VOID *Registration OPTIONAL,
    OUT VOID **Interface
    );

typedef
EFI_STATUS
(EFIAPI *EFI_INSTALL_MULTIPLE_PROTOCOL_INTERFACES) (
    IN OUT EFI_HANDLE *Handle,
    ...
    );

typedef
EFI_STATUS
(EFIAPI *EFI_UNINSTALL_MULTIPLE_PROTOCOL_INTERFACES) (
    IN EFI_HANDLE Handle,
    ...
    );

typedef
EFI_STATUS
(EFIAPI *EFI_IMAGE_ENTRY_POINT) (
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
    );

typedef
EFI_STATUS
(EFIAPI *EFI_IMAGE_LOAD) (
    IN BOOLEAN BootPolicy,
    IN EFI_HANDLE ParentImageHandle,
    IN EFI_DEVICE_PATH_PROTOCOL *DevicePath OPTIONAL,
    IN VOID *SourceBuffer OPTIONAL,
    IN UINTN SourceSize,
    OUT EFI_HANDLE *ImageHandle
    );

typedef
EFI_STATUS
(EFIAPI *EFI_IMAGE_START) (
    IN EFI_HANDLE ImageHandle,
    OUT UINTN *ExitDataSize,
    OUT CHAR16 **ExitData OPTIONAL
    );

typedef
EFI_STATUS
(EFIAPI *EFI_IMAGE_UNLOAD) (
    IN EFI_HANDLE ImageHandle
    );

typedef
EFI_STATUS
(EFIAPI *EFI_EXIT) (
    IN EFI_HANDLE ImageHandle,
    IN EFI_STATUS ExitStatus,
    IN UINTN ExitDataSize,
    IN CHAR16 *ExitData OPTIONAL
    );

typedef
EFI_STATUS
(EFIAPI *EFI_EXIT_BOOT_SERVICES) (
    IN EFI_HANDLE ImageHandle,
    IN UINTN MapKey
    );

typedef
EFI_STATUS
(EFIAPI *EFI_SET_WATCHDOG_TIMER) (
    IN UINTN Timeout,
    IN UINT64 WatchdogCode,
    IN UINTN DataSize,
    IN CHAR16 *WatchdogData OPTIONAL
    );

typedef
EFI_STATUS
(EFIAPI *EFI_STALL) (
    IN UINTN Microseconds
    );

typedef
VOID
(EFIAPI *EFI_COPY_MEM) (
    IN VOID *Destination,
    IN VOID *Source,
    IN UINTN Length
    );

typedef
VOID
(EFIAPI *EFI_SET_MEM) (
    IN VOID *Buffer,
    IN UINTN Size,
    IN UINT8 Value
    );

typedef
EFI_STATUS
(EFIAPI *EFI_GET_NEXT_MONOTONIC_COUNT) (
    OUT UINT64 *Count
    );

typedef
EFI_STATUS
(EFIAPI *EFI_INSTALL_CONFIGURATION_TABLE) (
    IN EFI_GUID *Guid,
    IN VOID *Table
    );

typedef
EFI_STATUS
(EFIAPI *EFI_CALCULATE_CRC32) (
    IN VOID *Data,
    IN UINTN DataSize,
    OUT UINT32 *Crc32
    );

typedef struct _EFI_BOOT_SERVICES {
    EFI_TABLE_HEADER Hdr;
    EFI_RAISE_TPL RaiseTPL;
    EFI_RESTORE_TPL RestoreTPL;
    EFI_ALLOCATE_PAGES AllocatePages;
    EFI_FREE_PAGES FreePages;
    EFI_GET_MEMORY_MAP GetMemoryMap;
    EFI_ALLOCATE_POOL AllocatePool;
    EFI_FREE_POOL FreePool;
    EFI_CREATE_EVENT CreateEvent;
    EFI_SET_TIMER SetTimer;
    EFI_WAIT_FOR_EVENT WaitForEvent;
    EFI_SIGNAL_EVENT SignalEvent;
    EFI_CLOSE_EVENT CloseEvent;
    EFI_CHECK_EVENT CheckEvent;
    EFI_INSTALL_PROTOCOL_INTERFACE InstallProtocolInterface;
    EFI_REINSTALL_PROTOCOL_INTERFACE ReinstallProtocolInterface;
    EFI_UNINSTALL_PROTOCOL_INTERFACE UninstallProtocolInterface;
    EFI_HANDLE_PROTOCOL HandleProtocol;
    VOID* Reserved;
    EFI_REGISTER_PROTOCOL_NOTIFY RegisterProtocolNotify;
    EFI_LOCATE_HANDLE LocateHandle;
    EFI_LOCATE_DEVICE_PATH LocateDevicePath;
    EFI_INSTALL_CONFIGURATION_TABLE InstallConfigurationTable;
    EFI_IMAGE_UNLOAD LoadImage;
    EFI_IMAGE_START StartImage;
    EFI_EXIT Exit;
    EFI_IMAGE_UNLOAD UnloadImage;
    EFI_EXIT_BOOT_SERVICES ExitBootServices;
    EFI_GET_NEXT_MONOTONIC_COUNT GetNextMonotonicCount;
    EFI_STALL Stall;
    EFI_SET_WATCHDOG_TIMER SetWatchdogTimer;
    EFI_CONNECT_CONTROLLER ConnectController;
    EFI_DISCONNECT_CONTROLLER DisconnectController;
    EFI_OPEN_PROTOCOL OpenProtocol;
    EFI_CLOSE_PROTOCOL CloseProtocol;
    EFI_OPEN_PROTOCOL_INFORMATION OpenProtocolInformation;
    EFI_PROTOCOLS_PER_HANDLE ProtocolsPerHandle;
    EFI_LOCATE_HANDLE_BUFFER LocateHandleBuffer;
    EFI_LOCATE_PROTOCOL LocateProtocol;
    EFI_UNINSTALL_MULTIPLE_PROTOCOL_INTERFACES InstallMultipleProtocolInterfaces;
    EFI_UNINSTALL_MULTIPLE_PROTOCOL_INTERFACES UninstallMultipleProtocolInterfaces;
    EFI_CALCULATE_CRC32 CalculateCrc32;
    EFI_COPY_MEM CopyMem;
    EFI_SET_MEM SetMem;
    EFI_CREATE_EVENT_EX CreateEventEx;
} EFI_BOOT_SERVICES;

// EFI_RUNTIME_SERVICES

#define EFI_VARIABLE_NON_VOLATILE 0x00000001
#define EFI_VARIABLE_BOOTSERVICE_ACCESS 0x00000002
#define EFI_VARIABLE_RUNTIME_ACCESS 0x00000004
#define EFI_VARIABLE_HARDWARE_ERROR_RECORD 0x00000008
#define EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS 0x00000010
#define EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS 0x00000020
#define EFI_VARIABLE_APPEND_WRITE 0x00000040
#define EFI_VARIABLE_ENHANCED_AUTHENTICATED_ACCESS 0x00000080

#define EFI_VARIABLE_AUTHENTICATION_3_CERT_ID_SHA256 1
typedef struct {
    UINT8 Type;
    UINT32 IdSize;
    // UINT8 Id[IdSize];
} EFI_VARIABLE_AUTHENTICATION_3_CERT_ID;

typedef
EFI_STATUS
(EFIAPI *EFI_GET_VARIABLE) (
    IN CHAR16 *VariableName,
    IN EFI_GUID *VendorGuid,
    OUT UINT32 *Attributes OPTIONAL,
    IN OUT UINTN *DataSize,
    OUT VOID *Data OPTIONAL
    );

typedef
EFI_STATUS
(EFIAPI *EFI_GET_NEXT_VARIABLE_NAME) (
    IN OUT UINTN *VariableNameSize,
    IN OUT CHAR16 *VariableName,
    IN OUT EFI_GUID *VendorGuid
    );

typedef struct _WIN_CERTIFICATE {
    UINT32 dwLength;
    UINT16 wRevision;
    UINT16 wCertificateType;
    //UINT8 bCertificate[ANYSIZE_ARRAY];
} WIN_CERTIFICATE;

#define ANYSIZE_ARRAY 256

typedef struct _WIN_CERTIFICATE_UEFI_GUID {
    WIN_CERTIFICATE Hdr;
    EFI_GUID CertType;
    UINT8 CertData[ANYSIZE_ARRAY];
} WIN_CERTIFICATE_UEFI_GUID;

typedef struct {
    UINT64 MonotonicCount;
    WIN_CERTIFICATE_UEFI_GUID AuthInfo;
} EFI_VARIABLE_AUTHENTICATION;

typedef struct {
    UINT16 Year; // 1900 - 9999
    UINT8 Month; // 1 - 12
    UINT8 Day; // 1 - 31
    UINT8 Hour; // 0 - 23
    UINT8 Minute; // 0 - 59
    UINT8 Second; // 0 - 59
    UINT8 Pad1;
    UINT32 Nanosecond; // 0 - 999,999,999
    INT16 TimeZone; // --1440 to 1440 or 2047
    UINT8 Daylight;
    UINT8 Pad2;
} EFI_TIME;

typedef struct {
    EFI_TIME TimeStamp;
    WIN_CERTIFICATE_UEFI_GUID AuthInfo;
} EFI_VARIABLE_AUTHENTICATION_2;

#define EFI_VARIABLE_AUTHENTICATION_3_TIMESTAMP_TYPE 1
#define EFI_VARIABLE_AUTHENTICATION_3_NONCE_TYPE 2
typedef struct {
    UINT8 Version;
    UINT8 Type;
    UINT32 MetadataSize;
    UINT32 Flags;
} EFI_VARIABLE_AUTHENTICATION_3;

typedef struct {
    UINT32 NonceSize;
    // UINT8 Nonce[NonceSize];
} EFI_VARIABLE_AUTHENTICATION_3_NONCE;

typedef
EFI_STATUS
(EFIAPI *EFI_SET_VARIABLE) (
    IN CHAR16 *VariableName,
    IN EFI_GUID *VendorGuid,
    IN UINT32 Attributes,
    IN UINTN DataSize,
    IN VOID *Data
    );

typedef
EFI_STATUS
(EFIAPI *EFI_QUERY_VARIABLE_INFO) (
    IN UINT32 Attributes,
    OUT UINT64 *MaximumVariableStorageSize,
    OUT UINT64 *RemainingVariableStorageSize,
    OUT UINT64 *MaximumVariableSize
    );

typedef struct {
    UINT32 Resolution;
    UINT32 Accuracy;
    BOOLEAN SetsToZero;
} EFI_TIME_CAPABILITIES;

#define EFI_TIME_ADJUST_DAYLIGHT 0x01
#define EFI_TIME_IN_DAYLIGHT 0x02
#define EFI_UNSPECIFIED_TIMEZONE 0x07FF

typedef
EFI_STATUS
(EFIAPI *EFI_GET_TIME) (
    OUT EFI_TIME *Time,
    OUT EFI_TIME_CAPABILITIES *Capabilities OPTIONAL
    );

typedef
EFI_STATUS
(EFIAPI *EFI_SET_TIME) (
    IN EFI_TIME *Time
    );

typedef
EFI_STATUS
(EFIAPI *EFI_GET_WAKEUP_TIME) (
    OUT BOOLEAN *Enabled,
    OUT BOOLEAN *Pending,
    OUT EFI_TIME *Time
    );

typedef
EFI_STATUS
(EFIAPI *EFI_SET_WAKEUP_TIME) (
    IN BOOLEAN Enable,
    IN EFI_TIME *Time OPTIONAL
    );

typedef
EFI_STATUS
(EFIAPI *EFI_SET_VIRTUAL_ADDRESS_MAP) (
    IN UINTN MemoryMapSize,
    IN UINTN DescriptorSize,
    IN UINT32 DescriptorVersion,
    IN EFI_MEMORY_DESCRIPTOR *VirtualMap
    );

#define EFI_OPTIONAL_PTR 0x00000001

typedef
EFI_STATUS
(EFIAPI *EFI_CONVERT_POINTER) (
    IN UINTN DebugDisposition,
    IN VOID **Address
    );

typedef enum {
    EfiResetCold,
    EfiResetWarm,
    EfiResetShutdown,
    EfiResetPlatformSpecific
} EFI_RESET_TYPE;

typedef
VOID
(EFIAPI *EFI_RESET_SYSTEM) (
    IN EFI_RESET_TYPE ResetType,
    IN EFI_STATUS ResetStatus,
    IN UINTN DataSize,
    IN VOID *ResetData OPTIONAL
    );

typedef
EFI_STATUS
(EFIAPI *EFI_GET_NEXT_HIGH_MONO_COUNT) (
    OUT UINT32 *HighCount
    );

typedef struct {
    UINT64 Length;
    union {
        EFI_PHYSICAL_ADDRESS DataBlock;
        EFI_PHYSICAL_ADDRESS ContinuationPointer;
    }Union;
} EFI_CAPSULE_BLOCK_DESCRIPTOR;

typedef struct {
    EFI_GUID CapsuleGuid;
    UINT32 HeaderSize;
    UINT32 Flags;
    UINT32 CapsuleImageSize;
} EFI_CAPSULE_HEADER;

#define CAPSULE_FLAGS_PERSIST_ACROSS_RESET 0x00010000
#define CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE 0x00020000
#define CAPSULE_FLAGS_INITIATE_RESET 0x00040000

typedef
EFI_STATUS
(EFIAPI *EFI_UPDATE_CAPSULE) (
    IN EFI_CAPSULE_HEADER **CapsuleHeaderArray,
    IN UINTN CapsuleCount,
    IN EFI_PHYSICAL_ADDRESS ScatterGatherList OPTIONAL
    );

typedef struct {
    UINT32 CapsuleArrayNumber;
    VOID* CapsulePtr[1];
} EFI_CAPSULE_TABLE;

typedef
EFI_STATUS
(EFIAPI *EFI_QUERY_CAPSULE_CAPABILITIES) (
    IN EFI_CAPSULE_HEADER **CapsuleHeaderArray,
    IN UINTN CapsuleCount,
    OUT UINT64 *MaximumCapsuleSize,
    OUT EFI_RESET_TYPE *ResetType
    );

typedef struct _EFI_RUNTIME_SERVICES {
    EFI_TABLE_HEADER Hdr;
    EFI_GET_TIME GetTime;
    EFI_SET_TIME SetTime;
    EFI_GET_WAKEUP_TIME GetWakeupTime;
    EFI_SET_WAKEUP_TIME SetWakeupTime;
    EFI_SET_VIRTUAL_ADDRESS_MAP SetVirtualAddressMap;
    EFI_CONVERT_POINTER ConvertPointer;
    EFI_GET_VARIABLE GetVariable;
    EFI_GET_NEXT_VARIABLE_NAME GetNextVariableName;
    EFI_SET_VARIABLE SetVariable;
    EFI_GET_NEXT_HIGH_MONO_COUNT GetNextHighMonotonicCount;
    EFI_RESET_SYSTEM ResetSystem;
    EFI_UPDATE_CAPSULE UpdateCapsule;
    EFI_QUERY_CAPSULE_CAPABILITIES QueryCapsuleCapabilities;
    EFI_QUERY_VARIABLE_INFO QueryVariableInfo;
} EFI_RUNTIME_SERVICES;

// EFI_CONFIGURATION_TABLE

typedef struct _EFI_CONFIGURATION_TABLE {
    EFI_GUID VendorGuid;
    VOID *VendorTable;
} EFI_CONFIGURATION_TABLE;

// EFI_LOADED_IMAGE_PROTOCOL

typedef
EFI_STATUS
(EFIAPI *EFI_IMAGE_UNLOAD) (
    IN EFI_HANDLE ImageHandle
    );

typedef struct {
    UINT32 Revision;
    EFI_HANDLE ParentHandle;
    EFI_SYSTEM_TABLE *SystemTable;
    EFI_HANDLE DeviceHandle;
    EFI_DEVICE_PATH_PROTOCOL *FilePath;
    VOID *Reserved;
    UINT32 LoadOptionsSize;
    VOID *LoadOptions;
    VOID *ImageBase;
    UINT64 ImageSize;
    EFI_MEMORY_TYPE ImageCodeType;
    EFI_MEMORY_TYPE ImageDataType;
    EFI_IMAGE_UNLOAD Unload;
} EFI_LOADED_IMAGE_PROTOCOL;

// EFI_DEVICE_PATH_PROTOCOL

typedef struct _EFI_DEVICE_PATH_PROTOCOL {
    UINT8 Type;
    UINT8 SubType;
    UINT8 Length[2];
} EFI_DEVICE_PATH_PROTOCOL;

// EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL

typedef
EFI_STATUS
(EFIAPI *EFI_INPUT_RESET_EX) (
    IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
    IN BOOLEAN ExtendedVerification
);
typedef struct {
    UINT16 ScanCode;
    CHAR16 UnicodeChar;
} EFI_INPUT_KEY;

typedef UINT8 EFI_KEY_TOGGLE_STATE;

typedef struct _EFI_KEY_STATE {
    UINT32 KeyShiftState;
    EFI_KEY_TOGGLE_STATE KeyToggleState;
} EFI_KEY_STATE;

#define EFI_SHIFT_STATE_VALID 0x80000000
#define EFI_RIGHT_SHIFT_PRESSED 0x00000001
#define EFI_LEFT_SHIFT_PRESSED 0x00000002
#define EFI_RIGHT_CONTROL_PRESSED 0x00000004
#define EFI_LEFT_CONTROL_PRESSED 0x00000008
#define EFI_RIGHT_ALT_PRESSED 0x00000010
#define EFI_LEFT_ALT_PRESSED 0x00000020
#define EFI_RIGHT_LOGO_PRESSED 0x00000040
#define EFI_LEFT_LOGO_PRESSED 0x00000080
#define EFI_MENU_KEY_PRESSED 0x00000100
#define EFI_SYS_REQ_PRESSED 0x00000200
#define EFI_TOGGLE_STATE_VALID 0x80
#define EFI_KEY_STATE_EXPOSED 0x40
#define EFI_SCROLL_LOCK_ACTIVE 0x01
#define EFI_NUM_LOCK_ACTIVE 0x02
#define EFI_CAPS_LOCK_ACTIVE 0x04

typedef struct {
EFI_INPUT_KEY Key;
EFI_KEY_STATE KeyState;
} EFI_KEY_DATA;

typedef
EFI_STATUS
(EFIAPI *EFI_INPUT_READ_KEY_EX) (
    IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
    OUT EFI_KEY_DATA *KeyData
);

typedef
EFI_STATUS
(EFIAPI *EFI_SET_STATE) (
    IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
    IN EFI_KEY_TOGGLE_STATE *KeyToggleState
    );

typedef
EFI_STATUS
(EFIAPI *EFI_KEY_NOTIFY_FUNCTION) (
    IN EFI_KEY_DATA *KeyData
    );

typedef
EFI_STATUS
(EFIAPI *EFI_REGISTER_KEYSTROKE_NOTIFY) (
    IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
    IN EFI_KEY_DATA *KeyData,
    IN EFI_KEY_NOTIFY_FUNCTION KeyNotificationFunction,
    OUT VOID **NotifyHandle
);

typedef
EFI_STATUS
(EFIAPI *EFI_UNREGISTER_KEYSTROKE_NOTIFY) (
    IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
    IN VOID *NotificationHandle
    );

#define EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL_GUID \
    {0xdd9e7534, 0x7762, 0x4698, \
    {0x8c, 0x14, 0xf5, 0x85, 0x17, 0xa6, 0x25, 0xaa}}

typedef struct _EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL {
    EFI_INPUT_RESET_EX Reset;
    EFI_INPUT_READ_KEY_EX ReadKeyStrokeEx;
    EFI_EVENT WaitForKeyEx;
    EFI_SET_STATE SetState;
    EFI_REGISTER_KEYSTROKE_NOTIFY RegisterKeyNotify;
    EFI_UNREGISTER_KEYSTROKE_NOTIFY UnregisterKeyNotify;
} EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL;

// EFI_SIMPLE_TEXT_INPUT_PROTOCOL

typedef
EFI_STATUS
(EFIAPI *EFI_INPUT_RESET) (
    IN EFI_SIMPLE_TEXT_INPUT_PROTOCOL *This,
    IN BOOLEAN ExtendedVerification
    );

typedef
EFI_STATUS
(EFIAPI *EFI_INPUT_READ_KEY) (
    IN EFI_SIMPLE_TEXT_INPUT_PROTOCOL *This,
    OUT EFI_INPUT_KEY *Key
    );

#define EFI_SIMPLE_TEXT_INPUT_PROTOCOL_GUID \
    {0x387477c1,0x69c7,0x11d2,\
    {0x8e,0x39,0x00,0xa0,0xc9,0x69,0x72,0x3b}}

typedef struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL {
    EFI_INPUT_RESET Reset;
    EFI_INPUT_READ_KEY ReadKeyStroke;
    EFI_EVENT WaitForKey;
} EFI_SIMPLE_TEXT_INPUT_PROTOCOL;

// EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_RESET) (
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
    IN BOOLEAN ExtendedVerification
    );
    
typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_STRING) (
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
    IN CHAR16 *String
    );

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_TEST_STRING) (
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
    IN CHAR16 *String
    );

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_QUERY_MODE) (
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
    IN UINTN ModeNumber,
    OUT UINTN *Columns,
    OUT UINTN *Rows
    );

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_SET_MODE) (
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
    IN UINTN ModeNumber
    );

#define EFI_BLACK 0x00
#define EFI_BLUE 0x01
#define EFI_GREEN 0x02
#define EFI_CYAN 0x03
#define EFI_RED 0x04
#define EFI_MAGENTA 0x05
#define EFI_BROWN 0x06
#define EFI_LIGHTGRAY 0x07
#define EFI_BRIGHT 0x08
#define EFI_DARKGRAY 0x08
#define EFI_LIGHTBLUE 0x09
#define EFI_LIGHTGREEN 0x0A
#define EFI_LIGHTCYAN 0x0B
#define EFI_LIGHTRED 0x0C
#define EFI_LIGHTMAGENTA 0x0D
#define EFI_YELLOW 0x0E
#define EFI_WHITE 0x0F
#define EFI_BACKGROUND_BLACK 0x00
#define EFI_BACKGROUND_BLUE 0x10
#define EFI_BACKGROUND_GREEN 0x20
#define EFI_BACKGROUND_CYAN 0x30
#define EFI_BACKGROUND_RED 0x40
#define EFI_BACKGROUND_MAGENTA 0x50
#define EFI_BACKGROUND_BROWN 0x60
#define EFI_BACKGROUND_LIGHTGRAY 0x70
#define EFI_TEXT_ATTR(Foreground,Background) ((Foreground) | ((Background) << 4))

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_SET_ATTRIBUTE) (
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
    IN UINTN Attribute
    );

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_CLEAR_SCREEN) (
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This
    );

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_SET_CURSOR_POSITION) (
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
    IN UINTN Column,
    IN UINTN Row
    );

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_ENABLE_CURSOR) (
    IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
    IN BOOLEAN Visible
    );

typedef struct {
    INT32 MaxMode;
    // current settings
    INT32 Mode;
    INT32 Attribute;
    INT32 CursorColumn;
    INT32 CursorRow;
    BOOLEAN CursorVisible;
    } SIMPLE_TEXT_OUTPUT_MODE;

#define EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL_GUID \
    {0x387477c2,0x69c7,0x11d2,\
    {0x8e,0x39,0x00,0xa0,0xc9,0x69,0x72,0x3b}}

typedef struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
    EFI_TEXT_RESET Reset;
    EFI_TEXT_STRING OutputString;
    EFI_TEXT_TEST_STRING TestString;
    EFI_TEXT_QUERY_MODE QueryMode;
    EFI_TEXT_SET_MODE SetMode;
    EFI_TEXT_SET_ATTRIBUTE SetAttribute;
    EFI_TEXT_CLEAR_SCREEN ClearScreen;
    EFI_TEXT_SET_CURSOR_POSITION SetCursorPosition;
    EFI_TEXT_ENABLE_CURSOR EnableCursor;
    SIMPLE_TEXT_OUTPUT_MODE *Mode;
} EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

// EFI_SIMPLE_POINTER_PROTOCOL

typedef
EFI_STATUS
(EFIAPI *EFI_SIMPLE_POINTER_RESET) (
    IN EFI_SIMPLE_POINTER_PROTOCOL *This,
    IN BOOLEAN ExtendedVerification
    );

typedef struct {
    INT32 RelativeMovementX;
    INT32 RelativeMovementY;
    INT32 RelativeMovementZ;
    BOOLEAN LeftButton;
    BOOLEAN RightButton;
} EFI_SIMPLE_POINTER_STATE;

typedef
EFI_STATUS
(EFIAPI *EFI_SIMPLE_POINTER_GET_STATE) (
    IN EFI_SIMPLE_POINTER_PROTOCOL *This,
    OUT EFI_SIMPLE_POINTER_STATE *State
    );

typedef struct {
    UINT64 ResolutionX;
    UINT64 ResolutionY;
    UINT64 ResolutionZ;
    BOOLEAN LeftButton;
    BOOLEAN RightButton;
} EFI_SIMPLE_POINTER_MODE;

#define EFI_SIMPLE_POINTER_PROTOCOL_GUID \
    {0x31878c87,0xb75,0x11d5,\
    {0x9a,0x4f,0x00,0x90,0x27,0x3f,0xc1,0x4d}}

typedef struct _EFI_SIMPLE_POINTER_PROTOCOL {
    EFI_SIMPLE_POINTER_RESET Reset;
    EFI_SIMPLE_POINTER_GET_STATE GetState;
    EFI_EVENT WaitForInput;
    EFI_SIMPLE_POINTER_MODE *Mode;
} EFI_SIMPLE_POINTER_PROTOCOL;

// EFI_ABSOLUTE_POINTER_PROTOCOL

typedef
EFI_STATUS
(EFIAPI *EFI_ABSOLUTE_POINTER_RESET) (
    IN EFI_ABSOLUTE_POINTER_PROTOCOL *This,
    IN BOOLEAN ExtendedVerification
    );

#define EFI_ABSP_TouchActive 0x00000001
#define EFI_ABS_AltActive 0x00000002

typedef struct {
    UINT64 CurrentX;
    UINT64 CurrentY;
    UINT64 CurrentZ;
    UINT32 ActiveButtons;
} EFI_ABSOLUTE_POINTER_STATE;

typedef
EFI_STATUS
(EFIAPI *EFI_ABSOLUTE_POINTER_GET_STATE) (
    IN EFI_ABSOLUTE_POINTER_PROTOCOL *This,
    OUT EFI_ABSOLUTE_POINTER_STATE *State
    );

#define EFI_ABSP_SupportsAltActive 0x00000001
#define EFI_ABSP_SupportsPressureAsZ 0x00000002

typedef struct {
    UINT64 AbsoluteMinX;
    UINT64 AbsoluteMinY;
    UINT64 AbsoluteMinZ;
    UINT64 AbsoluteMaxX;
    UINT64 AbsoluteMaxY;
    UINT64 AbsoluteMaxZ;
    UINT32 Attributes;
} EFI_ABSOLUTE_POINTER_MODE;

#define EFI_ABSOLUTE_POINTER_PROTOCOL_GUID \
    {0x8D59D32B, 0xC655, 0x4AE9, \
    {0x9B, 0x15, 0xF2, 0x59, 0x04, 0x99, 0x2A, 0x43}}

typedef struct _EFI_ABSOLUTE_POINTER_PROTOCOL {
    EFI_ABSOLUTE_POINTER_RESET Reset;
    EFI_ABSOLUTE_POINTER_GET_STATE GetState;
    EFI_EVENT WaitForInput;
    EFI_ABSOLUTE_POINTER_MODE *Mode;
} EFI_ABSOLUTE_POINTER_PROTOCOL;

// EFI_GRAPHICS_OUTPUT_PROTOCOL

typedef struct {
    UINT32 RedMask;
    UINT32 GreenMask;
    UINT32 BlueMask;
    UINT32 ReservedMask;
} EFI_PIXEL_BITMASK;

typedef enum {
    PixelRedGreenBlueReserved8BitPerColor,
    PixelBlueGreenRedReserved8BitPerColor,
    PixelBitMask,
    PixelBltOnly,
    PixelFormatMax
} EFI_GRAPHICS_PIXEL_FORMAT;

typedef struct {
    UINT32 Version;
    UINT32 HorizontalResolution;
    UINT32 VerticalResolution;
    EFI_GRAPHICS_PIXEL_FORMAT PixelFormat;
    EFI_PIXEL_BITMASK PixelInformation;
    UINT32 PixelsPerScanLine;
} EFI_GRAPHICS_OUTPUT_MODE_INFORMATION;

typedef
EFI_STATUS
(EFIAPI *EFI_GRAPHICS_OUTPUT_PROTOCOL_QUERY_MODE) (
    IN EFI_GRAPHICS_OUTPUT_PROTOCOL *This,
    IN UINT32 ModeNumber,
    OUT UINTN *SizeOfInfo,
    OUT EFI_GRAPHICS_OUTPUT_MODE_INFORMATION **Info
    );

typedef
EFI_STATUS
(EFIAPI *EFI_GRAPHICS_OUTPUT_PROTOCOL_SET_MODE) (
    IN EFI_GRAPHICS_OUTPUT_PROTOCOL *This,
    IN UINT32 ModeNumber
    );
    
typedef struct {
    UINT8 Blue;
    UINT8 Green;
    UINT8 Red;
    UINT8 Reserved;
} EFI_GRAPHICS_OUTPUT_BLT_PIXEL;

typedef enum {
    EfiBltVideoFill,
    EfiBltVideoToBltBuffer,
    EfiBltBufferToVideo,
    EfiBltVideoToVideo,
    EfiGraphicsOutputBltOperationMax
} EFI_GRAPHICS_OUTPUT_BLT_OPERATION;

typedef
EFI_STATUS
(EFIAPI *EFI_GRAPHICS_OUTPUT_PROTOCOL_BLT) (
    IN EFI_GRAPHICS_OUTPUT_PROTOCOL *This,
    IN OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer, OPTIONAL
    IN EFI_GRAPHICS_OUTPUT_BLT_OPERATION BltOperation,
    IN UINTN SourceX,
    IN UINTN SourceY,
    IN UINTN DestinationX,
    IN UINTN DestinationY,
    IN UINTN Width,
    IN UINTN Height,
    IN UINTN Delta OPTIONAL
    );

typedef struct {
    UINT32 MaxMode;
    UINT32 Mode;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;
    UINTN SizeOfInfo;
    EFI_PHYSICAL_ADDRESS FrameBufferBase;
    UINTN FrameBufferSize;
} EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;

#define EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID \
    {0x9042a9de,0x23dc,0x4a38,\
    {0x96,0xfb,0x7a,0xde,0xd0,0x80,0x51,0x6a}}

typedef struct _EFI_GRAPHICS_OUTPUT_PROTOCOL {
    EFI_GRAPHICS_OUTPUT_PROTOCOL_QUERY_MODE QueryMode;
    EFI_GRAPHICS_OUTPUT_PROTOCOL_SET_MODE SetMode;
    EFI_GRAPHICS_OUTPUT_PROTOCOL_BLT Blt;
    EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE *Mode;
} EFI_GRAPHICS_OUTPUT_PROTOCOL;

// EFI_EDID_DISCOVERED_PROTOCOL

#define EFI_EDID_DISCOVERED_PROTOCOL_GUID \
    {0x1c0c34f6,0xd380,0x41fa,\
    {0xa0,0x49,0x8a,0xd0,0x6c,0x1a,0x66,0xaa}}

typedef struct _EFI_EDID_DISCOVERED_PROTOCOL {
    UINT32 SizeOfEdid;
    UINT8 *Edid;
} EFI_EDID_DISCOVERED_PROTOCOL;

// EFI_EDID_ACTIVE_PROTOCOL

#define EFI_EDID_ACTIVE_PROTOCOL_GUID \
    {0xbd8c1056,0x9f36,0x44ec,\
    {0x92,0xa8,0xa6,0x33,0x7f,0x81,0x79,0x86}}

typedef struct _EFI_EDID_ACTIVE_PROTOCOL {
    UINT32 SizeOfEdid;
    UINT8 *Edid;
} EFI_EDID_ACTIVE_PROTOCOL;

// EFI_EDID_OVERRIDE_PROTOCOL

typedef
EFI_STATUS
(EFIAPI *EFI_EDID_OVERRIDE_PROTOCOL_GET_EDID) (
    IN EFI_EDID_OVERRIDE_PROTOCOL *This,
    IN EFI_HANDLE *ChildHandle,
    OUT UINT32 *Attributes,
    OUT UINTN *EdidSize,
    OUT UINT8 **Edid
    );

#define EFI_EDID_OVERRIDE_PROTOCOL_GUID \
    {0x48ecb431,0xfb72,0x45c0,\
    {0xa9,0x22,0xf4,0x58,0xfe,0x04,0x0b,0xd5}}

typedef struct _EFI_EDID_OVERRIDE_PROTOCOL {
    EFI_EDID_OVERRIDE_PROTOCOL_GET_EDID GetEdid;
} EFI_EDID_OVERRIDE_PROTOCOL;

#define EFI_ENC_ERROR(VALUE) (((EFI_STATUS)(VALUE)) | MAX_BIT)
#define EFI_ENC_WARN(VALUE) ((EFI_STATUS)(VALUE))

#define EFI_SUCCESS                 0
#define EFI_LOAD_ERROR              EFI_ENC_ERROR(1)
#define EFI_INVALID_PARAMETER       EFI_ENC_ERROR(2)
#define EFI_UNSUPPORTED             EFI_ENC_ERROR(3)
#define EFI_BAD_BUFFER_SIZE         EFI_ENC_ERROR(4)
#define EFI_BUFFER_TOO_SMALL        EFI_ENC_ERROR(5)
#define EFI_NOT_READY               EFI_ENC_ERROR(6)
#define EFI_DEVICE_ERROR            EFI_ENC_ERROR(7)
#define EFI_WRITE_PROTECTED         EFI_ENC_ERROR(8)
#define EFI_OUT_OF_RESOURCES        EFI_ENC_ERROR(9)
#define EFI_VOLUME_CORRUPTED        EFI_ENC_ERROR(10)
#define EFI_VOLUME_FULL             EFI_ENC_ERROR(11)
#define EFI_NO_MEDIA                EFI_ENC_ERROR(12)
#define EFI_MEDIA_CHANGED           EFI_ENC_ERROR(13)
#define EFI_NOT_FOUND               EFI_ENC_ERROR(14)
#define EFI_ACCESS_DENIED           EFI_ENC_ERROR(15)
#define EFI_NO_RESPONSE             EFI_ENC_ERROR(16)
#define EFI_NO_MAPPING              EFI_ENC_ERROR(17)
#define EFI_TIMEOUT                 EFI_ENC_ERROR(18)
#define EFI_NOT_STARTED             EFI_ENC_ERROR(19)
#define EFI_ALREADY_STARTED         EFI_ENC_ERROR(20)
#define EFI_ABORTED                 EFI_ENC_ERROR(21)
#define EFI_ICMP_ERROR              EFI_ENC_ERROR(22)
#define EFI_TFTP_ERROR              EFI_ENC_ERROR(23)
#define EFI_PROTOCOL_ERROR          EFI_ENC_ERROR(24)
#define EFI_INCOMPATIBLE_VERSION    EFI_ENC_ERROR(25)
#define EFI_SECURITY_VIOLATION      EFI_ENC_ERROR(26)
#define EFI_CRC_ERROR               EFI_ENC_ERROR(27)
#define EFI_END_OF_MEDIA            EFI_ENC_ERROR(28)
#define EFI_END_OF_FILE             EFI_ENC_ERROR(31)
#define EFI_INVALID_LANGUAGE        EFI_ENC_ERROR(32)
#define EFI_COMPROMISED_DATA        EFI_ENC_ERROR(33)
#define EFI_IP_ADDRESS_CONFLICT     EFI_ENC_ERROR(34)
#define EFI_HTTP_ERROR              EFI_ENC_ERROR(35)

#define EFI_WARN_UNKNOWN_GLYPH      EFI_ENC_WARN(1)
#define EFI_WARN_DELETE_FAILURE     EFI_ENC_WARN(2)
#define EFI_WARN_WRITE_FAILURE      EFI_ENC_WARN(3)
#define EFI_WARN_BUFFER_TOO_SMALL   EFI_ENC_WARN(4)
#define EFI_WARN_STALE_DATA         EFI_ENC_WARN(5)
#define EFI_WARN_FILE_SYSTEM        EFI_ENC_WARN(6)
#define EFI_WARN_RESET_REQUIRED     EFI_ENC_WARN(7)

#ifdef __cplusplus
}
#endif

#endif