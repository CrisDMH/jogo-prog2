#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <stdbool.h>
#include <stdio.h>

#include "fase1.h"
#include "player.h" 
#include "hud.h"
#include "inimigos.h"

//--------CONSTANTES DA FASE------------
#define LARGURA_NIVEL 5330
#define ALTURA_NIVEL 800
#define MAX_INIMIGOS 10

//-------MUSICA DA FASE--------------
static ALLEGRO_SAMPLE* musica_fase1 = NULL;
static ALLEGRO_SAMPLE_INSTANCE* instancia_musica = NULL;
static float volume_musica = 0.0f;
static bool fade_in_ativo = false;

void verificar_colisoes(Player *player, Inimigo inimigos[], ProjetilPlayer projeteis_player[], ProjetilInimigo projeteis_inimigo[], float camera_x) 
{
    
    // --- VERIFICA TIROS DO JOGADOR ATINGINDO INIMIGOS ---
    for (int i = 0; i < MAX_PROJETEIS_PLAYER; i++) {
        if (!projeteis_player[i].ativo) continue;

        // Caixa de colisão do projétil
        float proj_p_x1 = projeteis_player[i].x - camera_x;
        float proj_p_x2 = projeteis_player[i].x + 100 - camera_x;
        float proj_p_y1 = projeteis_player[i].y + 175;
        float proj_p_y2 = projeteis_player[i].y + 185;

        for (int j = 0; j < MAX_INIMIGOS; j++) {
            if (!inimigos[j].ativo || inimigos[j].estado_atual == INIMIGO_ATINGIDO) continue;

            // --- HITBOX DO INIMIGO CORRIGIDA ---
            // A largura e altura da hitbox agora usam OS MESMOS DADOS da função de desenho,
            // garantindo um alinhamento perfeito com o sprite visível.
            // Usamos 'largura_frame_parado' como uma referência de tamanho consistente.
        float inimigo_largura = inimigos[j].largura_frame_parado * inimigos[j].escala;
        float inimigo_altura = inimigos[j].altura_frame_parado * inimigos[j].escala;
        float inimigo_x1 = (inimigos[j].x - inimigo_largura / 2) - camera_x + 140;
        float inimigo_x2 = (inimigos[j].x + inimigo_largura / 2)  - camera_x + 70;
        float inimigo_y1 = inimigos[j].y - inimigo_altura;
        float inimigo_y2 = inimigos[j].y;

            // Checagem de colisão de caixa contra caixa
            if (proj_p_x1 < inimigo_x2 && proj_p_x2 > inimigo_x1 && proj_p_y1 < inimigo_y2 && proj_p_y2 > inimigo_y1) {
                projeteis_player[i].ativo = false;
                inimigos[j].vida -= DANO_JOGADOR;
                
                if (inimigos[j].vida <= 0) {
                    inimigos[j].ativo = false; // MORREU
                } else {
                    inimigos[j].estado_atual = INIMIGO_ATINGIDO;
                    inimigos[j].frame_atual = 0;
                    inimigos[j].tempo_no_estado = 0;
                    inimigos[j].tempo_frame = 0;
                }
                break; 
            }
        }
    }

    // --- VERIFICA TIROS DO INIMIGO ATINGINDO O JOGADOR ---
    for (int i = 0; i < MAX_PROJETEIS_INIMIGO; i++) {
        if (!projeteis_inimigo[i].ativo) continue;

        // Caixa de colisão do projétil

        float proj_i_x1 = projeteis_inimigo[i].x - camera_x;
        float proj_i_x2 = projeteis_inimigo[i].x + 80 - camera_x;
        float proj_i_y1 = projeteis_inimigo[i].y - 30;
        float proj_i_y2 = projeteis_inimigo[i].y - 20;

    float player_largura = 60 * player->escala;
    float player_altura = 337 * player->escala; 
    
    float player_x1 = (player->x - player_largura / 2) - camera_x + 30 ;
    float player_x2 = (player->x + player_largura / 2) - camera_x + 70;
    float player_y1 = (player->y - player_altura) + 330;
    float player_y2 = player->y + 350;

    if (player->estado_atual == AGACHADO || player->estado_atual == ANDANDO_AGACHADO || player->estado_atual == ATACANDO_AGACHADO) 
      player_y1 = (player->y - player_altura) + 358;

        if (proj_i_x1 < player_x2 && proj_i_x2 > player_x1 && proj_i_y1 < player_y2 && proj_i_y2 > player_y1) {
            projeteis_inimigo[i].ativo = false;
            player->vida -= DANO_INIMIGO;
            if (player->vida < 0) player->vida = 0;
            break; 
        }
    }
}

