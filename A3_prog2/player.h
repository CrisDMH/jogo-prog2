#ifndef PLAYER_H
#define PLAYER_H

#include <allegro5/allegro.h>
#include <stdbool.h>

// Constantes relacionadas ao jogador
#define VELOCIDADE_PLAYER 5
#define MAX_PROJETEIS_PLAYER 20
#define COOLDOWN_TIRO 0.2 // Tempo em segundos entre os tiros

// Struct para os projéteis do jogador
typedef struct {
    float x, y;
    float dx; // Projéteis do player geralmente só se movem em X
    bool ativo;
} ProjetilPlayer;

// Struct para o jogador
typedef struct {
    float x, y;
    int vida;
    double ultimo_tiro; // Para controlar o cooldown do tiro
    
    // --- VARIÁVEIS PARA ANIMAÇÃO ---
    ALLEGRO_BITMAP *sprite_sheet; // O spritesheet completo
    int frame_atual;          // O frame que estamos desenhando (ex: 0, 1, 2...)
    int max_frames;           // Quantos frames a animação tem
    double tempo_frame;       // Há quanto tempo o frame atual está na tela
    double duracao_frame;     // Quanto tempo cada frame deve durar
    bool andando;             // O jogador está se movendo?
    int direcao;              // Para qual lado está virado (1 = direita, -1 = esquerda)
    int frame_largura;        // Largura de um único frame
    int frame_altura;         // Altura de um único frame
    int linha_atual;          // Linha que está no spritesheet
} Player;

// --- Declarações das Funções ---
// Funções de inicialização
void inicializar_player(Player *player, ALLEGRO_BITMAP *sprite_andando);

void inicializar_projeteis_player(ProjetilPlayer projeteis[]);

// Funções chamadas a cada frame no loop do jogo
void processar_input_player(Player *player, ALLEGRO_KEYBOARD_STATE *teclado, ProjetilPlayer projeteis[]);

void atualizar_player(Player *player);

void atualizar_projeteis_player(ProjetilPlayer projeteis[], float camera_x);

void desenhar_player(Player *player, float camera_x);

void desenhar_projeteis_player(ProjetilPlayer projeteis[], float camera_x, ALLEGRO_BITMAP *flecha);

#endif // PLAYER_H
