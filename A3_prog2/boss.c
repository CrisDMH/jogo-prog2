#include "boss.h"
#include <math.h> 

#define ESQUERDA -1
#define DIREITA 1
#define VELOCIDADE_BOSS_VOANDO 10
#define VELOCIDADE_PROJETIL_BOSS 25

static ALLEGRO_BITMAP *s_parado = NULL, *s_voando = NULL, *s_atacando = NULL,
                      *s_hit = NULL, *s_morrendo = NULL, *s_tiro = NULL;

void boss_carregar_sprites() {
    s_parado = al_load_bitmap("boss parado.png");
    s_voando = al_load_bitmap("boss voando.png");
    s_atacando = al_load_bitmap("boss atacando.png");
    s_hit = al_load_bitmap("boss hit.png");
    s_morrendo = al_load_bitmap("boss morrendo.png");
    s_tiro = al_load_bitmap("tiro_boss.png");
}

void boss_destruir_sprites() {
    al_destroy_bitmap(s_parado); al_destroy_bitmap(s_voando); al_destroy_bitmap(s_atacando);
    al_destroy_bitmap(s_hit); al_destroy_bitmap(s_morrendo); al_destroy_bitmap(s_tiro);
}

void boss_atirar(Boss *boss, ProjetilBoss projeteis[]) {
    for (int i = 0; i < MAX_PROJETEIS_BOSS; i++) {
        if (!projeteis[i].ativo) {
            projeteis[i].ativo = true;
            projeteis[i].x = boss->x;
            projeteis[i].y = boss->y + 100; 
            projeteis[i].dx = VELOCIDADE_PROJETIL_BOSS * boss->direcao;
            projeteis[i].dy = 0;
            return;
        }
    }
}

void inicializar_boss(Boss *boss, float arena_x, float arena_largura) {
    boss->ativo = true;
    boss->vida = VIDA_MAX_BOSS;
    boss->direcao = ESQUERDA;
    boss->x = arena_x + arena_largura - 300;
    boss->y = 600;
    boss->estado_atual = BOSS_PARADO;
    boss->estado_anterior = BOSS_PARADO;
    boss->tempo_no_estado = 0;
    boss->frame_atual = 0;
    boss->tempo_frame = 0;
    boss->duracao_frame = 0.15; 
    boss->escala = 3.0f; 

    boss->sheet_parado = s_parado; boss->sheet_voando = s_voando;
    boss->sheet_atacando = s_atacando; boss->sheet_hit = s_hit;
    boss->sheet_morrendo = s_morrendo; boss->sheet_tiro = s_tiro;

    boss->largura_frame_parado = 82;   boss->altura_frame_parado = 72;
    boss->largura_frame_voando = 82;   boss->altura_frame_voando = 72;
    boss->largura_frame_atacando = 82; boss->altura_frame_atacando = 72;
    boss->largura_frame_hit = 82;      boss->altura_frame_hit = 72;
    boss->largura_frame_morrendo = 82; boss->altura_frame_morrendo = 72;
}

void atualizar_boss(Boss *boss, Player *player, ProjetilBoss projeteis[]) {
    if (!boss->ativo) return;

    boss->tempo_no_estado += 1.0 / 60.0;
    boss->tempo_frame += 1.0 / 60.0;

    switch(boss->estado_atual) {
        case BOSS_PARADO:
        
            if (boss->tempo_frame >= boss->duracao_frame) {
                boss->frame_atual = (boss->frame_atual + 1) % 4;
                boss->tempo_frame = 0;
            }
            
            boss->y += sin(al_get_time() * 2.5) * 0.7;

         
            if (boss->tempo_no_estado > 2.0) {
                boss->estado_atual = BOSS_ATACANDO;
                boss->tempo_no_estado = 0;
                boss->frame_atual = 0;
            }
            break;

        case BOSS_ATACANDO:
         
            if (boss->tempo_frame >= boss->duracao_frame) {
                boss->frame_atual++;
                boss->tempo_frame = 0;

            
                if (boss->frame_atual == 4) {
                    boss_atirar(boss, projeteis);
                }

                if (boss->frame_atual >= 8) {
                 
                    boss->estado_atual = BOSS_VOANDO;
                    boss->tempo_no_estado = 0;
                    boss->frame_atual = 0;
                }
            }
            break;

        case BOSS_VOANDO:
        
            if (boss->tempo_frame >= boss->duracao_frame) {
                boss->frame_atual = (boss->frame_atual + 1) % 4;
                boss->tempo_frame = 0;
            }
           
            boss->x += VELOCIDADE_BOSS_VOANDO * boss->direcao;

          
            bool chegou = (boss->direcao == ESQUERDA && boss->x < 200) ||
                          (boss->direcao == DIREITA && boss->x > 1080);
            
            if (chegou) {
                boss->direcao *= -1; 
                boss->estado_atual = BOSS_PARADO;
                boss->tempo_no_estado = 0;
                boss->frame_atual = 0;
            }
            break;

        case BOSS_ATINGIDO:
            if (boss->tempo_frame >= 0.1) { 
                boss->frame_atual = (boss->frame_atual + 1) % 4;
                boss->tempo_frame = 0;
            }
            if (boss->tempo_no_estado > 0.4) {
                boss->estado_atual = boss->estado_anterior;
            }
            break;

        case BOSS_MORRENDO:
            if (boss->tempo_frame >= 0.2) { 
                if (boss->frame_atual < 5) { 
                    boss->frame_atual++;
                    boss->tempo_frame = 0;
                }
            }
            
            if (boss->tempo_no_estado > 2.0) {
                boss->ativo = false;
            }
            
            break;
    }
}

