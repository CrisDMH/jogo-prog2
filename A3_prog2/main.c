// COMPILAÇÃO: gcc main.c menu.c -o JOGO $(pkg-config --libs --cflags allegro-5 allegro_main-5 allegro_font-5 allegro_ttf-5 allegro_primitives-5 allegro_image-5 allegro_audio-5 allegro_acodec-5)
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <stdio.h>
#include <stdbool.h>

#include "menu.h"
#include "player.h"

int main() 
{
  al_init();
  al_install_keyboard();
  al_init_image_addon();
  al_init_font_addon();
  al_init_ttf_addon();
  al_install_audio();
  al_init_acodec_addon();
  al_reserve_samples(10);
  al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);

  ALLEGRO_DISPLAY *disp = al_create_display(1280, 800);
  ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
  al_register_event_source(queue, al_get_keyboard_event_source());
  al_register_event_source(queue, al_get_display_event_source(disp));

  ALLEGRO_BITMAP *background = al_load_bitmap("menu.png");
  ALLEGRO_BITMAP *opcoes_background = al_load_bitmap("menu_opcoes.png");

  ALLEGRO_FONT *fonte_titulo = al_load_ttf_font("FetteUNZFraktur.ttf", 100, 0);
  ALLEGRO_FONT *fonte_inicial = al_load_ttf_font("alagard.ttf", 70, 0);
  ALLEGRO_FONT *fonte_opcoes = al_load_ttf_font("alagard.ttf", 50, 0);
  
  ALLEGRO_BITMAP *mao = al_load_bitmap("mao.png");

  ALLEGRO_SAMPLE *musica_menu = al_load_sample("menu_theme.ogg");
  ALLEGRO_SAMPLE_ID musica_menu_id;

  bool musica_tocando = false;
  int opcao = 0;
  int rodando = 1;

  if (al_play_sample(musica_menu, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &musica_menu_id)) 
    musica_tocando = true;

  while (rodando) 
  {
    menu_aparencia(opcao, disp, background, fonte_titulo, fonte_inicial, mao);

    ALLEGRO_EVENT ev;
    al_wait_for_event(queue, &ev);

    if (ev.type == ALLEGRO_EVENT_KEY_DOWN) 
    {
      if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN) 
      {
        opcao = (opcao + 1) % 3;
      } 
      else if (ev.keyboard.keycode == ALLEGRO_KEY_UP) 
      {
        opcao = (opcao + 2) % 3;
      } 
      else if (ev.keyboard.keycode == ALLEGRO_KEY_ENTER) 
      {
        if (opcao == START) 
        {
          // Jogar
          // Aqui você chama o início do jogo
        }
        else if (opcao == OPTIONS) 
        {
          menu_opcoes(disp, opcoes_background, fonte_opcoes, queue);
        }
        else if (opcao == QUIT) 
        {
          rodando = 0; 
        }
      }
    }
    else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
    {
      rodando = 0;
    }
  }
  if (musica_tocando) 
  { 
    al_stop_sample(&musica_menu_id);
  }
  al_destroy_sample(musica_menu);

  al_destroy_bitmap(background);
  al_destroy_bitmap(opcoes_background);
  al_destroy_bitmap(mao);
  al_destroy_font(fonte_inicial);
  al_destroy_font(fonte_opcoes);
  al_destroy_display(disp);
  al_destroy_event_queue(queue);
  al_uninstall_audio();

  return 0;
}