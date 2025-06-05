#ifndef _DISPLAY_
#define _DISPLAY_

#define START 0
#define OPTIONS 1
#define QUIT 2 

void menu_aparencia(int opcoes, ALLEGRO_DISPLAY *disp, ALLEGRO_BITMAP *background,
                    ALLEGRO_FONT *fonte_titulo, ALLEGRO_FONT *fonte_itens, ALLEGRO_BITMAP *mao);

void menu_opcoes(ALLEGRO_DISPLAY *disp, ALLEGRO_BITMAP *opcoes_background, ALLEGRO_FONT *fonte, ALLEGRO_EVENT_QUEUE *queue);













#endif