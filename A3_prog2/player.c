#include "player.h"
#include <allegro5/allegro_primitives.h>

#define DIREITA 1
#define ESQUERDA -1

void inicializar_player(Player *player, ALLEGRO_BITMAP *sprite_andando) {
    player->x = 100;
    player->y = 600; // Posição inicial no "chão"
    player->vida = 100;
    player->ultimo_tiro = 0; // Pode atirar imediatamente

    player->sprite_sheet = sprite_andando;
    player->frame_atual = 0;
    player->max_frames = 14; 
    player->tempo_frame = 0;
    player->duracao_frame = 0.1; // Cada frame dura 0.1 segundos
    player->andando = false;
    player->direcao = DIREITA; 
    player->frame_largura = 200; 
    player->frame_altura = 368; 
    player->linha_atual = 0;
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

void processar_input_player(Player *player, ALLEGRO_KEYBOARD_STATE *teclado, ProjetilPlayer projeteis[]) 
{
    player->andando = false; // parado no inicio do frame

    if (al_key_down(teclado, ALLEGRO_KEY_RIGHT)) {
        player->andando = true;
        player->direcao = DIREITA;
    }
    if (al_key_down(teclado, ALLEGRO_KEY_LEFT)) {
        player->andando = true;
        player->direcao = ESQUERDA;
    }
    
    if (al_key_down(teclado, ALLEGRO_KEY_X)) {
        atirar_projetil_player(player, projeteis);
    }
}

void atualizar_player(Player *player) {
    // 1. Mover o jogador baseado no seu estado
    if (player->andando) {
        if (player->direcao == 1) {
            player->x += VELOCIDADE_PLAYER;
        } else if (player->direcao == -1) {
            player->x -= VELOCIDADE_PLAYER;
        }
    }

    // 2. Atualizar o frame da animação baseado no tempo
    if (player->andando) {
        // Acumula o tempo que passou (estamos em um timer de 60 FPS, então 1/60 de segundo)
        player->tempo_frame += 1.0 / 60.0;

        // Se o tempo acumulado for maior que a duração de um frame...
        if (player->tempo_frame >= player->duracao_frame) {
            // ...avança para o próximo frame
            player->frame_atual = (player->frame_atual + 1) % player->max_frames;
            // E reseta o contador de tempo do frame
            player->tempo_frame -= player->duracao_frame;
        }
    } else {
        // Se o jogador está parado, volta para o primeiro frame
        player->frame_atual = 0;
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

void desenhar_player(Player *player, float camera_x) 
{
    if (player->sprite_sheet) {
        int coluna = player->frame_atual % 7;
        int linha = player->frame_atual / 7;

        int fx = coluna * player->frame_largura;
        int fy = linha * player->frame_altura;
        
        // Flag para inverter a imagem se estiver andando para a esquerda
        int flag = (player->direcao == -1) ? ALLEGRO_FLIP_HORIZONTAL : 0;
        
        al_draw_bitmap_region(player->sprite_sheet, fx, fy, 
                              player->frame_largura, player->frame_altura,
                              player->x - camera_x, player->y, flag);
    }
}

void desenhar_projeteis_player(ProjetilPlayer projeteis[], float camera_x, ALLEGRO_BITMAP *flecha) {
    for (int i = 0; i < MAX_PROJETEIS_PLAYER; i++) {
      if (projeteis[i].ativo) {
            // Substituímos o círculo pela imagem
            if (flecha) {
                // Desenhamos o sprite centralizado na posição do projétil
                float px = projeteis[i].x - camera_x;
                float py = projeteis[i].y;
                float sw = al_get_bitmap_width(flecha);
                float sh = al_get_bitmap_height(flecha);
                al_draw_bitmap(flecha, px - sw / 2, py - sh / 2, 0);
            }
        }
    }
}