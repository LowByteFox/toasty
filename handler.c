#include "include/handler.h"

#include <string.h>
#include <concord/discord.h>
#include <concord/log.h>

extern struct guild_command guild_commands[];
extern struct discord_create_global_application_command global_commands[];

static void (*_on_ready)(struct discord *client) = NULL;
static u64snowflake _guild_id = 0;

static void on_concord_ready(struct discord *client,
        const struct discord_ready *event);
void on_concord_interraction(struct discord *client,
        const struct discord_interaction *event);

void init_handler(u64snowflake guild_id, struct discord *client,
    void (*on_ready)(struct discord *client))
{
    log_info("Starting bot!");

    _guild_id = guild_id;
    _on_ready = on_ready;
    discord_set_on_ready(client, on_concord_ready);
}

static void on_concord_ready(struct discord *client,
        const struct discord_ready *event)
{
    log_info("Cleaning up old commands!");
    struct discord_application_commands app_cmds = { 0 };

    struct discord_ret_application_commands ret = {
        .sync = &app_cmds,
    };

    struct discord_ret del_ret = { .sync = true };

    CCORDcode code = discord_get_guild_application_commands(client, event->application->id,
            _guild_id, &ret);

    if (CCORD_OK == code && app_cmds.size) {
        for (int i = 0; i < app_cmds.size; ++i) {
            discord_delete_guild_application_command(client, event->application->id,
                    _guild_id, app_cmds.array[i].id, &del_ret);
        }

        discord_application_commands_cleanup(&app_cmds);
    }

    struct discord_application_commands global_app_cmds = { 0 };

    ret.sync = &global_app_cmds;

    struct discord_ret global_del_ret = { .sync = true };

    code = discord_get_guild_application_commands(client, event->application->id,
            _guild_id, &ret);

    if (CCORD_OK == code && global_app_cmds.size) {
        for (int i = 0; i < global_app_cmds.size; ++i) {
            discord_delete_guild_application_command(client, event->application->id,
                    _guild_id, global_app_cmds.array[i].id, &global_del_ret);
        }

        discord_application_commands_cleanup(&global_app_cmds);
    }

    log_info("Installing commands!");

    struct guild_command *iter = guild_commands;

    while (iter->cmd.name != NULL) {
        log_info("Installing: %s", iter->cmd.name);
        discord_create_guild_application_command(client, event->application->id,
                _guild_id, &iter->cmd, NULL);
        iter++;
    }

    struct discord_create_global_application_command *global_iter = global_commands;

    while (global_iter->name != NULL) {
        log_info("Installing global: %s", global_iter->name);
        discord_create_global_application_command(client, event->application->id,
                global_iter, NULL);
        global_iter++;
    }

    log_info("Bot is ready!");

    if (_on_ready != NULL) {
        _on_ready(client);
    }

    discord_set_on_interaction_create(client, on_concord_interraction);
}

void on_concord_interraction(struct discord *client,
        const struct discord_interaction *event)
{
    if (event->type != DISCORD_INTERACTION_APPLICATION_COMMAND)
        return; /* return if interaction isn't a slash command */

    struct guild_command *iter = guild_commands;

    while (iter->cmd.name != NULL) {
        if (strcmp(event->data->name, iter->cmd.name) == 0) {
            iter->on_call(client, event);
            break;
        }
        iter++;
    }
}
