/*********************
 *      INCLUDES
 *********************/

#include "lv_uefi_fs.h"
#include "lv_uefi_private.h"

#if LV_USE_UEFI

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct _lv_uefi_fs_context_t {
    EFI_FILE_PROTOCOL * fs_root;
    BOOLEAN read_only;
} lv_uefi_fs_context_t;

typedef struct _lv_uefi_fs_file_context_t {
    EFI_FILE_PROTOCOL * interface;
} lv_uefi_fs_file_context_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static bool lv_uefi_fs_ready_cb(lv_fs_drv_t * drv);

static void * lv_uefi_fs_open_cb(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode);
static lv_fs_res_t lv_uefi_fs_close_cb(lv_fs_drv_t * drv, void * file_p);
static lv_fs_res_t lv_uefi_fs_read_cb(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br);
static lv_fs_res_t lv_uefi_fs_write_cb(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw);
static lv_fs_res_t lv_uefi_fs_seek_cb(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence);
static lv_fs_res_t lv_uefi_fs_tell_cb(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p);

static void * lv_uefi_fs_dir_open_cb(lv_fs_drv_t * drv, const char * path);
static lv_fs_res_t lv_uefi_fs_dir_read_cb(lv_fs_drv_t * drv, void * rddir_p, char * fn, uint32_t fn_len);
static lv_fs_res_t lv_uefi_fs_dir_close_cb(lv_fs_drv_t * drv, void * rddir_p);

/**********************
 *  GOLBAL VARIABLES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

static lv_fs_drv_t drv;
static lv_uefi_fs_context_t drv_ctx = { NULL };
static EFI_GUID _uefi_guid_simple_file_system = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
static EFI_GUID _uefi_guid_loaded_image = EFI_LOADED_IMAGE_PROTOCOL_GUID;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief Register a driver for the File system interface.
*/
void lv_fs_uefi_init(
    void * handle,
    uint8_t read_only)
{
    EFI_STATUS status;
    EFI_LOADED_IMAGE_PROTOCOL * interface_loaded_image = NULL;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL * interface_fs = NULL;

    LV_ASSERT_MSG(drv_ctx.fs_root == NULL, "[lv_uefi] File system driver is already initialized.");

    if(handle == NULL) {
        LV_ASSERT_NULL(gLvEfiImageHandle);

        interface_loaded_image = lv_uefi_open_protocol(gLvEfiImageHandle, &_uefi_guid_loaded_image);
        LV_ASSERT_NULL(interface_loaded_image);

        interface_fs = lv_uefi_open_protocol(interface_loaded_image->DeviceHandle, &_uefi_guid_simple_file_system);
    }
    else {
        interface_fs = lv_uefi_open_protocol(handle, &_uefi_guid_simple_file_system);
    }

    // May happen if the applications was not loaded from a block device
    if(interface_fs == NULL) {
        LV_LOG_WARN("[lv_uefi] Unable to open the file system protocol.");
        goto error;
    }

    drv_ctx.read_only = read_only == 1;

    status = interface_fs->OpenVolume(
                 interface_fs,
                 &drv_ctx.fs_root);
    if(status != EFI_SUCCESS) {
        LV_LOG_WARN("[lv_uefi] Unable to open the file system.");
        goto error;
    }

    lv_fs_drv_init(&drv);

    drv.letter = 'E';
    drv.cache_size = 0;

    drv.ready_cb = lv_uefi_fs_ready_cb;
    drv.open_cb = lv_uefi_fs_open_cb;
    drv.close_cb = lv_uefi_fs_close_cb;
    drv.read_cb = lv_uefi_fs_read_cb;
    drv.write_cb = lv_uefi_fs_write_cb;
    drv.seek_cb = lv_uefi_fs_seek_cb;
    drv.tell_cb = lv_uefi_fs_tell_cb;

    drv.dir_open_cb = lv_uefi_fs_dir_open_cb;
    drv.dir_read_cb = lv_uefi_fs_dir_read_cb;
    drv.dir_close_cb = lv_uefi_fs_dir_close_cb;

    drv.user_data = (void *) &drv_ctx;

    lv_fs_drv_register(&drv);

    goto finish;

error:
    if(drv_ctx.fs_root != NULL) {
        drv_ctx.fs_root->Close(drv_ctx.fs_root);
        drv_ctx.fs_root = NULL;
    }

finish:
    if(interface_loaded_image != NULL) {
        if(interface_fs != NULL) lv_uefi_close_protocol(interface_loaded_image->DeviceHandle, &_uefi_guid_simple_file_system);
        lv_uefi_close_protocol(gLvEfiImageHandle, &_uefi_guid_loaded_image);
    }
}

