/**
 * @file main.c
 * @brief Aplicativo Servidor de Arquivos HTTP/WebDAV para Tab5 OS
 */

#include "tab5_sdk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool s_server_running = false;

static void update_server_view(void)
{
    tab5_wifi_info_t info = {0};
    tab5_system_get_wifi_status(&info);

    char buf[512];
    if (s_server_running) {
        snprintf(buf, sizeof(buf),
                 "====================================\n"
                 "    SERVIDOR DE ARQUIVOS LOCAL     \n"
                 "====================================\n\n"
                 " Status:    ONLINE / ATIVO\n"
                 " Porta:     8080 (HTTP / WebDAV)\n"
                 " IP Local:  %s\n"
                 " URL:       http://%s:8080/\n\n"
                 " Acesse o endereco acima em seu navegador\n"
                 " no mesmo Wi-Fi para transferir arquivos.\n",
                 info.is_connected ? info.ip_addr : "127.0.0.1",
                 info.is_connected ? info.ip_addr : "127.0.0.1");
    } else {
        snprintf(buf, sizeof(buf),
                 "====================================\n"
                 "    SERVIDOR DE ARQUIVOS LOCAL     \n"
                 "====================================\n\n"
                 " Status:    PARADO\n"
                 " IP Local:  %s\n\n"
                 " Pressione 'Iniciar Servidor' para\n"
                 " habilitar o compartilhamento via Wi-Fi.\n",
                 info.is_connected ? info.ip_addr : "Sem conexao");
    }

    tab5_ui_obj_t ta = tab5_ui_get_main_textarea();
    if (ta != NULL) {
        tab5_ui_textarea_set_text(ta, buf);
    }
}

static void on_toggle_server(void *user_data)
{
    (void)user_data;
    s_server_running = !s_server_running;
    tab5_sound_play_beep(s_server_running ? 1500 : 800, 40);
    update_server_view();
    tab5_ui_show_toast(s_server_running ? "Servidor iniciado em :8080" : "Servidor parado", 1500);
}

static void app_init(void)
{
    tab5_system_log(2, "tab5_fileserver", "Aplicativo Servidor iniciado");
    tab5_ui_app_bar_set_title("Servidor de Arquivos");
    tab5_ui_app_bar_add_action_button("LV_SYMBOL_PLAY", on_toggle_server, NULL);
    update_server_view();
}

static void app_resume(void)
{
    tab5_system_log(2, "tab5_fileserver", "Servidor retomado");
    update_server_view();
}

static void app_pause(void)
{
    tab5_system_log(2, "tab5_fileserver", "Servidor pausado");
}

static void app_destroy(void)
{
    tab5_system_log(2, "tab5_fileserver", "Servidor finalizado");
}

TAB5_APP_EXPORT int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    tab5_lifecycle_callbacks_t cbs = {
        .on_init = app_init,
        .on_resume = app_resume,
        .on_pause = app_pause,
        .on_destroy = app_destroy,
        .on_open_file = NULL,
    };

    tab5_lifecycle_register(&cbs);
    app_init();
    return 0;
}
