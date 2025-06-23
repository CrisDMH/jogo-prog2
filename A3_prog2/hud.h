#ifndef HUD_H
#define HUD_H

#include <allegro5/allegro.h>
#include "player.h"

void carregar_hud();

void desenhar_barra_vida(Player* player);

void destruir_hud();

void desenhar_balas(Player *player, ALLEGRO_BITMAP *icone_bala);

void desenhar_contador_inimigos(int inimigos_mortos);

void desenhar_seta_go(ALLEGRO_BITMAP *seta_go, ALLEGRO_FONT *fonte_go, bool mostrar);

void desenhar_barra_vida_boss(int vida_atual, int vida_max, const char* nome, ALLEGRO_FONT *fonte);

#endif