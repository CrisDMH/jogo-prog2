#ifndef INIMIGO_H
#define INIMIGO_H

#include <allegro5/allegro.h>
#include <stdbool.h>
#include "player.h" // Incluímos para ter acesso à struct do Player

#define MAX_INIMIGOS 10
#define VELOCIDADE_INIMIGO 2
#define DISTANCIA_ATAQUE 400 // Distância em pixels para o inimigo começar a atirar
#define MAX_PROJETEIS_INIMIGO 20
#define DANO_INIMIGO 15

typedef struct {
    float x, y;
    float dx, dy;
    bool ativo;
} ProjetilInimigo;

// Estados possíveis do inimigo
typedef enum {
    INIMIGO_PARADO,
    INIMIGO_ANDANDO,
    INIMIGO_ATACANDO,
    INIMIGO_ATINGIDO,
    INIMIGO_MORRENDO
} InimigoEstado;

// Estrutura de dados para o inimigo
typedef struct {
    float x, y;
    int vida;
    int direcao;
    bool ativo;

    InimigoEstado estado_atual;
    double tempo_no_estado;
    double tempo_ultimo_ataque;

    // Variáveis de animação
    int frame_atual;
    double tempo_frame;
    double duracao_frame;

    // Spritesheets do inimigo
    ALLEGRO_BITMAP *sheet_parado;
    ALLEGRO_BITMAP *sheet_andando;
    ALLEGRO_BITMAP *sheet_atirando;
    ALLEGRO_BITMAP *sheet_hit;
    ALLEGRO_BITMAP *sheet_morrendo;

    // Dimensões dos frames (ajuste conforme seus sprites)
    int largura_frame_parado, altura_frame_parado;
    int largura_frame_andando, altura_frame_andando;
    int largura_frame_atirando, altura_frame_atirando;
    int largura_frame_hit, altura_frame_hit;
    int largura_frame_morrendo, altura_frame_morrendo;

    float escala;

} Inimigo;

// Declarações das funções públicas do módulo do inimigo
void inimigo_carregar_sprites();

void inimigo_destruir_sprites();

void inicializar_inimigos(Inimigo inimigos[]);

void atualizar_inimigos(Inimigo inimigos[], Player *player, ProjetilInimigo projeteis[]);

void desenhar_inimigos(Inimigo inimigos[], float camera_x);

void inimigo_atirar(Inimigo *inimigo, ProjetilInimigo projeteis[]);

void inicializar_projeteis_inimigo(ProjetilInimigo projeteis[]);

void atualizar_projeteis_inimigo(ProjetilInimigo projeteis[]);

void desenhar_projeteis_inimigo(ProjetilInimigo projeteis[], float camera_x, ALLEGRO_BITMAP *sprite_flecha);

#endif 