void desenhar_debug_hitboxes(Player *player, Inimigo inimigos[], ProjetilPlayer projeteis_player[], ProjetilInimigo projeteis_inimigo[], float camera_x) 
{
    
    // --- HITBOX DO JOGADOR (Verde) ---
    float player_largura = 60 * player->escala;
    float player_altura = 337 * player->escala; // Altura baseada no sprite agachado
    float player_x1 = (player->x - player_largura / 2) - camera_x + 30 ;
    float player_x2 = (player->x + player_largura / 2) - camera_x + 70;
    float player_y1 = (player->y - player_altura) + 330;
    float player_y2 = player->y + 350;
        if (player->estado_atual == AGACHADO || player->estado_atual == ANDANDO_AGACHADO || player->estado_atual == ATACANDO_AGACHADO) 
      player_y1 = (player->y - player_altura) + 358;
    al_draw_rectangle(player_x1, player_y1, player_x2, player_y2, al_map_rgb(0, 255, 0), 2.0);

    // --- HITBOX DOS INIMIGOS (Vermelho) ---
    for (int i = 0; i < MAX_INIMIGOS; i++) {
        if (!inimigos[i].ativo) continue;
        float inimigo_largura = inimigos[i].largura_frame_parado * inimigos[i].escala;
        float inimigo_altura = inimigos[i].altura_frame_parado * inimigos[i].escala;
        float inimigo_x1 = (inimigos[i].x - inimigo_largura / 2) - camera_x + 140;
        float inimigo_x2 = (inimigos[i].x + inimigo_largura / 2)  - camera_x + 70;
        float inimigo_y1 = inimigos[i].y - inimigo_altura;
        float inimigo_y2 = inimigos[i].y;
        al_draw_rectangle(inimigo_x1, inimigo_y1, inimigo_x2, inimigo_y2, al_map_rgb(255, 0, 0), 2.0);
    }

    // --- HITBOX DOS PROJÉTEIS DO JOGADOR (Azul) ---
    for (int i = 0; i < MAX_PROJETEIS_PLAYER; i++) {
        if (!projeteis_player[i].ativo) continue;
        al_draw_filled_rectangle(projeteis_player[i].x - camera_x, projeteis_player[i].y + 175, projeteis_player[i].x + 100 - camera_x, projeteis_player[i].y + 185, al_map_rgb(0, 0, 255));
    }

    // --- HITBOX DOS PROJÉTEIS DO INIMIGO (Amarelo) ---
    for (int i = 0; i < MAX_PROJETEIS_INIMIGO; i++) {
        if (!projeteis_inimigo[i].ativo) continue;
        al_draw_filled_rectangle(projeteis_inimigo[i].x - camera_x, projeteis_inimigo[i].y -30, projeteis_inimigo[i].x + 80 - camera_x, projeteis_inimigo[i].y - 20, al_map_rgb(255, 255, 0));
    }
}

