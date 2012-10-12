
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <guacamole/client.h>
#include <guacamole/protocol.h>

#include "ball_client.h"

/* Client plugin arguments */
const char* GUAC_CLIENT_ARGS[] = { NULL };

int guac_client_init(guac_client* client, int argc, char** argv) {

    ball_client_data* data = malloc(sizeof(ball_client_data));

    /* Send the name of the connection */
    guac_protocol_send_name(client->socket, "Bouncing Ball");

    /* Send the display size */
    guac_protocol_send_size(client->socket, GUAC_DEFAULT_LAYER, 1024, 768);

    /* Fill with solid color */
    guac_protocol_send_rect(client->socket, GUAC_DEFAULT_LAYER,
            0, 0, 1024, 768);

    guac_protocol_send_cfill(client->socket,
            GUAC_COMP_OVER, GUAC_DEFAULT_LAYER,
            0x80, 0x80, 0x80, 0xFF);

    /* Set up client data and handlers */
    client->data = data;
    client->handle_messages = ball_client_handle_messages;

    /* Set up our ball layer */
    data->ball = guac_client_alloc_layer(client);

    /* Start ball at upper left */
    data->ball_x = 0;
    data->ball_y = 0;

    /* Move at a reasonable pace to the lower right */
    data->ball_velocity_x = 200; /* pixels per second */
    data->ball_velocity_y = 200;   /* pixels per second */

    guac_protocol_send_size(client->socket, data->ball, 128, 128);

    /* Fill with solid color */
    guac_protocol_send_rect(client->socket, data->ball,
            0, 0, 128, 128);

    guac_protocol_send_cfill(client->socket,
            GUAC_COMP_OVER, data->ball,
            0x00, 0x80, 0x80, 0xFF);

    /* Flush buffer */
    guac_socket_flush(client->socket);

    /* Done */
    return 0;

}

int ball_client_handle_messages(guac_client* client) {

    /* Get data */
    ball_client_data* data = (ball_client_data*) client->data;

    /* Sleep a bit */
    usleep(30000);

    /* Update position */
    data->ball_x += data->ball_velocity_x * 30 / 1000;
    data->ball_y += data->ball_velocity_y * 30 / 1000;

    /* Bounce if necessary */
    if (data->ball_x < 0) {
        data->ball_x = -data->ball_x;
        data->ball_velocity_x = -data->ball_velocity_x;
    }
    else if (data->ball_x >= 1024-128) {
        data->ball_x = (2*(1024-128)) - data->ball_x;
        data->ball_velocity_x = -data->ball_velocity_x;
    }

    if (data->ball_y < 0) {
        data->ball_y = -data->ball_y;
        data->ball_velocity_y = -data->ball_velocity_y;
    }
    else if (data->ball_y >= (768-128)) {
        data->ball_y = (2*(768-128)) - data->ball_y;
        data->ball_velocity_y = -data->ball_velocity_y;
    }

    guac_protocol_send_move(client->socket, data->ball,
            GUAC_DEFAULT_LAYER, data->ball_x, data->ball_y, 0);

    return 0;

}

