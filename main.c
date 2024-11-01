#include "include/handler.h"
#include <concord/discord_codecs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <concord/discord.h>

void on_ready(struct discord *client) {
    struct discord_activity activities[] = {
        {
            .name = "Blåhaj prices",
            .type = DISCORD_ACTIVITY_WATCHING,
        },
    };

    struct discord_presence_update status = {
        .activities =
            &(struct discord_activities){
                .size = sizeof(activities) / sizeof *activities,
                .array = activities,
            },
        .status = "online",
        .afk = false,
        .since = discord_timestamp(client),
    };

    discord_update_presence(client, &status);
}

void on_beep(struct discord *client, const struct discord_interaction *event) {
    struct discord_interaction_response params = {
        .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,
        .data = &(struct discord_interaction_callback_data){
            .content = "beep"
        }
    };
    discord_create_interaction_response(client, event->id,
            event->token, &params, NULL);
}

struct guild_command guild_commands[] = {
    { { .name = "beep", .description = "Beeps back" }, on_beep },
    { 0 }
};

struct discord_create_guild_application_command global_commands[] = {
    { 0 }
};

int main(void) {
    ccord_global_init();
    struct ccord_szbuf_readonly value;
    struct discord *client = discord_config_init("config.json");

    value = discord_config_get_field(client, (char *[2]){ "discord", "guild_id" }, 2);
    init_handler(strtoull(value.start, NULL, 10), client, on_ready);
    discord_run(client);
}
