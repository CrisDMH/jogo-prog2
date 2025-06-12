#include "player.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro.h>

#define DIREITA 1
#define ESQUERDA -1

void inicializar_player(Player *player, ALLEGRO_BITMAP *sprite_andando, ALLEGRO_BITMAP *sprite_mirando, ALLEGRO_BITMAP *sprite_pulando)
{
    player->x = 100;
    player->y = CHAO_Y; // Posição inicial no "chão"
    player->vida = 100;
    player->ultimo_tiro = 0; // Pode atirar imediatamente
    player->escala = 0.55f;
    player->dy = 0;
    player->no_chao = true;

    player->spritesheet_andando = sprite_andando;
    player->spritesheet_atirando = sprite_mirando;
    player->spritesheet_pulando = sprite_pulando;

    player->frame_atual = 0;
    player->max_frames = 14; // spritesheet tem 7 frames de animação por linha
    player->tempo_frame = 0;
    player->duracao_frame = 0.1; // Cada frame dura 0.1 segundos
    player->andando = false;
    player->direcao = DIREITA; 
    
    player->frame_largura_andando = 200; 
    player->frame_altura_andando = 365; 
    
    player->frame_largura_mirando = 302;
    player->frame_altura_mirando = 367;

    player->frame_largura_pulo = 301;
    player->frame_altura_pulo = 367;

    player->linha_atual = 0;

    player->estado_atual = PARADO;

}

void inicializar_projeteis_player(ProjetilPlayer projeteis[]) {
    for (int i = 0; i < MAX_PROJETEIS_PLAYER; i++) {
        projeteis[i].ativo = false;
    }
}

void atirar_projetil_player(Player *player, ProjetilPlayer projeteis[])
{
    player->atirando = true;
    player->tempo_tiro = al_get_time();
    
    // Verifica o cooldown
    if (al_get_time() - player->ultimo_tiro < COOLDOWN_TIRO) {
        return; // Ainda não pode atirar
    }

    for (int i = 0; i < MAX_PROJETEIS_PLAYER; i++) {
        if (!projeteis[i].ativo) { // Acha um projétil inativo para usar
            projeteis[i].ativo = true;
            projeteis[i].x = (player->direcao == DIREITA) ? player->x + 50 : player->x; // Posição inicial do tiro (frente do player)
            projeteis[i].y = player->y + 25; // Meio do player
            projeteis[i].dx = (player->direcao == DIREITA) ? 30 : -30; // Velocidade do tiro
            
            player->ultimo_tiro = al_get_time(); // Atualiza o tempo do último tiro
            break; // Sai do loop depois de atirar um
        }
    }
}

void processar_input_player(Player *player, ALLEGRO_KEYBOARD_STATE *teclado, ProjetilPlayer projeteis[]) 
{
    bool movendo = al_key_down(teclado, ALLEGRO_KEY_LEFT) || al_key_down(teclado, ALLEGRO_KEY_RIGHT);
    bool atacando = al_key_down(teclado, ALLEGRO_KEY_X);
    bool pulando = al_key_down(teclado, ALLEGRO_KEY_UP);

    if (pulando && player->no_chao) {
        player->dy = FORCA_PULO;
        player->no_chao = false;
        // O estado será definido como PULANDO em atualizar_player
        return; // Sai da função para não processar outros inputs neste frame
    }

    // O estado de ATACANDO tem prioridade e não pode ser interrompido por movimento
    if (player->estado_atual != ATACANDO) {
        if (atacando) {
            player->estado_atual = ATACANDO;
            player->frame_atual = 0; // Reinicia a animação de ataque
            player->tempo_no_estado = 0; // Reinicia o timer do estado de ataque
            atirar_projetil_player(player, projeteis);
        } else if (movendo) {
            player->estado_atual = ANDANDO;
        } else {
            player->estado_atual = PARADO;
        }
    }

    if (al_key_down(teclado, ALLEGRO_KEY_LEFT)) 
      player->direcao = ESQUERDA;
    
    if (al_key_down(teclado, ALLEGRO_KEY_RIGHT)) 
      player->direcao = DIREITA;
}

