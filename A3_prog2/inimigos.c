#include "inimigos.h"
#include <stdio.h>
#include <math.h> 

#define ESQUERDA -1
#define DIREITA 1

// Ponteiros estáticos para os sprites, para carregar apenas uma vez
static ALLEGRO_BITMAP *sprite_parado = NULL;
static ALLEGRO_BITMAP *sprite_andando = NULL;
static ALLEGRO_BITMAP *sprite_atirando = NULL;
static ALLEGRO_BITMAP *sprite_hit = NULL;
static ALLEGRO_BITMAP *sprite_morrendo = NULL;

void inimigo_carregar_sprites() 
{
  sprite_parado = al_load_bitmap("inimigo parado.png");
  sprite_andando = al_load_bitmap("inimigo andando.png");
  sprite_atirando = al_load_bitmap("inimigo atirando.png");
  sprite_hit = al_load_bitmap("inimigo hit.png");
  sprite_morrendo = al_load_bitmap("inimigo morrendo.png");
}

void inimigo_destruir_sprites() 
{
  al_destroy_bitmap(sprite_parado);
  al_destroy_bitmap(sprite_andando);
  al_destroy_bitmap(sprite_atirando);
  al_destroy_bitmap(sprite_hit);
  al_destroy_bitmap(sprite_morrendo);
}

void inicializar_inimigos(Inimigo inimigos[]) 
{
    for (int i = 0; i < MAX_INIMIGOS; i++) {
        inimigos[i].ativo = false;
    }

    for (int i = 0; i < 2; i++) {
        inimigos[i].ativo = true;
        inimigos[i].x = 800 + i * 700;
        inimigos[i].y = CHAO_Y + 355; 
        inimigos[i].vida = 100;
        inimigos[i].direcao = ESQUERDA;
        inimigos[i].estado_atual = INIMIGO_PARADO;
        inimigos[i].tempo_no_estado = 0;
        inimigos[i].tempo_ultimo_ataque = 0;
        inimigos[i].frame_atual = 0;
        inimigos[i].tempo_frame = 0;
        inimigos[i].duracao_frame = 0.1;

        inimigos[i].sheet_parado = sprite_parado;
        inimigos[i].sheet_andando = sprite_andando;
        inimigos[i].sheet_atirando = sprite_atirando;
        inimigos[i].sheet_hit = sprite_hit;
        inimigos[i].sheet_morrendo = sprite_morrendo;

        inimigos[i].largura_frame_parado = 250;     inimigos[i].altura_frame_parado = 365;
        inimigos[i].largura_frame_andando = 250;    inimigos[i].altura_frame_andando = 365;
        inimigos[i].largura_frame_atirando = 500;   inimigos[i].altura_frame_atirando = 510;
        inimigos[i].largura_frame_hit = 500;        inimigos[i].altura_frame_hit = 500;
        inimigos[i].largura_frame_morrendo = 500;   inimigos[i].altura_frame_morrendo = 500;

        inimigos[i].escala = 0.64f;
    }
}

void atualizar_inimigos(Inimigo inimigos[], Player *player, ProjetilInimigo projeteis[]) 
{
    for (int i = 0; i < MAX_INIMIGOS; i++) {
        if (!inimigos[i].ativo) continue;

        Inimigo *inimigo = &inimigos[i];
        
        inimigo->tempo_frame += 1.0 / 60.0;

        switch (inimigo->estado_atual) {
            
            case INIMIGO_PARADO:
                // Animação de Parado
                if (inimigo->tempo_frame >= inimigo->duracao_frame) {
                    inimigo->tempo_frame = 0;
                    // --- CORREÇÃO AQUI ---
                    // Mudado de % 7 para % 6 para corresponder aos 6 frames da sua sprite.
                    inimigo->frame_atual = (inimigo->frame_atual + 1) % 6;
                }

                // Lógica de Decisão (IA)
                float distancia_x = fabs(player->x - inimigo->x);
                bool pode_atacar = (al_get_time() - inimigo->tempo_ultimo_ataque > 2.0);

                if (player->x < inimigo->x) inimigo->direcao = ESQUERDA;
                else inimigo->direcao = DIREITA;
                
                if (distancia_x < DISTANCIA_ATAQUE && pode_atacar) {
                    inimigo->estado_atual = INIMIGO_ATACANDO;
                    inimigo->frame_atual = 0;
                    inimigo->tempo_frame = 0;
                }
                break;

            case INIMIGO_ATACANDO:
                // Animação de Ataque
                if (inimigo->tempo_frame >= inimigo->duracao_frame) {
                    inimigo->tempo_frame = 0;
                    inimigo->frame_atual++;
                    
                if (inimigo->frame_atual == 12)
                {
                  inimigo_atirar(inimigo, projeteis);
                }
                    
                    if (inimigo->frame_atual >= 15) {
                        inimigo->estado_atual = INIMIGO_PARADO;
                        inimigo->frame_atual = 0;
                        inimigo->tempo_ultimo_ataque = al_get_time();
                    }
                }
                break;

            case INIMIGO_ATINGIDO:
                inimigo->tempo_no_estado += 1.0/60.0;    

                if(inimigo->tempo_frame >= 0.1) { // Animação de hit mais rápida
                    inimigo->tempo_frame = 0;
                    inimigo->frame_atual = (inimigo->frame_atual + 1) % 2;
                }
                // Após 0.3 segundos no estado, volta a ficar parado
                if (inimigo->tempo_no_estado > 0.3) {
                    inimigo->estado_atual = INIMIGO_PARADO;
                }
                break;                
            
            default:
                inimigo->estado_atual = INIMIGO_PARADO;
                inimigo->frame_atual = 0;
                break;
        }
    }
}

