//#if LV_BUILD_TEST
#include "lv_efi.h"

static bool _s_exit_flag = false;

static EFI_GRAPHICS_OUTPUT_PROTOCOL * _find_gop()
{
    EFI_STATUS  status;
    EFI_HANDLE * handles = NULL;
    UINTN no_handles;
    UINTN index;
    EFI_GRAPHICS_OUTPUT_PROTOCOL * protocol = NULL;
    EFI_GUID protocol_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GUID edid_active_protocol_guid = EFI_EDID_ACTIVE_PROTOCOL_GUID;

    status = gBS->LocateHandleBuffer(ByProtocol, &protocol_guid, NULL, &no_handles, &handles);
    if(status != EFI_SUCCESS) {
        LV_LOG_WARN("Unable to find a handle with an installed graphics output protocol, error code: %0llx.", status);
        goto cleanup;
    }

    for(index = 0; index < no_handles; index++) {
        EFI_GRAPHICS_OUTPUT_PROTOCOL * candidate = NULL;
        EFI_EDID_ACTIVE_PROTOCOL * edid = NULL;

        status = gBS->OpenProtocol(handles[index], &protocol_guid, (VOID **) &candidate, gImageHandle, NULL,
                                   EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
        if(status != EFI_SUCCESS) {
            continue;
        }

        if(protocol == NULL) {
            protocol = candidate;
        }

        status = gBS->OpenProtocol(handles[index], &edid_active_protocol_guid, (VOID **) &edid, gImageHandle, NULL,
                                   EFI_OPEN_PROTOCOL_TEST_PROTOCOL);
        if(status != EFI_SUCCESS) {
            continue;
        }

        if(protocol == NULL) {
            protocol = candidate;
        }
    }

cleanup:
    if(handles) gBS->FreePool(handles);

    return protocol;
}

static EFI_SIMPLE_TEXT_INPUT_PROTOCOL * _find_text_input()
{
    EFI_STATUS  status;
    EFI_HANDLE * handles = NULL;
    UINTN no_handles;
    UINTN index;
    EFI_SIMPLE_TEXT_INPUT_PROTOCOL * protocol = NULL;
    EFI_GUID protocol_guid = EFI_SIMPLE_TEXT_INPUT_PROTOCOL_GUID;

    // shortcut
    if(gST->ConIn) {
        return gST->ConIn;
    }

    status = gBS->LocateHandleBuffer(ByProtocol, &protocol_guid, NULL, &no_handles, &handles);
    if(status != EFI_SUCCESS) {
        LV_LOG_WARN("Unable to find a handle with an installed text input protocol, error code: %0llx.", status);
        goto cleanup;
    }

    for(index = 0; index < no_handles; index++) {
        EFI_SIMPLE_TEXT_INPUT_PROTOCOL * candidate = NULL;

        status = gBS->OpenProtocol(handles[index], &protocol_guid, (VOID **) &candidate, gImageHandle, NULL,
                                   EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
        if(status != EFI_SUCCESS) {
            continue;
        }

        if(protocol == NULL) {
            protocol = candidate;
        }

        if(protocol->WaitForKey == NULL) {
            continue;
        }

        if(protocol == NULL) {
            protocol = candidate;
        }
    }

cleanup:
    if(handles) gBS->FreePool(handles);

    return protocol;
}

static void _exit_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {
        _s_exit_flag = true;
    }
    else if(code == LV_EVENT_PRESSED) {
        _s_exit_flag = true;
    }
}

EFI_STATUS efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE * system_table)
{
    EFI_STATUS status;
    EFI_SIMPLE_TEXT_INPUT_PROTOCOL * text_input_protocol = NULL;
    EFI_GRAPHICS_OUTPUT_PROTOCOL * graphics_output_protocol = NULL;
    lv_display_t * display = NULL;
    lv_indev_t * keyboard = NULL;

    status = lv_efi_init(image_handle, system_table);
    if(status != EFI_SUCCESS) {
        LV_LOG_ERROR("lv_efi_init failed.");
        goto cleanup;
    }

    graphics_output_protocol = _find_gop();
    if(graphics_output_protocol == NULL) {
        goto cleanup;
    }

    text_input_protocol = _find_text_input();
    if(text_input_protocol == NULL) {
        goto cleanup;
    }

    display = lv_efi_init_display(graphics_output_protocol);
    if(display == NULL) {
        goto cleanup;
    }

    keyboard = lv_efi_init_keyboard(text_input_protocol);
    if(keyboard == NULL) {
        goto cleanup;
    }

    lv_indev_set_display(keyboard, display);

    lv_obj_t * screen = lv_obj_create(NULL);
    lv_obj_set_layout(screen, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(screen, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_group_t * default_group = lv_group_create();
    lv_group_set_default(default_group);
    lv_indev_set_group(keyboard, default_group);

    lv_obj_t * hello_label = lv_label_create(screen);
    lv_label_set_text(hello_label, "Hello from UEFI.");

    lv_obj_t * text_area = lv_textarea_create(screen);

    lv_obj_t * exit = lv_button_create(screen);
    lv_obj_t * exit_label = lv_label_create(exit);
    lv_label_set_text(exit_label, "exit");
    lv_obj_add_event_cb(exit, _exit_event_handler, LV_EVENT_ALL, NULL);

    lv_group_add_obj(lv_group_get_default(), text_area);
    lv_group_add_obj(lv_group_get_default(), exit);

    lv_screen_load(screen);

    _s_exit_flag = false;
    while(_s_exit_flag == false) {
        gBS->Stall(1000);   /*Sleep for 1 millisecond*/
        lv_tick_inc(1);     /*Tell LVGL that 1 millisecond was elapsed*/
        lv_timer_handler();
    }

    status = EFI_SUCCESS;
    goto cleanup;

cleanup:
    if(display) {
        lv_efi_deinit_display(display);
    }

    if(keyboard) {
        lv_efi_deinit_keyboard(keyboard);
    }

    lv_efi_deinit();

    return status;
}

// memcpy is required as symbol for the clang compiler
void * memcpy(void * s, const void * ct, size_t n)
{
    const uint8_t * ct_8 = (const uint8_t *) ct;
    uint8_t * s_8 = (uint8_t *) s;

    while(n-- > 0) {
        *s_8++ = *ct_8++;
    }

    return s;
}

// memset is required as symbol for the clang compiler
void * memset(void * s, int c, size_t n)
{
    uint8_t * s_8 = (uint8_t *)s;

    while(n-- > 0) {
        *s_8++ = (uint8_t)c;
    }

    return s;
}

//#endif