void desenhar_boss(Boss *boss) 
{
    if (!boss->ativo) return;

    ALLEGRO_BITMAP *sheet_atual = NULL;
    int largura_frame = 0, altura_frame = 0;

    switch(boss->estado_atual) 
    {
        case BOSS_PARADO:
            sheet_atual = boss->sheet_parado;
            largura_frame = boss->largura_frame_parado;
            altura_frame = boss->altura_frame_parado;
            
            break;
            
        case BOSS_VOANDO:
            sheet_atual = boss->sheet_voando;
            largura_frame = boss->largura_frame_voando;
            altura_frame = boss->altura_frame_voando;
            
            break;

        case BOSS_ATACANDO:
            sheet_atual = boss->sheet_atacando;
            largura_frame = boss->largura_frame_atacando - 1;
            altura_frame = boss->altura_frame_atacando;
            
            break;

        case BOSS_ATINGIDO:
            sheet_atual = boss->sheet_hit;
            largura_frame = boss->largura_frame_hit;
            altura_frame = boss->altura_frame_hit;
            
            break;

        case BOSS_MORRENDO:
            sheet_atual = boss->sheet_morrendo;
            largura_frame = boss->largura_frame_morrendo;
            altura_frame = boss->altura_frame_morrendo;
            
            break;
            
        default: return;
    }
    
    if (sheet_atual) {
        int fx = boss->frame_atual * largura_frame;
        int fy = 0;
        int flag = (boss->direcao == DIREITA) ? ALLEGRO_FLIP_HORIZONTAL : 0;

        al_draw_scaled_bitmap(sheet_atual, fx, fy, largura_frame, altura_frame,
                              boss->x, boss->y,
                              largura_frame * boss->escala, altura_frame * boss->escala,
                              flag);
    }
}

void inicializar_projeteis_boss(ProjetilBoss projeteis[])
{
  for (int i = 0; i < MAX_PROJETEIS_BOSS; i++) projeteis[i].ativo = false;
}

void atualizar_projeteis_boss(ProjetilBoss projeteis[]) 
{
    for (int i = 0; i < MAX_PROJETEIS_BOSS; i++) {
        if (projeteis[i].ativo) {
            projeteis[i].x += projeteis[i].dx;
            projeteis[i].y += projeteis[i].dy;
            
        }
    }
}

void desenhar_projeteis_boss(ProjetilBoss projeteis[], float camera_x) 
{
    float escala_projetil = 3.0f;

    for (int i = 0; i < MAX_PROJETEIS_BOSS; i++) {
        if (projeteis[i].ativo) {
           
            float largura_original = al_get_bitmap_width(s_tiro);
            float altura_original = al_get_bitmap_height(s_tiro);

            float largura_escalada = largura_original * escala_projetil;
            float altura_escalada = altura_original * escala_projetil;

            int flag = (projeteis[i].dx > 0) ? ALLEGRO_FLIP_HORIZONTAL : 0;

            al_draw_scaled_bitmap(s_tiro,
                                  0, 0, 
                                  largura_original, altura_original, 
                                  projeteis[i].x - camera_x, projeteis[i].y, 
                                  largura_escalada, altura_escalada, 
                                  flag); 
        }
    }
}
