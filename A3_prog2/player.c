#include "player.h"
#include <allegro5/allegro_primitives.h>

void inicializar_player(Player *player) {
    player->x = 100;
    player->y = 600; // Posição inicial no "chão"
    player->vida = 100;
    player->ultimo_tiro = 0; // Pode atirar imediatamente
}

void inicializar_projeteis_player(ProjetilPlayer projeteis[]) {
    for (int i = 0; i < MAX_PROJETEIS_PLAYER; i++) {
        projeteis[i].ativo = false;
    }
}

void atirar_projetil_player(Player *player, ProjetilPlayer projeteis[]) {
    // Verifica o cooldown
    if (al_get_time() - player->ultimo_tiro < COOLDOWN_TIRO) {
        return; // Ainda não pode atirar
    }

    for (int i = 0; i < MAX_PROJETEIS_PLAYER; i++) {
        if (!projeteis[i].ativo) { // Acha um projétil inativo para usar
            projeteis[i].ativo = true;
            projeteis[i].x = player->x + 50; // Posição inicial do tiro (frente do player)
            projeteis[i].y = player->y + 25; // Meio do player
            projeteis[i].dx = 15; // Velocidade do tiro
            
            player->ultimo_tiro = al_get_time(); // Atualiza o tempo do último tiro
            break; // Sai do loop depois de atirar um
        }
    }
}

void processar_input_player(Player *player, ALLEGRO_KEYBOARD_STATE *teclado, ProjetilPlayer projeteis[]) {
    if (al_key_down(teclado, ALLEGRO_KEY_RIGHT)) {
        player->x += VELOCIDADE_PLAYER;
    }
    if (al_key_down(teclado, ALLEGRO_KEY_LEFT)) {
        player->x -= VELOCIDADE_PLAYER;
    }
    // Lógica de pulo viria aqui
    if (al_key_down(teclado, ALLEGRO_KEY_X)) { // Usando 'X' para atirar, por exemplo
        atirar_projetil_player(player, projeteis);
    }
}

void atualizar_projeteis_player(ProjetilPlayer projeteis[], float camera_x) {
    for (int i = 0; i < MAX_PROJETEIS_PLAYER; i++) {
        if (projeteis[i].ativo) {
            projeteis[i].x += projeteis[i].dx;
            // Desativar projétil se sair da área visível da câmera
            if (projeteis[i].x < camera_x || projeteis[i].x > camera_x + 1280) { // 1280 é a largura da tela
                projeteis[i].ativo = false;
            }
        }
    }
}

void desenhar_player(Player *player, float camera_x) {
    // Desenha o player (um retângulo verde) relativo à câmera
    al_draw_filled_rectangle(player->x - camera_x, player->y, 
                             player->x - camera_x + 50, player->y + 50, 
                             al_map_rgb(0, 255, 0));
}

void desenhar_projeteis_player(ProjetilPlayer projeteis[], float camera_x) {
    for (int i = 0; i < MAX_PROJETEIS_PLAYER; i++) {
        if (projeteis[i].ativo) {
            al_draw_filled_circle(projeteis[i].x - camera_x, projeteis[i].y, 5, al_map_rgb(255, 255, 0));
        }
    }
}