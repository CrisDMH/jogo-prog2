#ifndef FASE1_H
#define FASE1_H

#include <allegro5/allegro.h>

#include "player.h"
#include "inimigos.h"
#include "boss.h"

bool verificar_colisoes(Player *player, Inimigo inimigos[], ProjetilPlayer projeteis_player[], ProjetilInimigo projeteis_inimigo[],
                        Boss *boss, ProjetilBoss projeteis_boss[], float camera_x, int *inimigos_mortos_cont);

void desenhar_debug_hitboxes(Player *player, Inimigo inimigos[], ProjetilPlayer projeteis_player[], ProjetilInimigo projeteis_inimigo[],
                             Boss *boss, ProjetilBoss projeteis_boss[], float camera_x);

void iniciar_fase1(ALLEGRO_DISPLAY *display, ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_BITMAP *flecha, ALLEGRO_BITMAP *flecha_inimigo);

void encerrar_fase1();

bool mostrar_tela_instrucoes(ALLEGRO_DISPLAY *display, ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_FONT *fonte_titulo, ALLEGRO_FONT *fonte_texto);

#endif //FASE1_H