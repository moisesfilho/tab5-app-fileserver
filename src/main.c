/**
 * @file main.c
 * @brief Aplicativo Servidor de Arquivos HTTP/WebDAV Desacoplado para Tab5 OS
 */

#include "tab5_sdk.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static tab5_ui_obj_t s_status_badge = TAB5_UI_INVALID_OBJ;
static tab5_ui_obj_t s_status_label = TAB5_UI_INVALID_OBJ;
static tab5_ui_obj_t s_btn_toggle = TAB5_UI_INVALID_OBJ;
static tab5_ui_obj_t s_lbl_url = TAB5_UI_INVALID_OBJ;
static tab5_ui_obj_t s_lbl_ip = TAB5_UI_INVALID_OBJ;

static void update_server_view(void)
{
    bool running = tab5_fileserver_is_running();
    uint16_t port = tab5_fileserver_get_port();
    if (port == 0) {
        port = 8080;
    }

    tab5_wifi_info_t wifi = {0};
    tab5_system_get_wifi_status(&wifi);

    uint32_t pal_accent = tab5_ui_theme_get_color(TAB5_UI_COLOR_ACCENT);
    uint32_t pal_text_muted = tab5_ui_theme_get_color(TAB5_UI_COLOR_TEXT_MUTED);

    if (running) {
        if (s_status_badge != TAB5_UI_INVALID_OBJ) {
            tab5_ui_obj_set_style_bg(s_status_badge, 0x2ECC71, 255);
        }
        if (s_status_label != TAB5_UI_INVALID_OBJ) {
            tab5_ui_label_set_text(s_status_label, LV_SYMBOL_OK "  ONLINE / ATIVO");
            tab5_ui_obj_set_style_text_color(s_status_label, 0xFFFFFF, 255);
        }
        if (s_btn_toggle != TAB5_UI_INVALID_OBJ) {
            tab5_ui_label_set_text(s_btn_toggle, LV_SYMBOL_POWER "  Desligar Servidor");
            tab5_ui_obj_set_style_bg(s_btn_toggle, 0xE74C3C, 255);
            tab5_ui_obj_set_style_text_color(s_btn_toggle, 0xFFFFFF, 255);
        }
    } else {
        if (s_status_badge != TAB5_UI_INVALID_OBJ) {
            tab5_ui_obj_set_style_bg(s_status_badge, 0xE74C3C, 255);
        }
        if (s_status_label != TAB5_UI_INVALID_OBJ) {
            tab5_ui_label_set_text(s_status_label, LV_SYMBOL_CLOSE "  PARADO / INATIVO");
            tab5_ui_obj_set_style_text_color(s_status_label, 0xFFFFFF, 255);
        }
        if (s_btn_toggle != TAB5_UI_INVALID_OBJ) {
            tab5_ui_label_set_text(s_btn_toggle, LV_SYMBOL_PLAY "  Iniciar Servidor");
            tab5_ui_obj_set_style_bg(s_btn_toggle, pal_accent, 255);
            tab5_ui_obj_set_style_text_color(s_btn_toggle, 0xFFFFFF, 255);
        }
    }

    if (s_lbl_ip != TAB5_UI_INVALID_OBJ) {
        char ip_str[128];
        if (wifi.is_connected && wifi.ip_addr[0] != '\0') {
            snprintf(ip_str, sizeof(ip_str), "IP Local:  %s  (Rede: %s)",
                     wifi.ip_addr, wifi.ssid);
        } else {
            snprintf(ip_str, sizeof(ip_str),
                     "IP Local:  Desconectado (Sem rede Wi-Fi)");
        }
        tab5_ui_label_set_text(s_lbl_ip, ip_str);
    }

    if (s_lbl_url != TAB5_UI_INVALID_OBJ) {
        char url_str[128];
        if (running && wifi.is_connected && wifi.ip_addr[0] != '\0') {
            snprintf(url_str, sizeof(url_str), "http://%s:%u/", wifi.ip_addr,
                     (unsigned)port);
            tab5_ui_label_set_text(s_lbl_url, url_str);
            tab5_ui_obj_set_style_text_color(s_lbl_url, 0x3498DB, 255);
        } else if (running) {
            tab5_ui_label_set_text(s_lbl_url, "Aguardando conexao Wi-Fi...");
            tab5_ui_obj_set_style_text_color(s_lbl_url, pal_text_muted, 255);
        } else {
            tab5_ui_label_set_text(s_lbl_url, "Servidor desligado");
            tab5_ui_obj_set_style_text_color(s_lbl_url, pal_text_muted, 255);
        }
    }
}

