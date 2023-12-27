#if 0
#include <vhex/fs/fugue.h>

#include <unistd.h>
#include <errno.h>
#include <string.h>

/* fugue_open() : try to open file */
int fugue_open(void **data, const char *pathname, int flags, int mode)
{
    (void)mode;
    if (data == NULL || pathname == NULL) {
        errno = EFAULT;
        return -1;
    }

    /* we need to convert the pathname into FONTCHARACTER format with meta
       information inside (mainly the device) */
    uint16_t *fcpath = fs_path_normalize_fc(pathname);
    if (fcpath == NULL) {
        errno = ENOMEM;
        return -1;
    }

    /* handle open mode */
    int bfile_mode = BFile_ReadOnly;
    if(flags & O_WRONLY)
        bfile_mode = BFile_WriteOnly;
    else if(flags & O_RDWR)
        bfile_mode = BFile_ReadWrite;

    /* Exclusive open means no sense unless creation is also requested */
    bool excl = (flags & O_EXCL) && (flags & O_CREAT);
    /* Truncation requires the file to be removed/recreated */
    bool trunc = (flags & O_TRUNC) && (flags & O_CREAT);

    /* Stat the entry. A special case is needed for the root, which doesn't
       respond well. fs_path_normalize_fc() normalizes the path so we just
       have to check for the fixed string "\\fls0\". */
    bool exists;
    if(!memcmp(fcpath, u"\\\\fls0\\", 16)) {
        exists = true;
        type = BFile_Type_Directory;
    } else {
        exists = (BFile_Ext_Stat(fcpath, &type, NULL) == 0);
        if(!exists) type = -1;
    }

    /* If the entry exists and O_EXCL was requested, fail. */
    if(exists && excl) {
        errno = EEXIST;
        rc = -1;
        goto end;
    }

    /* If the entry is not a directory but O_DIRECTORY is set, fail. If the
       directory doesn't exist, we fail regardless of O_CREAT. */
    if((flags & O_DIRECTORY) && type != BFile_Type_Directory) {
        errno = (exists ? ENOTDIR : ENOENT);
        rc = -1;
        goto end;
    }

    /* If the entry is a directory, open it as such */
    if(type == BFile_Type_Directory) {
        void *dp = fugue_dir_explore(path);
        fugue_descriptor_t data = {
            .type = &fugue_dir_descriptor_type,
            .data = dp,
        };
        fd = fugue_create_descriptor(&data);
        rc = fd;
        goto end;
    }

    /* Try and open the file normally, unless O_TRUNC is specified without
       O_EXCL, in which case we simply delete and recreate the file. */
    fugue_fd = BFile_EntryNotFound;
    if(trunc == 0)
        fugue_fd = BFile_Open(fcpath, bfile_mode);

    /* If O_TRUNC is requested and either the file exists or we can create
       it, remove it. (If fugue_fd < 0 an opening error might still have
       occurred so we delete it just in case.) */
    if((flags & O_TRUNC) && (fugue_fd >= 0 || (flags & O_CREAT))) {
        if(fugue_fd >= 0)
            BFile_Close(fugue_fd);
        BFile_Remove(fcpath);
        fugue_fd = BFile_EntryNotFound;
    }

    /* If the file does not exist and O_CREAT is set, create it */
    if((flags & O_CREAT) && ((flags & O_TRUNC) || fugue_fd < 0)) {
        new_file_size = 0;
        err = BFile_Create(fcpath, BFile_File, &new_file_size);
        if(err < 0) {
            errno = bfile_error_to_errno(err);
            rc = -1;
            goto end;
        }
        fugue_fd = BFile_Open(fcpath, bfile_mode);
    }

    /* check error during hadling */
    if(fugue_fd < 0) {
        errno = bfile_error_to_errno(fugue_fd);
        rc = fugue_fd;
        goto end;
    }

    /* If O_APPEND is set, move to the end of the file */
    // TODO: O_APPEND should move the cursor before *each* write
    int pos = 0;
    if((flags & O_APPEND)) {
        pos = BFile_Size(fugue_fd);
        BFile_Seek(fugue_fd, pos);
    }

    /* Return the now-open file descriptor */
    fugue_fd_t *data = malloc(sizeof(fugue_fs_t));
    if(data == NULL) {
        BFile_Close(fugue_fd);
        goto end;
    }
    data->fd = fugue_fd;
    data->pos = pos;

    /* generate file descriptor information */
    fugue_descriptor_t fd_data = {
        .type = &fugue_descriptor_type,
        .data = data,
    };
    rc = fd = fugue_create_descriptor(&fd_data);

    if(fd == -1) {
        BFile_Close(fugue_fd);
        free(data);
        errno = ENFILE;
        goto end;
    }

end:
    free(fcpath);
    return rc;
}
#endif