void atualizar_player(Player *player, ALLEGRO_KEYBOARD_STATE *teclado) 
{
    player->dy += GRAVIDADE;
    player->y += player->dy;

    // Colisão com o chão e estado de pulo
    if (player->y >= CHAO_Y) {
        player->y = CHAO_Y;
        player->dy = 0;
        player->no_chao = true;
        // Se o estado era PULANDO, agora que está no chão, deve voltar a ser PARADO
        if (player->estado_atual == PULANDO) {
            player->estado_atual = PARADO;
        }
    } else {
        player->no_chao = false;
        // Força o estado PULANDO se estiver no ar (e não estiver atacando)
        if(player->estado_atual != ATACANDO) {
            player->estado_atual = PULANDO;
        }
    }
    // Acumula o tempo que passou (estamos em um timer de 60 FPS, então 1/60 de segundo)
    player->tempo_frame += 1.0 / 60.0;

    // Lógica baseada no estado atual
    switch (player->estado_atual) {
        case ANDANDO:
            // Move o personagem
            player->x += VELOCIDADE_PLAYER * player->direcao;
            
            // Anima o personagem usando TODOS OS 14 FRAMES
            if (player->tempo_frame >= player->duracao_frame) {
                player->tempo_frame -= player->duracao_frame;
                // O contador agora vai de 0 a 13 (total de 14 frames)
                player->frame_atual = (player->frame_atual + 1) % 14; 
            }
            break;

        case ATACANDO:
            player->tempo_no_estado += 1.0 / 60.0;
            
            // Anima o ataque usando os 3 FRAMES do outro spritesheet
            if (player->tempo_frame >= player->duracao_frame) {
                player->tempo_frame -= player->duracao_frame;
                // O contador vai de 0 a 2 (total de 3 frames)
                player->frame_atual = (player->frame_atual + 1) % 3;
            }

            // Verifica se o tempo da animação de ataque acabou
            if (player->tempo_no_estado > 0.3) {
                player->estado_atual = PARADO;
            }
            break;

        case PULANDO:
            // Animação do pulo
            if (player->tempo_frame >= player->duracao_frame) {
                if (player->frame_atual < 3) { // Animação de pulo tem 4 frames (0 a 3)
                    player->frame_atual++;
                }
                player->tempo_frame -= player->duracao_frame;
            }

            if (al_key_down(teclado, ALLEGRO_KEY_RIGHT)) {
                player->x += VELOCIDADE_PLAYER;
                player->direcao = DIREITA;
            }
            if (al_key_down(teclado, ALLEGRO_KEY_LEFT)) {
                player->x -= VELOCIDADE_PLAYER;
                player->direcao = ESQUERDA;
            }
            break; 

        case PARADO:
            // Quando parado, reseta para o primeiro frame da animação de andar
            player->frame_atual = 0;
            break;
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
    ALLEGRO_BITMAP *sheet_usado = NULL;
    int largura_frame = 0;
    int altura_frame = 0;
    int fx = 0, fy = 0; // Source X e Source Y no spritesheet

    // Decide qual spritesheet e dimensões usar baseado no estado
    switch (player->estado_atual) {
        case PARADO:
            sheet_usado = player->spritesheet_andando;
            largura_frame = player->frame_largura_andando;
            altura_frame = player->frame_altura_andando;
            // Para o estado parado, desenhamos o primeiro frame (coluna 0, linha 0)
            fx = 0;
            fy = 0;
            break;
        case ANDANDO:
            sheet_usado = player->spritesheet_andando;
            largura_frame = player->frame_largura_andando;
            altura_frame = player->frame_altura_andando;
            
            // --- CÁLCULO DA LINHA E COLUNA PARA ANDAR (14 frames) ---
            // O seu spritesheet tem 7 frames por linha.
            int coluna_andar = player->frame_atual % 7; // Módulo 7 nos dá a coluna (0 a 6)
            int linha_andar = player->frame_atual / 7;  // Divisão inteira por 7 nos dá a linha (0 ou 1)
            
            fx = coluna_andar * largura_frame;
            fy = linha_andar * altura_frame;
            break;
        case ATACANDO:
            sheet_usado = player->spritesheet_atirando;
            largura_frame = player->frame_largura_mirando;
            altura_frame = player->frame_altura_mirando;
            
            // A animação de ataque tem apenas 3 frames em uma única linha
            int coluna_ataque = player->frame_atual; // frame_atual aqui vai de 0 a 2
            
            fx = coluna_ataque * largura_frame;
            fy = 0; // Sempre a primeira (e única) linha
            break;
        case PULANDO: 
            sheet_usado = player->spritesheet_pulando;
            largura_frame = player->frame_largura_pulo;
            altura_frame = player->frame_altura_pulo;
            fx = player->frame_atual * largura_frame;
            break;
    }   

    if (sheet_usado) 
    {
        int flag = (player->direcao == -1) ? ALLEGRO_FLIP_HORIZONTAL : 0;
        float dx = player->x - camera_x;
        float dy = player->y; 
        float largura_desenho = largura_frame * player->escala;
        float altura_desenho = altura_frame * player->escala;

        al_draw_scaled_bitmap(sheet_usado, fx, fy, largura_frame, altura_frame,
                              dx, dy + 145, largura_desenho, altura_desenho, flag);
    }
}

void desenhar_projeteis_player(ProjetilPlayer projeteis[], float camera_x, ALLEGRO_BITMAP *flecha) {
    // --- CONTROLE DE TAMANHO ---
    // 1.0 = tamanho original, 2.0 = dobro do tamanho, etc.
    // Experimente valores como 1.5, 2.0, 2.5 para ver o que fica melhor.
    float escala_projetil = 1.5f;

    for (int i = 0; i < MAX_PROJETEIS_PLAYER; i++) {
        if (projeteis[i].ativo) {
            if (flecha) {
                float px = projeteis[i].x - camera_x;
                float py = projeteis[i].y;

                // Dimensões originais da imagem da flecha
                float largura_original = al_get_bitmap_width(flecha);
                float altura_original = al_get_bitmap_height(flecha);

                // Novas dimensões baseadas na escala
                float largura_escalada = largura_original * escala_projetil;
                float altura_escalada = altura_original * escala_projetil;
                
                // Posição para desenhar (centralizando o sprite escalado)
                float pos_x = px - (largura_escalada / 2.0f) + 50;
                float pos_y = py - (altura_escalada / 2.0f) + 180;

                // Desenha o bitmap com a nova escala
                int flag = (projeteis->dx  < 0) ? ALLEGRO_FLIP_HORIZONTAL : 0;
                al_draw_scaled_bitmap(flecha, 
                                      0, 0, largura_original, altura_original, // Região de origem (imagem inteira)
                                      pos_x, pos_y, largura_escalada, altura_escalada, // Destino e tamanho na tela
                                      flag); // Flags
            }
        }
    }
}
