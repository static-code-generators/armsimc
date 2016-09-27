#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "shellcmds.h"

struct CmdContext {
    int argc;
    char *args[20];
};

void parse(struct CmdContext *ctx, char *cmdstr)
{
    char *tok;
    ctx->argc = 0;
    char *sep = " \n\t";
    for (tok = strtok(cmdstr, sep); tok; tok = strtok(NULL, sep)) {
        ctx->args[ctx->argc++] = strdup(tok);
    }
}

void exec(struct CmdContext *ctx)
{
    char * argerrstr = "Error: Argument Error, refer to `?` or `help`\n";
    if (ctx->argc == 0) {
        return;
    }
    char *cmd = ctx->args[0];
    if (strcmp(cmd, "r") == 0 || strcmp(cmd, "run") == 0) {
        cmd_run();
    } else if (strcmp(cmd, "file") == 0) {
        if (ctx->argc < 2) {
            fprintf(stderr, "%s", argerrstr);
            return;
        }
        char *fname = ctx->args[1];
        cmd_file(fname);
    } else if (strcmp(cmd, "step") == 0) {
        if (ctx->argc < 1) {
            fprintf(stderr, "%s", argerrstr);
            return;
        }
        int i;
        if (ctx->argc >= 2) {
            i = atoi(ctx->args[1]);
        } else {
            i = 1;
        }
        cmd_step(i);
    } else if (strcmp(cmd, "mdump") == 0) {
        if (ctx->argc < 3) {
            fprintf(stderr, "%s", argerrstr);
            return;
        }
        int l = atoi(ctx->args[1]), h = atoi(ctx->args[2]);
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
        if (ctx->argc < 3) {
            fprintf(stderr, "%s", argerrstr);
            return;
        }
        int rnum = atoi(ctx->args[1]), rval = atoi(ctx->args[2]);
        cmd_set(rnum, rval);
    } else if (strcmp(cmd, "?") == 0 || strcmp(cmd, "help") == 0) {
        cmd_help();
    } else if (strcmp(cmd, "q") == 0 || strcmp(cmd, "quit") == 0) {
        exit(EXIT_SUCCESS);
    } else {
        fprintf(stderr, "Error: Unknown command, refer to `?` or `help`\n");
    }
}

int parse_and_exec(char *cmdstr)
{
    struct CmdContext *ctx = malloc(sizeof(struct CmdContext));
    if (!ctx) {
        return -ENOMEM;
    }

    parse(ctx, cmdstr);
    exec(ctx);

    /* Free CmdContext */
    for (int i = 0; i < ctx->argc; i++)
        free(ctx->args[i]);
    free(ctx);

    return EXIT_SUCCESS;
}

int shell_loop()
{
    char cmdstr[100];
    int ret;
    while (1) {
        printf("armsh> ");
        fgets(cmdstr, 100, stdin);
        if ((ret = parse_and_exec(cmdstr)) < 0) { 
            return ret;
        }
    }
}

int main(int argc, char *argv[])
{
    int ret = shell_loop();
    return ret;
}

