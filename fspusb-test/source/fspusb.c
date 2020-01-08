#define NX_SERVICE_ASSUME_NON_DOMAIN
#include "fspusb.h"
#include "service_guard.h"

#include <string.h>

static Service g_fspusbSrv;

NX_GENERATE_SERVICE_GUARD(fspusb);

Result _fspusbInitialize(void) {
    return smGetService(&g_fspusbSrv, "fsp-usb");
}

void _fspusbCleanup(void) {
    serviceClose(&g_fspusbSrv);
}

Service* fspusbGetServiceSession(void) {
    return &g_fspusbSrv;
}

Result fspusbListMountedDrives(s32 *drives_buf, size_t drive_count, s32 *out_total) {
    return serviceDispatchOut(&g_fspusbSrv, 0, *out_total,
        .buffer_attrs = { SfBufferAttr_Out | SfBufferAttr_HipcMapAlias },
        .buffers = { { drives_buf, drive_count * sizeof(s32) } },
    );
}

Result fspusbGetDriveFileSystemType(s32 interface_id, FspUsbFileSystemType *out_type) {
    return serviceDispatchInOut(&g_fspusbSrv, 1, interface_id, *out_type);
}

Result fspusbGetDriveLabel(s32 interface_id, char *out_label, size_t out_label_size) {
    return serviceDispatchIn(&g_fspusbSrv, 2, interface_id,
        .buffer_attrs = { SfBufferAttr_Out | SfBufferAttr_HipcMapAlias },
        .buffers = { { out_label, out_label_size } },
    );
}

Result fspusbSetDriveLabel(s32 interface_id, const char *label) {
    char inputlbl[11 + 1] = {0}; // Actual limit is 11 characters
    strncpy(inputlbl, label, 11);
    return serviceDispatchIn(&g_fspusbSrv, 3, interface_id,
        .buffer_attrs = { SfBufferAttr_In | SfBufferAttr_HipcMapAlias },
        .buffers = { { inputlbl, 11 + 1 } },
    );
}

Result fspusbOpenDriveFileSystem(s32 interface_id, FsFileSystem *out_fs) {
    return serviceDispatchIn(&g_fspusbSrv, 4, interface_id,
        .out_num_objects = 1,
        .out_objects = &out_fs->s,
    );
}