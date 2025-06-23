#ifndef BOSS_H
#define BOSS_H

#include <allegro5/allegro.h>
#include "player.h"

#define VIDA_MAX_BOSS 1000
#define MAX_PROJETEIS_BOSS 10

typedef enum {
    BOSS_PARADO,
    BOSS_VOANDO,
    BOSS_ATACANDO,
    BOSS_ATINGIDO,
    BOSS_MORRENDO
} BossEstado;

typedef struct {
    float x, y;
    float dx, dy;
    bool ativo;
} ProjetilBoss;

typedef struct {
    float x, y;
    int vida;
    int direcao;
    bool ativo;

    BossEstado estado_atual;
    BossEstado estado_anterior;
    double tempo_no_estado;
    float target_x; 

    
    int frame_atual;
    double tempo_frame;
    double duracao_frame;

    ALLEGRO_BITMAP *sheet_parado;
    ALLEGRO_BITMAP *sheet_voando;
    ALLEGRO_BITMAP *sheet_atacando;
    ALLEGRO_BITMAP *sheet_hit;
    ALLEGRO_BITMAP *sheet_morrendo;
    ALLEGRO_BITMAP *sheet_tiro;

    int largura_frame_parado, altura_frame_parado;
    int largura_frame_voando, altura_frame_voando;
    int largura_frame_atacando, altura_frame_atacando;
    int largura_frame_hit, altura_frame_hit;
    int largura_frame_morrendo, altura_frame_morrendo;

    float escala;

} Boss;

// --- Funções Públicas ---
void boss_carregar_sprites();

void boss_destruir_sprites();

void inicializar_boss(Boss *boss, float arena_x, float arena_largura);

void atualizar_boss(Boss *boss, Player *player, ProjetilBoss projeteis[]);

void desenhar_boss(Boss *boss);

void inicializar_projeteis_boss(ProjetilBoss projeteis[]);

void atualizar_projeteis_boss(ProjetilBoss projeteis[]);

void desenhar_projeteis_boss(ProjetilBoss projeteis[], float camera_x);

#endif