#include "inimigos.h"
#include <stdio.h>
#include <math.h> 

#define ESQUERDA -1
#define DIREITA 1

#define LARGURA_NIVEL 5330

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

    for (int i = 0; i < MAX_INIMIGOS; i++) {
        inimigos[i].ativo = true;
        inimigos[i].x = 800 + i * 450;
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

        inimigos[i].patrol_x1 = inimigos[i].x - 200; 
        inimigos[i].patrol_x2 = inimigos[i].x + 200; 

    }
}

void atualizar_inimigos(Inimigo inimigos[], Player *player, ProjetilInimigo projeteis[]) 
{
    for (int i = 0; i < MAX_INIMIGOS; i++) {
        if (!inimigos[i].ativo) continue;

        Inimigo *inimigo = &inimigos[i];
        
        inimigo->tempo_frame += 1.0 / 60.0;
        inimigo->tempo_no_estado += 1.0 / 60.0;


        switch (inimigo->estado_atual) {
            
            case INIMIGO_PARADO:
                if (inimigo->tempo_frame >= inimigo->duracao_frame) {
                    inimigo->tempo_frame = 0;
                    inimigo->frame_atual = (inimigo->frame_atual + 1) % 6;
                }

                // Lógica de Decisão (IA)
                float distancia_x = fabs(player->x - inimigo->x);
                bool pode_atacar = (al_get_time() - inimigo->tempo_ultimo_ataque > 2.0);
                
                // Prioridade 1: Atacar o jogador
                if (distancia_x < DISTANCIA_ATAQUE && pode_atacar) {
                    // --- CORREÇÃO DE MIRA ---
                    // Antes de atacar, garante que está virado para o jogador.
                    if (player->x < inimigo->x) inimigo->direcao = ESQUERDA;
                    else inimigo->direcao = DIREITA;
                    
                    inimigo->estado_atual = INIMIGO_ATACANDO;
                    inimigo->frame_atual = 0;
                    inimigo->tempo_frame = 0;
                    inimigo->tempo_no_estado = 0;
                }
                // Prioridade 2: Se não puder atacar, começa a andar após esperar 2 segundos
                else if (inimigo->tempo_no_estado > 2.0) {
                    inimigo->estado_atual = INIMIGO_ANDANDO;
                    inimigo->tempo_no_estado = 0;
                }
                
                break;

            case INIMIGO_ANDANDO:
                inimigo->x += VELOCIDADE_INIMIGO * inimigo->direcao;
                if (inimigo->tempo_frame >= inimigo->duracao_frame) {
                    inimigo->tempo_frame = 0;
                    inimigo->frame_atual = (inimigo->frame_atual + 1) % 8;
                }
                
                // APRIMORAMENTO DA IA: Checa se deve parar para atacar
                float distancia_x_andando = fabs(player->x - inimigo->x);
                bool pode_atacar_andando = (al_get_time() - inimigo->tempo_ultimo_ataque > 2.0);
                if (distancia_x_andando < DISTANCIA_ATAQUE && pode_atacar_andando) {

                    inimigo->estado_atual = INIMIGO_PARADO;
                    inimigo->tempo_no_estado = 0; // Zera o timer de espera para que ele possa decidir atacar rápido
                }
                // Se não viu o jogador, continua a patrulha normal
                else if (inimigo->x <= inimigo->patrol_x1 || inimigo->x >= inimigo->patrol_x2) {
                    inimigo->direcao *= -1; 
                    inimigo->estado_atual = INIMIGO_PARADO;
                    inimigo->tempo_no_estado = 0;
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
                        inimigo->tempo_no_estado = 0;
                    }
                }
                break;

            case INIMIGO_ATINGIDO: 

                if(inimigo->tempo_frame >= 0.1) { // Animação de hit mais rápida
                    inimigo->tempo_frame = 0;
                    inimigo->frame_atual = (inimigo->frame_atual + 1) % 2;
                }
                // Após 0.3 segundos no estado, volta a ficar parado
                if (inimigo->tempo_no_estado > 0.3) {
                    inimigo->estado_atual = INIMIGO_PARADO;
                    inimigo->tempo_no_estado = 0;
                }
                break;                

            case INIMIGO_MORRENDO:
                
                // Toca a animação de morte (5 frames) uma única vez
                if (inimigo->tempo_frame >= 0.15 && inimigo->frame_atual < 4) { // Animação mais lenta
                    inimigo->tempo_frame = 0;
                    inimigo->frame_atual++;
                }

                // Após 3 segundos, desativa o inimigo permanentemente
                if (inimigo->tempo_no_estado > 3.0) {
                    inimigo->ativo = false;
                }
                
                break;
        }

      if (inimigo->x < 0) { inimigo->x = 0; inimigo->direcao = 1; }
      if (inimigo->x > LARGURA_NIVEL) { inimigo->x = LARGURA_NIVEL; inimigo->direcao = -1; }    

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

            case INIMIGO_ANDANDO:
                sheet_usado = inimigo->sheet_andando;
                largura_frame_atual = inimigo->largura_frame_andando;
                altura_frame_atual = inimigo->altura_frame_andando;
                fx = inimigo->frame_atual * largura_frame_atual;
                fy = 0;
                
                break; 

            case INIMIGO_ATACANDO:
                sheet_usado = inimigo->sheet_atirando;
                largura_frame_atual = inimigo->largura_frame_atirando;
                altura_frame_atual = inimigo->altura_frame_atirando;
                
              
                deslocamento_x = -65.0f; 
                deslocamento_y = 10.0f;  

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

                deslocamento_x = -140.0f;
                deslocamento_y = 90;
                fx = inimigo->frame_atual * largura_frame_atual;
                fy = 0;
                break;
                
            case INIMIGO_MORRENDO:
                sheet_usado = inimigo->sheet_morrendo;
                largura_frame_atual = inimigo->largura_frame_morrendo;
                altura_frame_atual = inimigo->altura_frame_morrendo;

                deslocamento_x = -140.0f;
                deslocamento_y = 90.0f;
                fx = inimigo->frame_atual * largura_frame_atual;
                fy = 0;
                break;

        }
        

        if (sheet_usado) {
            float largura_desenho = largura_frame_atual * inimigo->escala;
            float altura_desenho = altura_frame_atual * inimigo->escala;


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
            projeteis[i].y = (inimigo->y - (inimigo->altura_frame_atirando * inimigo->escala / 2));
            
            projeteis[i].dx = 15.0f * inimigo->direcao;

            projeteis[i].dy = 0; 
            return;
        }
    }
}


void atualizar_projeteis_inimigo(ProjetilInimigo projeteis[]) {
    for (int i = 0; i < MAX_PROJETEIS_INIMIGO; i++) {
        if (projeteis[i].ativo) {
 
            projeteis[i].x += projeteis[i].dx;
            projeteis[i].y += projeteis[i].dy; // dy agora é sempre 0

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