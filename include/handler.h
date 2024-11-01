#pragma once

#include <concord/discord.h>

void init_handler(u64snowflake guild_id, struct discord *client,
    void (*on_ready)(struct discord *client));

struct guild_command {
    struct discord_create_guild_application_command cmd;
    void (*on_call)(struct discord*, const struct discord_interaction*);
};