void lv_fs_uefi_deinit()
{
    if(drv_ctx.fs_root != NULL) {
        drv_ctx.fs_root->Close(drv_ctx.fs_root);
        drv_ctx.fs_root = NULL;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static bool lv_uefi_fs_ready_cb(lv_fs_drv_t * drv)
{
    lv_uefi_fs_context_t * drv_ctx = (lv_uefi_fs_context_t *)drv->user_data;

    return drv_ctx->fs_root != NULL;
}

static void * lv_uefi_fs_open_cb(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
{
    EFI_STATUS status;
    CHAR16 path_ucs2[LV_FS_MAX_PATH_LENGTH + 1];
    lv_uefi_fs_file_context_t * file_ctx = NULL;
    UINT64 uefi_mode = 0;

    lv_uefi_fs_context_t * drv_ctx = (lv_uefi_fs_context_t *)drv->user_data;

    if(drv_ctx->fs_root == NULL) goto error;

    if(mode == LV_FS_MODE_WR && drv_ctx->read_only) {
        LV_LOG_WARN("[lv_uefi] Unable to open file for writing, the driver is read only.");
        goto error;
    }

    if(lv_uefi_ascii_to_ucs2(path, path_ucs2, LV_FS_MAX_PATH_LENGTH + 1) == 0) {
        LV_LOG_WARN("[lv_uefi] Unable to convert the ASCII path into an UCS-2 path.");
        goto error;
    }

    file_ctx = lv_calloc(1, sizeof(lv_uefi_fs_file_context_t));
    LV_ASSERT_MALLOC(file_ctx);

    if(mode == LV_FS_MODE_WR) {
        uefi_mode = EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE;
    }
    else {
        uefi_mode = EFI_FILE_MODE_READ;
    }

    status = drv_ctx->fs_root->Open(
                 drv_ctx->fs_root,
                 &file_ctx->interface,
                 path_ucs2,
                 uefi_mode,
                 0);
    if(status != EFI_SUCCESS) {
        LV_LOG_WARN("[lv_uefi] Unable to open file.");
        goto error;
    }

    goto finish;

error:
    if(file_ctx != NULL) {
        if(file_ctx->interface != NULL) file_ctx->interface->Close(file_ctx->interface);
        lv_free(file_ctx);
        file_ctx = NULL;
    }

finish:

    return file_ctx;
}

static lv_fs_res_t lv_uefi_fs_close_cb(lv_fs_drv_t * drv, void * file_p)
{
    EFI_STATUS status;
    lv_uefi_fs_file_context_t * file_ctx = (lv_uefi_fs_file_context_t *)file_p;

    if(file_ctx->interface == NULL) return LV_FS_RES_NOT_EX;

    status = file_ctx->interface->Close(file_ctx->interface);
    if(status != EFI_SUCCESS) return LV_FS_RES_HW_ERR;

    lv_free(file_ctx);

    return LV_FS_RES_OK;
}

static lv_fs_res_t lv_uefi_fs_read_cb(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    EFI_STATUS status;
    lv_uefi_fs_file_context_t * file_ctx = (lv_uefi_fs_file_context_t *)file_p;
    UINTN buf_size = btr;

    if(file_ctx->interface == NULL) return LV_FS_RES_NOT_EX;

    status = file_ctx->interface->Read(
                     file_ctx->interface,
                     &buf_size,
                     buf);
    if(status != EFI_SUCCESS) return LV_FS_RES_HW_ERR;

    *br = (uint32_t) buf_size;

    return LV_FS_RES_OK;
}

static lv_fs_res_t lv_uefi_fs_write_cb(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw)
{
    EFI_STATUS status;
    lv_uefi_fs_context_t * drv_ctx = (lv_uefi_fs_context_t *)drv->user_data;
    lv_uefi_fs_file_context_t * file_ctx = (lv_uefi_fs_file_context_t *)file_p;
    UINTN buf_size = btw;

    if(drv_ctx->read_only) return LV_FS_RES_DENIED;
    if(file_ctx->interface == NULL) return LV_FS_RES_NOT_EX;

    status = file_ctx->interface->Write(
                     file_ctx->interface,
                     &buf_size,
                     (VOID *)buf);
    if(status != EFI_SUCCESS) return LV_FS_RES_HW_ERR;

    *bw = (uint32_t) buf_size;

    return LV_FS_RES_OK;
}

static lv_fs_res_t lv_uefi_fs_seek_cb(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence)
{
    EFI_STATUS status;
    lv_uefi_fs_file_context_t * file_ctx = (lv_uefi_fs_file_context_t *)file_p;
    UINT64 new_pos;

    if(file_ctx->interface == NULL) return LV_FS_RES_NOT_EX;

    if(whence == LV_FS_SEEK_END) {
        status = file_ctx->interface->SetPosition(
                         file_ctx->interface,
                         UINT64_MAX);
        if(status != EFI_SUCCESS) return LV_FS_RES_HW_ERR;

        status = file_ctx->interface->GetPosition(
                         file_ctx->interface,
                         &new_pos);
        if(status != EFI_SUCCESS) return LV_FS_RES_HW_ERR;

        if(new_pos < pos) {
            new_pos = 0;
        }
        else {
            new_pos -= pos;
        }
    }
    else if(whence == LV_FS_SEEK_SET) {
        new_pos = pos;
    }
    else if(whence == LV_FS_SEEK_CUR) {
        status = file_ctx->interface->GetPosition(
                         file_ctx->interface,
                         &new_pos);
        if(status != EFI_SUCCESS) return LV_FS_RES_HW_ERR;

        new_pos += pos;
    }
    else {
        return LV_FS_RES_INV_PARAM;
    }

    status = file_ctx->interface->SetPosition(
                     file_ctx->interface,
                     new_pos);
    if(status != EFI_SUCCESS) return LV_FS_RES_HW_ERR;

    return LV_FS_RES_OK;
}

static lv_fs_res_t lv_uefi_fs_tell_cb(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p)
{
    EFI_STATUS status;
    lv_uefi_fs_file_context_t * file_ctx = (lv_uefi_fs_file_context_t *)file_p;
    UINT64 pos;

    if(file_ctx->interface == NULL) return LV_FS_RES_NOT_EX;

    status = file_ctx->interface->GetPosition(
                     file_ctx->interface,
                     &pos);
    if(status != EFI_SUCCESS) return LV_FS_RES_HW_ERR;

    if(pos > UINT32_MAX) return LV_FS_RES_UNKNOWN;

    *pos_p = (uint32_t) pos;

    return LV_FS_RES_OK;
}

static void * lv_uefi_fs_dir_open_cb(lv_fs_drv_t * drv, const char * path)
{
    EFI_STATUS status;
    CHAR16 path_ucs2[LV_FS_MAX_PATH_LENGTH + 1];
    lv_uefi_fs_file_context_t * file_ctx = NULL;
    UINT64 mode = 0;
    UINT64 attributes = 0;

    lv_uefi_fs_context_t * drv_ctx = (lv_uefi_fs_context_t *)drv->user_data;

    if(drv_ctx->fs_root == NULL) goto error;

    if(lv_uefi_ascii_to_ucs2(path, path_ucs2, LV_FS_MAX_PATH_LENGTH + 1) == 0) {
        LV_LOG_WARN("[lv_uefi] Unable to convert the ASCII path into an UCS-2 path.");
        goto error;
    }

    file_ctx = lv_calloc(1, sizeof(lv_uefi_fs_file_context_t));
    LV_ASSERT_MALLOC(file_ctx);

    if(drv_ctx->read_only) {
        mode = EFI_FILE_MODE_READ;
    }
    else {
        mode = EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE;
        attributes = EFI_FILE_DIRECTORY;
    }

    status = drv_ctx->fs_root->Open(
                 drv_ctx->fs_root,
                 &file_ctx->interface,
                 path_ucs2,
                 mode,
                 attributes);
    if(status != EFI_SUCCESS) {
        LV_LOG_WARN("[lv_uefi] Unable to open directory.");
        goto error;
    }

    goto finish;

error:
    if(file_ctx != NULL) {
        if(file_ctx->interface != NULL) {
            file_ctx->interface->Close(file_ctx->interface);
        }
        lv_free(file_ctx);
        file_ctx = NULL;
    }

finish:

    return file_ctx;
}

static lv_fs_res_t lv_uefi_fs_dir_read_cb(lv_fs_drv_t * drv, void * rddir_p, char * fn, uint32_t fn_len)
{
    lv_fs_res_t return_code;
    EFI_STATUS status;
    lv_uefi_fs_file_context_t * file_ctx = (lv_uefi_fs_file_context_t *)rddir_p;

    EFI_FILE_INFO * info = NULL;
    UINTN size;

    if(file_ctx->interface == NULL) return LV_FS_RES_NOT_EX;

    size = 0;
    status = file_ctx->interface->Read(
                     file_ctx->interface,
                     &size,
                     info);
    if(status != EFI_BUFFER_TOO_SMALL || size == 0) {
        return_code = LV_FS_RES_NOT_EX;
        goto error;
    }

    info = lv_calloc(1, size);
    LV_ASSERT_MALLOC(info);

    status = file_ctx->interface->Read(
                     file_ctx->interface,
                     &size,
                     info);
    if(status != EFI_SUCCESS) {
        return_code = LV_FS_RES_HW_ERR;
        goto error;
    }

    if(lv_uefi_ucs2_to_ascii(info->FileName, fn, fn_len) == 0) {
        LV_LOG_WARN("[lv_uefi] Unable to convert the UCS-2 path into an ascii path.");
        return_code = LV_FS_RES_UNKNOWN;
        goto error;
    }

    return_code = LV_FS_RES_OK;
    goto finish;

error:

finish:
    if(info) lv_free(info);

    return return_code;
}

static lv_fs_res_t lv_uefi_fs_dir_close_cb(lv_fs_drv_t * drv, void * rddir_p)
{
    EFI_STATUS status;
    lv_uefi_fs_file_context_t * file_ctx = (lv_uefi_fs_file_context_t *)rddir_p;

    if(file_ctx->interface == NULL) return LV_FS_RES_NOT_EX;

    status = file_ctx->interface->Close(file_ctx->interface);
    if(status != EFI_SUCCESS) return LV_FS_RES_HW_ERR;

    lv_free(file_ctx);

    return LV_FS_RES_OK;
}

#endif