static void on_toggle_server(void)
{
    if (tab5_fileserver_is_running()) {
        tab5_fileserver_stop();
        tab5_sound_play_beep(1000, 30);
        tab5_ui_show_toast("Servidor de arquivos desligado", 1500);
    } else {
        tab5_fileserver_start();
        tab5_sound_play_beep(1800, 50);
        tab5_ui_show_toast("Servidor de arquivos iniciado!", 1500);
    }
    update_server_view();
}

static void build_fileserver_ui(void)
{
    uint32_t pal_surface = tab5_ui_theme_get_color(TAB5_UI_COLOR_SURFACE);
    uint32_t pal_surface_alt = tab5_ui_theme_get_color(TAB5_UI_COLOR_SURFACE_ALT);
    uint32_t pal_border = tab5_ui_theme_get_color(TAB5_UI_COLOR_BORDER);
    uint32_t pal_text = tab5_ui_theme_get_color(TAB5_UI_COLOR_TEXT);
    uint32_t pal_text_muted = tab5_ui_theme_get_color(TAB5_UI_COLOR_TEXT_MUTED);

    tab5_ui_obj_t scr = tab5_ui_get_screen();

    // Contêiner raiz
    tab5_ui_obj_t main_cont = tab5_ui_container_create(scr);
    tab5_ui_obj_set_size(main_cont, TAB5_UI_PCT(100), TAB5_UI_SIZE_CONTENT);
    tab5_ui_obj_set_align(main_cont, TAB5_UI_ALIGN_TOP_MID, 0, 104);
    tab5_ui_obj_set_flex_flow(main_cont, TAB5_UI_FLEX_FLOW_COLUMN);
    tab5_ui_obj_set_style_bg(main_cont, 0, 0);
    tab5_ui_obj_set_style_border(main_cont, 0, 0);
    tab5_ui_obj_set_pad(main_cont, 14);
    tab5_ui_obj_set_gap(main_cont, 14);

    // 1. Card Status
    tab5_ui_obj_t card_status = tab5_ui_container_create(main_cont);
    tab5_ui_obj_set_size(card_status, TAB5_UI_PCT(100), TAB5_UI_SIZE_CONTENT);
    tab5_ui_obj_set_flex_flow(card_status, TAB5_UI_FLEX_FLOW_COLUMN);
    tab5_ui_obj_set_style_bg(card_status, pal_surface, 255);
    tab5_ui_obj_set_style_border(card_status, pal_border, 1);
    tab5_ui_obj_set_style_radius(card_status, 10);
    tab5_ui_obj_set_pad(card_status, 14);
    tab5_ui_obj_set_gap(card_status, 12);

    tab5_ui_obj_t lbl_card1 =
        tab5_ui_label_create(card_status, "STATUS DO SERVIDOR LOCAL");
    tab5_ui_obj_set_style_text_color(lbl_card1, pal_text, 255);

    // Linha de badge
    tab5_ui_obj_t badge_row = tab5_ui_container_create(card_status);
    tab5_ui_obj_set_size(badge_row, TAB5_UI_PCT(100), 40);
    tab5_ui_obj_set_flex_flow(badge_row, TAB5_UI_FLEX_FLOW_ROW);
    tab5_ui_obj_set_style_bg(badge_row, 0, 0);
    tab5_ui_obj_set_style_border(badge_row, 0, 0);
    tab5_ui_obj_set_pad(badge_row, 0);
    tab5_ui_obj_set_gap(badge_row, 10);

    s_status_badge = tab5_ui_container_create(badge_row);
    tab5_ui_obj_set_size(s_status_badge, 200, 36);
    tab5_ui_obj_set_style_radius(s_status_badge, 18);
    tab5_ui_obj_set_style_border(s_status_badge, 0, 0);
    tab5_ui_obj_set_pad(s_status_badge, 0);

    s_status_label = tab5_ui_label_create(s_status_badge, LV_SYMBOL_CLOSE "  PARADO");
    tab5_ui_obj_set_align(s_status_label, TAB5_UI_ALIGN_CENTER, 0, 0);
    tab5_ui_obj_set_style_text_color(s_status_label, 0xFFFFFF, 255);

    s_btn_toggle = tab5_ui_btn_create(card_status, LV_SYMBOL_PLAY "  Iniciar Servidor");
    tab5_ui_obj_set_size(s_btn_toggle, TAB5_UI_PCT(100), 48);
    tab5_ui_obj_set_style_radius(s_btn_toggle, 8);

    // 2. Card Rede & Acesso
    tab5_ui_obj_t card_net = tab5_ui_container_create(main_cont);
    tab5_ui_obj_set_size(card_net, TAB5_UI_PCT(100), TAB5_UI_SIZE_CONTENT);
    tab5_ui_obj_set_flex_flow(card_net, TAB5_UI_FLEX_FLOW_COLUMN);
    tab5_ui_obj_set_style_bg(card_net, pal_surface, 255);
    tab5_ui_obj_set_style_border(card_net, pal_border, 1);
    tab5_ui_obj_set_style_radius(card_net, 10);
    tab5_ui_obj_set_pad(card_net, 14);
    tab5_ui_obj_set_gap(card_net, 10);

    tab5_ui_obj_t lbl_card2 =
        tab5_ui_label_create(card_net, "ENDERECO DE ACESSO WEB / HTTP");
    tab5_ui_obj_set_style_text_color(lbl_card2, pal_text, 255);

    s_lbl_ip = tab5_ui_label_create(card_net, "IP Local: Verificando...");
    tab5_ui_obj_set_style_text_color(s_lbl_ip, pal_text_muted, 255);

    // Caixa de URL em destaque
    tab5_ui_obj_t url_box = tab5_ui_container_create(card_net);
    tab5_ui_obj_set_size(url_box, TAB5_UI_PCT(100), 44);
    tab5_ui_obj_set_style_bg(url_box, pal_surface_alt, 255);
    tab5_ui_obj_set_style_border(url_box, pal_border, 1);
    tab5_ui_obj_set_style_radius(url_box, 6);
    tab5_ui_obj_set_pad(url_box, 8);

    s_lbl_url = tab5_ui_label_create(url_box, "http://127.0.0.1:8080/");
    tab5_ui_obj_set_align(s_lbl_url, TAB5_UI_ALIGN_CENTER, 0, 0);

    tab5_ui_obj_t lbl_hint = tab5_ui_label_create(
        card_net,
        "Abra o navegador no seu computador ou smartphone conectado a mesma "
        "rede Wi-Fi para transferir, enviar e baixar arquivos do cartao "
        "microSD.");
    tab5_ui_obj_set_style_text_color(lbl_hint, pal_text_muted, 255);
}

static void app_init(void)
{
    tab5_system_log(2, "tab5_fileserver",
                    "Aplicativo Servidor de Arquivos desacoplado iniciado");
    tab5_ui_app_bar_set_title("Servidor de Arquivos");

    build_fileserver_ui();
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

TAB5_APP_EXPORT void tab5_app_on_theme_changed(bool dark)
{
    (void)dark;
    tab5_ui_clear_content();
    build_fileserver_ui();
    update_server_view();
}

TAB5_APP_EXPORT void tab5_app_on_ui_event(tab5_ui_obj_t obj,
                                          uint32_t event_type,
                                          int32_t event_val)
{
    (void)event_val;
    if (event_type != TAB5_UI_EVENT_CLICKED) {
        return;
    }

    if (obj == s_btn_toggle) {
        on_toggle_server();
    }
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
