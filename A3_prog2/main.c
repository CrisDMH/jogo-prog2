// COMPILAÇÃO: gcc main.c menu.c player.c fase1.c -o JOGO $(pkg-config --libs --cflags allegro-5 allegro_main-5 allegro_font-5 allegro_ttf-5 allegro_primitives-5 allegro_image-5 allegro_audio-5 allegro_acodec-5)
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>

#include <stdio.h>
#include <stdbool.h>

#include "menu.h"
#include "player.h"
#include "fase1.h"

typedef enum {
    ESTADO_MENU,
    ESTADO_JOGANDO,
    ESTADO_SAIR
} GameState;

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
  al_init_primitives_addon();

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
  ALLEGRO_BITMAP *seta_esquerda = al_load_bitmap("seta_esquerda.png");
  ALLEGRO_BITMAP *seta_direita = al_load_bitmap("seta_direita.png");
  ALLEGRO_BITMAP *seta_cima = al_load_bitmap("seta_cima.png");
  ALLEGRO_BITMAP *seta_baixo = al_load_bitmap("seta_baixo.png");
  ALLEGRO_BITMAP *flecha = al_load_bitmap("flecha.png");
  ALLEGRO_BITMAP *spritesheet_andando = al_load_bitmap("spritesheet andando.png");
  ALLEGRO_BITMAP *spritesheet_mirando = al_load_bitmap("spritesheet mirando.png");
  ALLEGRO_BITMAP *spritesheet_pulando = al_load_bitmap("spritesheet pulando.png"); 

  ALLEGRO_SAMPLE *musica_menu = al_load_sample("menu_theme.ogg");
  ALLEGRO_SAMPLE_INSTANCE *musica_menu_instancia = NULL;

  bool musica_tocando = false;
  int opcao = 0;
  static int volume_musica = 3;
  GameState estado_atual = ESTADO_MENU;

  if (musica_menu) 
  {
    musica_menu_instancia = al_create_sample_instance(musica_menu);
    al_attach_sample_instance_to_mixer(musica_menu_instancia, al_get_default_mixer());
    al_set_sample_instance_playmode(musica_menu_instancia, ALLEGRO_PLAYMODE_LOOP);
      
    // Define o ganho (volume) inicial baseado na nossa variável
    al_set_sample_instance_gain(musica_menu_instancia, (float)volume_musica / 50.0f);
      
    if (al_play_sample_instance(musica_menu_instancia))
      musica_tocando = true;
  }

    while (estado_atual != ESTADO_SAIR) 
  {
    if (estado_atual == ESTADO_MENU)
    {
        // Garante que a música do menu esteja tocando
        if (musica_menu_instancia && !al_get_sample_instance_playing(musica_menu_instancia)) {
            al_play_sample_instance(musica_menu_instancia);
        }

        menu_aparencia(opcao, disp, background, fonte_titulo, fonte_inicial, mao);

        ALLEGRO_EVENT ev;
        al_wait_for_event(queue, &ev);

        if (ev.type == ALLEGRO_EVENT_KEY_DOWN) 
        {
          if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN) { opcao = (opcao + 1) % 3; } 
          else if (ev.keyboard.keycode == ALLEGRO_KEY_UP) { opcao = (opcao + 2) % 3; } 
          else if (ev.keyboard.keycode == ALLEGRO_KEY_ENTER) 
          {
            if (opcao == START) 
            {
              estado_atual = ESTADO_JOGANDO; // Muda o estado para iniciar o jogo
            }
            else if (opcao == OPTIONS) 
            {
              menu_opcoes(disp, opcoes_background, fonte_opcoes, queue, musica_menu_instancia, &volume_musica);
            }
            else if (opcao == QUIT) 
            {
              estado_atual = ESTADO_SAIR; 
            }
          }
        }
        else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
          estado_atual = ESTADO_SAIR;
        }
    }
    else if (estado_atual == ESTADO_JOGANDO)
    {
        // Para a música do menu antes de entrar na fase
        if (musica_menu_instancia && al_get_sample_instance_playing(musica_menu_instancia)) {
            al_stop_sample_instance(musica_menu_instancia);
        }

        // Chama a função que contém todo o loop da fase 1
        iniciar_fase1(disp, queue, flecha, spritesheet_andando);
        
        // Quando a fase 1 terminar (seja por game over ou por completar), volta para o menu
        estado_atual = ESTADO_MENU;
    }
  }
  
  // --- LIMPEZA FINAL DE RECURSOS ---
  if (musica_menu_instancia) {
    al_stop_sample_instance(musica_menu_instancia);
    al_destroy_sample_instance(musica_menu_instancia);
  }
  al_destroy_sample(musica_menu);

  al_destroy_bitmap(background);
  al_destroy_bitmap(opcoes_background);
  al_destroy_bitmap(mao);
  al_destroy_bitmap(flecha);
  al_destroy_bitmap(spritesheet_andando);
  al_destroy_bitmap(spritesheet_mirando);
  al_destroy_bitmap(spritesheet_pulando);
  
  al_destroy_font(fonte_inicial);
  al_destroy_font(fonte_opcoes);
  al_destroy_font(fonte_titulo);
  
  al_destroy_display(disp);
  al_destroy_event_queue(queue);
  
  al_uninstall_audio();

  return 0;
}