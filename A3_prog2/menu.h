#ifndef _DISPLAY_
#define _DISPLAY_

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>

#define START 0
#define OPTIONS 1
#define QUIT 2 

void menu_aparencia(int opcoes, ALLEGRO_DISPLAY *disp, ALLEGRO_BITMAP *background,
                    ALLEGRO_FONT *fonte_titulo, ALLEGRO_FONT *fonte_itens, ALLEGRO_BITMAP *mao);

void menu_opcoes(ALLEGRO_DISPLAY *disp, ALLEGRO_BITMAP *opcoes_background, ALLEGRO_FONT *fonte, ALLEGRO_EVENT_QUEUE *queue,
                 ALLEGRO_SAMPLE_INSTANCE *musica_instancia, int *volume_atual);















#endif