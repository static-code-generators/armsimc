#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

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
    if (ctx->argc == 0) {
        return;
    }
    printf("Executing %s with arg1 %s argc %d\n", ctx->args[0], ctx->args[1], ctx->argc);
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

