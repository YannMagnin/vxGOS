#ifndef BOOTLOADER_COMMAND_H
#define BOOTLOADER_COMMAND_H 1

//---
// Public
//---

/* bootloader_cmd : bootloader command information */
struct bootloader_cmd
{
    char const * const name;
    char const * const desc;
    int (*func)(char const * const line);
};

/* VCMD_DECLARE() : declare bootloader command */
#define VCMD_DECLARE(name, ...)                 \
    __attribute__((section(".bootloader_cmd"))) \
    struct bootloader_cmd name = {              \
        __VA_ARGS__                             \
    }

/* command_exec() : try to execute a command */
extern int command_exec(char const * const command);

#endif /* BOOTLOADER_COMMAND_H */