void iniciar_fase1(ALLEGRO_DISPLAY *display, ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_BITMAP *flecha, ALLEGRO_BITMAP *flecha_inimigo) 
{
  ALLEGRO_BITMAP *fundo = al_load_bitmap("fundo_fase1.png");

  musica_fase1 = al_load_sample("lost paintings.ogg");
  if (!musica_fase1) 
  {
    fprintf(stderr, "Erro ao carregar a música lost paintings.ogg\n");
  } 
  else
  {
    instancia_musica = al_create_sample_instance(musica_fase1);
    al_attach_sample_instance_to_mixer(instancia_musica, al_get_default_mixer());
    al_set_sample_instance_playmode(instancia_musica, ALLEGRO_PLAYMODE_LOOP);  // Loop infinito
    al_set_sample_instance_gain(instancia_musica, 0.0f);
    al_play_sample_instance(instancia_musica);
  }

  volume_musica = 0.0f;
  fade_in_ativo = true;


  if (!fundo)
  {
    fprintf(stderr, "ERRO FATAL: Falha ao carregar 'fundo_fase1.png'.\n");
    
    return; // Sai da fase, mas não fecha o jogo.
  }
    
  // Declara as variáveis do jogador e seus projéteis
  Player player;
  ProjetilPlayer projeteis_player[MAX_PROJETEIS_PLAYER];
  Inimigo inimigos[MAX_INIMIGOS];
  ProjetilInimigo projeteis_inimigo[MAX_PROJETEIS_INIMIGO];

  // Passa o spritesheet carregado em main.c para o jogador
  inicializar_player(&player);
  inicializar_projeteis_player(projeteis_player);
  inicializar_inimigos(inimigos);
  inicializar_projeteis_inimigo(projeteis_inimigo);  

  float camera_x = 0;
  bool sair_fase = false;
  bool redesenhar = true;
  ALLEGRO_KEYBOARD_STATE teclado;

  // --- CRIAÇÃO DO TIMER PARA CONTROLAR O FPS ---
  ALLEGRO_TIMER *timer_fase = al_create_timer(1.0 / 60);
  al_register_event_source(queue, al_get_timer_event_source(timer_fase));
  al_start_timer(timer_fase);

  while(!sair_fase) 
  {
    ALLEGRO_EVENT ev;
    al_wait_for_event(queue, &ev);

    if (fade_in_ativo && instancia_musica) 
    {
      volume_musica += 0.005f;  // Velocidade do fade (ajuste se quiser mais lento ou mais rápido)
      if (volume_musica >= 0.2f) 
      {
        volume_musica = 0.2f;
        fade_in_ativo = false;  // Fade concluído
      }

      al_set_sample_instance_gain(instancia_musica, volume_musica);
    }


    // --- ATUALIZAÇÃO DA LÓGICA (60 vezes por segundo) ---
    if (ev.type == ALLEGRO_EVENT_TIMER) 
    {
      redesenhar = true;
            
      al_get_keyboard_state(&teclado);
            
      processar_input_player(&player, &teclado, projeteis_player);

      // --- CORREÇÃO CRÍTICA: CHAMANDO A FUNÇÃO DE ATUALIZAÇÃO ---
      atualizar_player(&player, &teclado);
      atualizar_inimigos(inimigos, &player, projeteis_inimigo);
      atualizar_projeteis_player(projeteis_player, camera_x);
      atualizar_projeteis_inimigo(projeteis_inimigo);
            
      // Câmera...
      float largura_tela = al_get_display_width(display);

      verificar_colisoes(&player, inimigos, projeteis_player, projeteis_inimigo, camera_x);
            
      // 1. Defina a posição alvo da câmera: o centro da tela no jogador.
      float camera_alvo_x = player.x - (largura_tela / 2.0f);

      // 2. Crie um fator de suavização.
      //    Valores menores (ex: 0.02) = câmera mais lenta e suave.
      //    Valores maiores (ex: 0.1) = câmera mais rápida.
      float fator_suavizacao = 0.05f;

      // 3. Mova a câmera uma fração da distância até o alvo a cada frame.
      camera_x += (camera_alvo_x - camera_x) * fator_suavizacao;


      // Garante que a câmera não passe dos limites do nível (ESTA PARTE PERMANECE IGUAL)
      if (camera_x < 0)
        camera_x = 0;
      if (camera_x > LARGURA_NIVEL - largura_tela) 
      {
        camera_x = LARGURA_NIVEL - largura_tela;
      }
            
    }
    else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE || 
            (ev.type == ALLEGRO_EVENT_KEY_DOWN && ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)) 
    {
      sair_fase = true;
    }

    // --- RENDERIZAÇÃO ---
    if (redesenhar && al_is_event_queue_empty(queue)) 
    {
      redesenhar = false;
      al_clear_to_color(al_map_rgb(0, 0, 0));

      // Desenho do fundo...
      if (fundo) 
      {
        float altura_fundo = al_get_bitmap_height(fundo);
        float largura_tela = al_get_display_width(display);
        float altura_tela = al_get_display_height(display);
        float largura_fatia = altura_fundo * (largura_tela / altura_tela);

        float fator_parallax = 0.3f;

        al_draw_scaled_bitmap(fundo, camera_x * fator_parallax, 0, largura_fatia, altura_fundo, 0, 0, largura_tela, altura_tela, 0);
      }
            
      // Desenho dos elementos do jogo
      desenhar_player(&player, camera_x);
      desenhar_barra_vida(&player);
      desenhar_balas(&player, flecha);
      desenhar_projeteis_player(projeteis_player, camera_x, flecha);
      desenhar_inimigos(inimigos, camera_x);
      desenhar_projeteis_inimigo(projeteis_inimigo, camera_x, flecha_inimigo);
      desenhar_debug_hitboxes(&player, inimigos, projeteis_player, projeteis_inimigo, camera_x);

      al_flip_display();
    }
  }

  // Limpeza de recursos da fase
  al_stop_timer(timer_fase);
  al_destroy_timer(timer_fase);
  al_destroy_bitmap(fundo);
}

void encerrar_fase1() 
{
  if (instancia_musica) 
  {
    al_stop_sample_instance(instancia_musica);
    al_destroy_sample_instance(instancia_musica);
  }
  if (musica_fase1) 
  {
    al_destroy_sample(musica_fase1);
  }
}