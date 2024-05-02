#include <stdint.h>
#include <string.h>

#include "bootloader/command.h"
#include "bootloader/console.h"

//---
// Internals
//---

/* disable out-of-bounds analysing since we use linker-script magic */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wanalyzer-out-of-bounds"

/* _command_next() : internal command iterator
 * @note
 * - `__bootloader_cmd_array_*` are provided by the linker script */
static int _command_next(struct bootloader_cmd **cmd)
{
    extern uintptr_t __bootloader_cmd_array_start;
    extern uintptr_t __bootloader_cmd_array_end;

    if (cmd == NULL)
        return -1;
    if (cmd[0] == NULL) {
        cmd[0] = (struct bootloader_cmd*)&__bootloader_cmd_array_start;
        return 0;
    }
    cmd[0] = &(cmd[0][1]);
    if ((uintptr_t)cmd[0] >= (uintptr_t)&__bootloader_cmd_array_end) {
        cmd[0] = NULL;
        return 1;
    }
    return 0;
}

/* _command_help() : display all available commands */
static int _command_help(void)
{
    struct bootloader_cmd *cmd;

    cmd = NULL;
    console_write("b\t- boot the kernel image\n");
    while (_command_next(&cmd) == 0)
    {
        console_write("%s\t- %s\n", cmd->name, cmd->desc);
    }
    return 0;
}

//---
// Public
//---

/* command_exec() : try to execute a command */
int command_exec(char const * const command)
{
    struct bootloader_cmd *cmd;

    cmd = NULL;
    while (_command_next(&cmd) == 0)
    {
        if (strcmp(cmd->name, command) != 0)
            continue;
        return cmd->func(command);
    }
    return 127;
}


/* declare generic "?" command */
VCMD_DECLARE(
    help_cmd,
    .name   = "?",
    .desc   = "list all commands",
    .func   = (void *)&_command_help,
);

/* restore GCC context (re-enable out-of-bounds checks */
#pragma GCC diagnostic pop
