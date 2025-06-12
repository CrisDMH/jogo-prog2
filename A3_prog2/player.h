#ifndef PLAYER_H
#define PLAYER_H

#include <allegro5/allegro.h>
#include <stdbool.h>

// Constantes relacionadas ao jogador
#define VELOCIDADE_PLAYER 5
#define MAX_PROJETEIS_PLAYER 20
#define COOLDOWN_TIRO 0.2 // Tempo em segundos entre os tiros
#define FORCA_PULO -20.0f // Para cima, então é negativo
#define GRAVIDADE 0.8f
#define CHAO_Y 600.0f    // Coordenada Y do chão

// Struct para os projéteis do jogador
typedef struct {
    float x, y;
    float dx; // Projéteis do player geralmente só se movem em X
    bool ativo;
} ProjetilPlayer;

typedef enum {
    PARADO,
    ANDANDO,
    ATACANDO,
    PULANDO
} player_estado;

// Struct para o jogador
typedef struct 
{
  float x, y;
  int vida;
  double ultimo_tiro; // Para controlar o cooldown do tiro
  bool atirando;
  double tempo_tiro;
  float escala;
  float dy; // velocidade vertical
  bool no_chao;

  player_estado estado_atual;
  double tempo_no_estado;
  int direcao;             

  double tempo_frame;       // Há quanto tempo o frame atual está na tela
  double duracao_frame;     // Quanto tempo cada frame deve durar
  int frame_atual;          // O frame que estamos desenhando (ex: 0, 1, 2...)
    
//---------------VARIÁVEIS PARA ANIMAÇÃO---------------------------------
  ALLEGRO_BITMAP *spritesheet_andando;
  ALLEGRO_BITMAP *spritesheet_atirando;
  ALLEGRO_BITMAP *spritesheet_pulando;

  int max_frames;           
  bool andando;   
  int frame_largura_andando;        
  int frame_altura_andando;         
  int frame_largura_mirando;
  int frame_altura_mirando; 
  int frame_largura_pulo; 
  int frame_altura_pulo;
  int linha_atual;   

} Player;

// --- Declarações das Funções ---
// Funções de inicialização
void inicializar_player(Player *player, ALLEGRO_BITMAP *sprite_andando, ALLEGRO_BITMAP *sprite_mirando, ALLEGRO_BITMAP *sprite_pulando);

void inicializar_projeteis_player(ProjetilPlayer projeteis[]);

// Funções chamadas a cada frame no loop do jogo
void processar_input_player(Player *player, ALLEGRO_KEYBOARD_STATE *teclado, ProjetilPlayer projeteis[]);

void atualizar_player(Player *player, ALLEGRO_KEYBOARD_STATE *teclado);

void atirar_projetil_player(Player *player, ProjetilPlayer projeteis[]);

void atualizar_projeteis_player(ProjetilPlayer projeteis[], float camera_x);

void desenhar_player(Player *player, float camera_x);

void desenhar_projeteis_player(ProjetilPlayer projeteis[], float camera_x, ALLEGRO_BITMAP *flecha);

#endif // PLAYER_H
