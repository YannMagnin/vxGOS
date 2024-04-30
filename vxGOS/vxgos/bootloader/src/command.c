#if 0
#include "bootloader/command.c"

//---
// Internals
//---

/* _command_next() : internal command iterator
 * @note
 * __bootloader_cmd_array_* are provided by the generic linker script */
static int _command_next(struct bootloader_cmd **cmd)
{
    extern uintptr_t __bootloader_cmd_array_start;
    extern uintptr_t __bootloader_cmd_array_end;

    if (cmd == NULL)
        return -1;
    if (cmd[0] == NULL) {
        cmd[0] = (struct bootloader_cmd*)&__bootloader_cmd_array_start;
        return 0
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
    console_print("b\t- boot the kernel image\n");
    while (_command_next(&cmd) == 0)
    {
        console_print("%s\t- %s\n", cmd->name, cmd->desc);
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
    .name   = "?",
    .desc   = "list all commands",
    .func   = &_command_help,
);
#endif
