#ifndef FASE1_H
#define FASE1_H

#include <allegro5/allegro.h>

#include "player.h"
#include "inimigos.h"

void verificar_colisoes(Player *player, Inimigo inimigos[], ProjetilPlayer projeteis_player[], ProjetilInimigo projeteis_inimigo[]);

void desenhar_debug_hitboxes(Player *player, Inimigo inimigos[], ProjetilPlayer projeteis_player[], ProjetilInimigo projeteis_inimigo[], float camera_x);

void iniciar_fase1(ALLEGRO_DISPLAY *display, ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_BITMAP *flecha, ALLEGRO_BITMAP *flecha_inimigo);

void encerrar_fase1();

#endif //FASE1_H