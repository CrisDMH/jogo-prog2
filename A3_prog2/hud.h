#ifndef HUD_H
#define HUD_H

#include <allegro5/allegro.h>
#include "player.h"

void carregar_hud();

void desenhar_barra_vida(Player* player);

void destruir_hud();

void desenhar_balas(Player *player, ALLEGRO_BITMAP *icone_bala);

#endif