void desenhar_inimigos(Inimigo inimigos[], float camera_x) {
    for (int i = 0; i < MAX_INIMIGOS; i++) {
        if (!inimigos[i].ativo) continue;

        Inimigo *inimigo = &inimigos[i];
        ALLEGRO_BITMAP *sheet_usado = NULL;
        int largura_frame_atual = 0;
        int altura_frame_atual = 0;
        float fx = 0, fy = 0;
        
        // 1. Declaramos as variáveis de deslocamento, ambas começando em zero.
        float deslocamento_x = 0;
        float deslocamento_y = 0;

        switch (inimigo->estado_atual) {
            case INIMIGO_PARADO:
                sheet_usado = inimigo->sheet_parado;
                largura_frame_atual = inimigo->largura_frame_parado;
                altura_frame_atual = inimigo->altura_frame_parado;
                
                fx = inimigo->frame_atual * largura_frame_atual;
                fy = 0;
                break;

            case INIMIGO_ATACANDO:
                sheet_usado = inimigo->sheet_atirando;
                largura_frame_atual = inimigo->largura_frame_atirando;
                altura_frame_atual = inimigo->altura_frame_atirando;
                
                // 2. Definimos ambos os deslocamentos apenas para o estado de ataque.
                deslocamento_x = -65.0f; // O valor perfeito que você encontrou.
                deslocamento_y = 10.0f;  // Um valor inicial para o deslocamento para baixo.

                int frames_por_linha = 4;
                int coluna = inimigo->frame_atual % frames_por_linha;
                int linha = inimigo->frame_atual / frames_por_linha;

                fx = coluna * largura_frame_atual;
                fy = linha * altura_frame_atual;
                break;

            case INIMIGO_ATINGIDO:
                sheet_usado = inimigo->sheet_hit;
                largura_frame_atual = inimigo->largura_frame_hit;
                altura_frame_atual = inimigo->altura_frame_hit;
                // A animação de hit provavelmente usa o mesmo deslocamento do ataque
                deslocamento_x = -140.0f;
                deslocamento_y = 90;
                fx = inimigo->frame_atual * largura_frame_atual;
                fy = 0;
                break;
                
        }
        

        if (sheet_usado) {
            float largura_desenho = largura_frame_atual * inimigo->escala;
            float altura_desenho = altura_frame_atual * inimigo->escala;

            // 3. Adicionamos ambos os deslocamentos aos cálculos de posição final.
            float dx = inimigo->x - camera_x + deslocamento_x;
            float dy = inimigo->y - altura_desenho + deslocamento_y;

            int flag = (inimigo->direcao == ESQUERDA) ? ALLEGRO_FLIP_HORIZONTAL : 0;
            
            al_draw_scaled_bitmap(sheet_usado, fx, fy, largura_frame_atual, altura_frame_atual,
                                  dx, dy, largura_desenho, altura_desenho, flag);
        }
    }
}

void inicializar_projeteis_inimigo(ProjetilInimigo projeteis[]) {
    for (int i = 0; i < MAX_PROJETEIS_INIMIGO; i++) {
        projeteis[i].ativo = false;
    }
}

void inimigo_atirar(Inimigo *inimigo, ProjetilInimigo projeteis[]) {
    for (int i = 0; i < MAX_PROJETEIS_INIMIGO; i++) {
        if (!projeteis[i].ativo) {
            projeteis[i].ativo = true;
            projeteis[i].x = inimigo->x;
            // Ajusta a altura inicial do tiro para o centro do sprite
            projeteis[i].y = (inimigo->y - (inimigo->altura_frame_atirando * inimigo->escala / 2));
            
            projeteis[i].dx = 15.0f * inimigo->direcao;
            // --- CORREÇÃO AQUI ---
            // A velocidade vertical inicial agora é 0 para um tiro reto.
            projeteis[i].dy = 0; 
            return;
        }
    }
}


void atualizar_projeteis_inimigo(ProjetilInimigo projeteis[]) {
    for (int i = 0; i < MAX_PROJETEIS_INIMIGO; i++) {
        if (projeteis[i].ativo) {
            // --- CORREÇÃO AQUI ---
            // Removemos a linha que adicionava gravidade ao projétil.
            // projeteis[i].dy += 0.5f; 
            
            projeteis[i].x += projeteis[i].dx;
            projeteis[i].y += projeteis[i].dy; // dy agora é sempre 0

            // (lógica de desativar o projétil permanece igual)
            // ...
        }
    }
}

void desenhar_projeteis_inimigo(ProjetilInimigo projeteis[], float camera_x, ALLEGRO_BITMAP *sprite_flecha) {
    for (int i = 0; i < MAX_PROJETEIS_INIMIGO; i++) {
        if (projeteis[i].ativo) {
            if (sprite_flecha) {
                // Desenha a flecha na posição correta
                al_draw_bitmap(sprite_flecha, projeteis[i].x - camera_x, projeteis[i].y - 70, 0);
            }
        }
    }
}