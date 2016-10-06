#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include "shellcmds.h"

struct CmdContext {
    int argc;
    char *args[20];
};

/** strdup isn't in c99 std, though in POSIX, so ftfy i make my own */
char * strdup(const char *str) {
    char *newstr = malloc(sizeof(char) * strlen(str));
    strcpy(newstr, str);
    return newstr;
}

/** Parses a string read from shell into CmdContext. */
void parse(struct CmdContext *ctx, char *cmdstr)
{
    char *tok;
    ctx->argc = 0;
    char *sep = " \n\t";
    for (tok = strtok(cmdstr, sep); tok; tok = strtok(NULL, sep)) {
        ctx->args[ctx->argc++] = strdup(tok);
    }
}

/** Calls the relevant functions from shellcmds module. */
void exec(struct CmdContext *ctx)
{
    char * argerrstr = "Error: Argument Error, refer to `?` or `help`\n";
#define CHECK_ARGC_ELSE_RETURN(x) if (ctx->argc < x) { fprintf(stderr, "%s", argerrstr); return; }
    if (ctx->argc == 0) {
        return;
    }
    char *cmd = ctx->args[0];
    if (strcmp(cmd, "r") == 0 || strcmp(cmd, "run") == 0) {
        cmd_run();
    } else if (strcmp(cmd, "file") == 0) {
        CHECK_ARGC_ELSE_RETURN(2);
        char *fname = ctx->args[1];
        cmd_file(fname);
    } else if (strcmp(cmd, "step") == 0) {
        CHECK_ARGC_ELSE_RETURN(1);
        int i;
        if (ctx->argc >= 2) {
            i = atoi(ctx->args[1]);
        } else {
            i = 1;
        }
        cmd_step(i);
    } else if (strcmp(cmd, "mdump") == 0) {
        CHECK_ARGC_ELSE_RETURN(3);
        uint32_t l, h;
        sscanf(ctx->args[1], "0x%x", &l);
        sscanf(ctx->args[2], "0x%x", &h);
        char * fname = NULL;
        if (ctx->argc >= 4) {
            fname = ctx->args[3];
        }
        cmd_mdump(l, h, fname);
    } else if (strcmp(cmd, "rdump") == 0) {
        char * fname = NULL;
        if (ctx->argc >= 2) {
            fname = ctx->args[1];
        }
        cmd_rdump(fname);
    } else if (strcmp(cmd, "set") == 0) {
        CHECK_ARGC_ELSE_RETURN(3);
        int rnum;
        sscanf(ctx->args[1], "r%d", &rnum);
        uint32_t rval;
        sscanf(ctx->args[2], "0x%x", &rval);
        cmd_set(rnum, rval);
    } else if (strcmp(cmd, "?") == 0 || strcmp(cmd, "help") == 0) {
        cmd_help();
    } else if (strcmp(cmd, "q") == 0 || strcmp(cmd, "quit") == 0) {
        exit(EXIT_SUCCESS);
    } else {
        fprintf(stderr, "Error: Unknown command, refer to `?` or `help`\n");
    }
#undef CHECK_ARGC_ELSE_RETURN
}

/** Parses and executes the line read from shell. */
int parse_and_exec(char *cmdstr)
{
    struct CmdContext *ctx = malloc(sizeof(struct CmdContext));
    if (!ctx) {
        return EXIT_FAILURE;
    }

    parse(ctx, cmdstr);
    exec(ctx);

    /* Free CmdContext */
    for (int i = 0; i < ctx->argc; i++)
        free(ctx->args[i]);
    free(ctx);

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    char cmdstr[100];
    int ret;
    if (argc == 2) {
        cmd_file(argv[1]);
    }
    if (argc > 2) {
        fprintf(stderr, "Wrong number of args. Run as %s [hex_file]\n", argv[0]);
    }
    while (1) {
        printf("armsh> ");
        if (fgets(cmdstr, 100, stdin) == NULL) {
            return EXIT_SUCCESS;
        }
        if ((ret = parse_and_exec(cmdstr)) < 0) { 
            return ret;
        }
    }
}
