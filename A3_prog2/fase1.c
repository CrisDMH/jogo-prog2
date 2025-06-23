#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <stdbool.h>
#include <stdio.h>

#include "fase1.h"
#include "player.h" 
#include "hud.h"
#include "inimigos.h"
#include "boss.h"

//--------CONSTANTES DA FASE------------
#define LARGURA_NIVEL 5330
#define ALTURA_NIVEL 800
#define MAX_INIMIGOS 10

//-------MUSICA DA FASE--------------
static ALLEGRO_SAMPLE* musica_fase1 = NULL;
static ALLEGRO_SAMPLE_INSTANCE* instancia_musica = NULL;
static float volume_musica = 0.0f;
static bool fade_in_ativo = false;

bool verificar_colisoes(Player *player, Inimigo inimigos[], ProjetilPlayer projeteis_player[], ProjetilInimigo projeteis_inimigo[],
                        Boss *boss, ProjetilBoss projeteis_boss[], float camera_x, int *inimigos_mortos_cont) 
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
            if (!inimigos[j].ativo || inimigos[j].estado_atual == INIMIGO_ATINGIDO || inimigos[j].estado_atual == INIMIGO_MORRENDO) continue;

        float inimigo_largura = inimigos[j].largura_frame_parado * inimigos[j].escala;
        float inimigo_altura = inimigos[j].altura_frame_parado * inimigos[j].escala;
        float inimigo_x1 = (inimigos[j].x - inimigo_largura / 2) - camera_x + 140;
        float inimigo_x2 = (inimigos[j].x + inimigo_largura / 2)  - camera_x + 70;
        float inimigo_y1 = inimigos[j].y - inimigo_altura;
        float inimigo_y2 = inimigos[j].y;

            if (proj_p_x1 < inimigo_x2 && proj_p_x2 > inimigo_x1 && proj_p_y1 < inimigo_y2 && proj_p_y2 > inimigo_y1) {
                projeteis_player[i].ativo = false;
                inimigos[j].vida -= DANO_JOGADOR;
                
                if (inimigos[j].vida <= 0) {
                    inimigos[j].estado_atual = INIMIGO_MORRENDO; // Muda para o estado de morte
                    inimigos[j].frame_atual = 0;
                    inimigos[j].tempo_no_estado = 0;
                    inimigos[j].tempo_frame = 0;
                    (*inimigos_mortos_cont)++;
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

    // --- VERIFICA TIROS DO INIMIGO ATINGINDO O JOGADOR----
    for (int i = 0; i < MAX_PROJETEIS_INIMIGO; i++) {
        if (!projeteis_inimigo[i].ativo) continue;

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

        if (proj_i_x1 < player_x2 && proj_i_x2 > player_x1 && proj_i_y1 < player_y2 && proj_i_y2 > player_y1) 
        {
            projeteis_inimigo[i].ativo = false;
            player->vida -= DANO_INIMIGO;
            if (player->vida <= 0) 
            {
              player->vida = 0;
                
              return true; // jogador morreu
            }
            
            break;
        }
    }

    // --- VERIFICA TIROS DO JOGADOR ATINGINDO O CHEFE --- 
    if (boss->ativo && boss->estado_atual != BOSS_ATINGIDO && boss->estado_atual != BOSS_MORRENDO) {
        float boss_largura = boss->largura_frame_parado * boss->escala;
        float boss_altura = boss->altura_frame_parado * boss->escala;
        float boss_x1 = boss->x - camera_x + 50;
        float boss_x2 = boss_x1 + boss_largura - 100;
        float boss_y1 = boss->y + 50;
        float boss_y2 = boss_y1 + boss_altura - 100;

        for (int i = 0; i < MAX_PROJETEIS_PLAYER; i++) {
            if (!projeteis_player[i].ativo) continue;
            float proj_p_x1 = projeteis_player[i].x - camera_x;
            float proj_p_x2 = proj_p_x1 + 115;
            float proj_p_y1 = projeteis_player[i].y + 175;
            float proj_p_y2 = proj_p_y1 + 10;

            if (proj_p_x1 < boss_x2 && proj_p_x2 > boss_x1 && proj_p_y1 < boss_y2 && proj_p_y2 > boss_y1) 
            {
                projeteis_player[i].ativo = false;
                boss->vida -= DANO_JOGADOR;

                if (boss->vida <= 0) 
                {
                  boss->estado_atual = BOSS_MORRENDO;
                } 
                else 
                {
                  boss->estado_anterior = boss->estado_atual;
                  boss->estado_atual = BOSS_ATINGIDO;
                }

                boss->frame_atual = 0;
                boss->tempo_no_estado = 0;
                break; 
            }
        }
    }

    // --- VERIFICA TIROS DO CHEFE ATINGINDO O JOGADOR --- 
    if (boss->ativo) {
        float player_largura = 60 * player->escala;
        float player_altura = 337 * player->escala;
        float player_x1 = (player->x - player_largura / 2) - camera_x + 30;
        float player_x2 = (player->x + player_largura / 2) - camera_x + 70;
        float player_y1 = (player->y - player_altura) + 330;
        float player_y2 = player->y + 350;

        for (int i = 0; i < MAX_PROJETEIS_BOSS; i++) {
            if (!projeteis_boss[i].ativo) continue;
            float proj_b_x1 = projeteis_boss[i].x - camera_x;
            float proj_b_x2 = proj_b_x1 + 100;
            float proj_b_y1 = projeteis_boss[i].y + 30;
            float proj_b_y2 = proj_b_y1 + 50;

            if (proj_b_x1 < player_x2 && proj_b_x2 > player_x1 && proj_b_y1 < player_y2 && proj_b_y2 > player_y1) {
                projeteis_boss[i].ativo = false;
                player->vida -= 30; // Dano do chefe, ajuste se necessário
                
                if (player->vida <= 0) {
                    player->vida = 0;
                    
                    return true; // Jogador morreu
                }

                break;
            }
        }
    }

  return false;
}

void desenhar_debug_hitboxes(Player *player, Inimigo inimigos[], ProjetilPlayer projeteis_player[], ProjetilInimigo projeteis_inimigo[],
                             Boss *boss, ProjetilBoss projeteis_boss[], float camera_x)
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
        if (!inimigos[i].ativo || inimigos[i].estado_atual == INIMIGO_MORRENDO) continue;
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

    // -----------HITBOX DO CHEFE (Magenta)---------
    if (boss->ativo) 
    {
        float boss_largura = boss->largura_frame_parado * boss->escala;
        float boss_altura = boss->altura_frame_parado * boss->escala;
        // Ajuste os valores (+50, etc) para alinhar a hitbox com a parte "sólida" do seu sprite
        float boss_x1 = boss->x - camera_x + 50; 
        float boss_x2 = boss_x1 + boss_largura - 100;
        float boss_y1 = boss->y + 50;
        float boss_y2 = boss_y1 + boss_altura - 100;
        al_draw_rectangle(boss_x1, boss_y1, boss_x2, boss_y2, al_map_rgb(255, 0, 255), 2.0);
    }

    //------------HITBOX DOS PROJÉTEIS DO CHEFE (Ciano)-----------
    for (int i = 0; i < MAX_PROJETEIS_BOSS; i++) 
    {
        if (!projeteis_boss[i].ativo) continue;
        // O tamanho (100x40) é um chute, ajuste conforme o tamanho escalado do seu projétil
        al_draw_filled_rectangle(projeteis_boss[i].x - camera_x, projeteis_boss[i].y + 30, projeteis_boss[i].x - camera_x + 115, projeteis_boss[i].y + 80, al_map_rgb(0, 255, 255));
    }

}

void iniciar_fase1(ALLEGRO_DISPLAY *display, ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_BITMAP *flecha, ALLEGRO_BITMAP *flecha_inimigo) 
{
  ALLEGRO_BITMAP* fundo  = al_load_bitmap("fundo_fase1.png");
  ALLEGRO_BITMAP* fundo_boss = al_load_bitmap("fundo boss.png");
  ALLEGRO_BITMAP* seta_go = al_load_bitmap("seta_go.png");
  ALLEGRO_FONT* fonte_go = al_load_ttf_font("alagard.ttf", 50, 0);

  ALLEGRO_SAMPLE* musica_boss = al_load_sample("Dracula Castle.ogg");
  ALLEGRO_SAMPLE_INSTANCE* instancia_musica_boss = NULL;
  bool fade_in_boss_ativo = false;  

  bool em_arena_boss = false;

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

  if (musica_boss) 
  {
    instancia_musica_boss = al_create_sample_instance(musica_boss);
    al_attach_sample_instance_to_mixer(instancia_musica_boss, al_get_default_mixer());
    al_set_sample_instance_playmode(instancia_musica_boss, ALLEGRO_PLAYMODE_LOOP);
    al_set_sample_instance_gain(instancia_musica_boss, 0.0f);
  }

  volume_musica = 0.0f;
  fade_in_ativo = true;
  int inimigos_mortos_cont = 0;

  if (!fundo)
  {
    fprintf(stderr, "ERRO FATAL: Falha ao carregar 'fundo_fase1.png'.\n");
    
    return; // Sai da fase, mas não fecha o jogo.
  }
    
  Player player;
  ProjetilPlayer projeteis_player[MAX_PROJETEIS_PLAYER];
  Inimigo inimigos[MAX_INIMIGOS];
  ProjetilInimigo projeteis_inimigo[MAX_PROJETEIS_INIMIGO];

  Boss boss;
  ProjetilBoss projeteis_boss[MAX_PROJETEIS_BOSS];
  bool boss_inicializado = false;

  inicializar_player(&player);
  inicializar_projeteis_player(projeteis_player);
  inicializar_inimigos(inimigos);
  inicializar_projeteis_inimigo(projeteis_inimigo);  

  float camera_x = 0;
  bool sair_fase = false;
  bool redesenhar = true;
  ALLEGRO_KEYBOARD_STATE teclado;

  // --- CRIAÇÃO DO TIMER PARA CONTROLAR O FPS ---
  ALLEGRO_TIMER* timer_fase = al_create_timer(1.0 / 60);
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

    if (fade_in_boss_ativo && instancia_musica_boss) 
    {
      float volume_atual = al_get_sample_instance_gain(instancia_musica_boss);
      volume_atual += 0.005f;
      if (volume_atual >= 0.4f) 
      { 
        volume_atual = 0.4f;
        fade_in_boss_ativo = false;
      }
      al_set_sample_instance_gain(instancia_musica_boss, volume_atual);
    }

    // --- ATUALIZAÇÃO DA LÓGICA (60 vezes por segundo) ---
    if (ev.type == ALLEGRO_EVENT_TIMER) 
    {
      redesenhar = true;
            
      al_get_keyboard_state(&teclado);
            
      processar_input_player(&player, &teclado, projeteis_player);

      atualizar_player(&player, &teclado);
      atualizar_inimigos(inimigos, &player, projeteis_inimigo);
      atualizar_projeteis_player(projeteis_player, camera_x);
      atualizar_projeteis_inimigo(projeteis_inimigo);

      if (inimigos_mortos_cont >= MAX_INIMIGOS) 
      {
        player.pode_avancar_fase = true;
      }

      if (player.pode_avancar_fase && player.x > LARGURA_NIVEL && !em_arena_boss) 
      {
        em_arena_boss = true;       // ativa o modo de arena
        player.x = 100.0f;          // reposiciona o jogador no começo da nova tela

        al_stop_sample_instance(instancia_musica);
        al_play_sample_instance(instancia_musica_boss);
        fade_in_boss_ativo = true;
      }

      if (verificar_colisoes(&player, inimigos, projeteis_player, projeteis_inimigo, &boss, projeteis_boss, camera_x, &inimigos_mortos_cont)) {
          sair_fase = true; // Se a função retornar true, o jogador morreu, então sai da fase
      }
              
      if (!em_arena_boss) 
      {
          // MODO SCROLLING: A câmera segue o jogador
          float largura_tela = al_get_display_width(display);
          float camera_alvo_x = player.x - (largura_tela / 2.0f);
          float fator_suavizacao = 0.05f;
          camera_x += (camera_alvo_x - camera_x) * fator_suavizacao;

          if (camera_x < 0) camera_x = 0;
          if (camera_x > LARGURA_NIVEL - largura_tela) {
              camera_x = LARGURA_NIVEL - largura_tela;
          }
      } 
      else 
      {
          // MODO ARENA: A câmera fica fixa
          camera_x = 0;
      }
              
      if (em_arena_boss) 
      {
          
        if (!boss_inicializado) 
        {
          float largura_tela = al_get_display_width(display);
          inicializar_boss(&boss, 0, largura_tela); // Arena começa em x=0 na tela
          inicializar_projeteis_boss(projeteis_boss);
          boss_inicializado = true;
        }

        atualizar_boss(&boss, &player, projeteis_boss);
        atualizar_projeteis_boss(projeteis_boss);

        float largura_tela = al_get_display_width(display);
        
        float player_largura_desenho = player.frame_largura_andando * player.escala;

        if (player.x < 0) 
        {
          player.x = 0;
        }
        if (player.x > largura_tela - player_largura_desenho) 
        {
          player.x = largura_tela - player_largura_desenho;
        }

        if (boss_inicializado && !boss.ativo) 
        {
            // --- TELA DE VITÓRIA ---

            // Para a música do chefe para criar um momento de silêncio
            if (instancia_musica_boss) {
                al_set_sample_instance_gain(instancia_musica_boss, 0.1f);
            }

            // Carrega as fontes para a tela de vitória
            ALLEGRO_FONT* fonte_vitoria = al_load_ttf_font("alagard.ttf", 35, 0);
            ALLEGRO_FONT* fonte_instrucao = al_load_ttf_font("alagard.ttf", 30, 0);

            bool enter_pressionado = false;
            while (!enter_pressionado) {
                float largura_tela = al_get_display_width(display);
                float altura_tela = al_get_display_height(display);

                // --- Desenho da tela ---
                al_clear_to_color(al_map_rgb(0, 0, 0));
                
                // Mensagem de vitória. Usamos al_draw_multiline_text para quebrar a linha automaticamente.
                const char* texto_vitoria = "Congratulations, you finished level 1. The other levels are in development. Dracula is waiting for you";
                if (fonte_vitoria) {
                    al_draw_multiline_text(fonte_vitoria, al_map_rgb(255, 215, 0), // Cor dourada
                                          largura_tela / 2, altura_tela / 2 - 80,
                                          largura_tela * 0.8, al_get_font_line_height(fonte_vitoria),
                                          ALLEGRO_ALIGN_CENTER, texto_vitoria);
                }

                // Instrução para o jogador
                const char* texto_instrucao = "Press ENTER to go back to the menu";
                if (fonte_instrucao) {
                    al_draw_text(fonte_instrucao, al_map_rgb(200, 200, 200),
                                largura_tela / 2, altura_tela / 2 + 80,
                                ALLEGRO_ALIGN_CENTER, texto_instrucao);
                }
              
                al_flip_display();

                // --- Espera pelo input ---
                ALLEGRO_EVENT ev_vitoria;
                al_wait_for_event(queue, &ev_vitoria);

                if ((ev_vitoria.type == ALLEGRO_EVENT_KEY_DOWN && ev_vitoria.keyboard.keycode == ALLEGRO_KEY_ENTER) 
                    || ev_vitoria.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                    
                    
                    if (instancia_musica_boss) {
                        al_stop_sample_instance(instancia_musica_boss);
                    }
                    enter_pressionado = true;
                }
              }

            // Limpa os recursos da tela de vitória
            if (fonte_vitoria) al_destroy_font(fonte_vitoria);
            if (fonte_instrucao) al_destroy_font(fonte_instrucao);

            // Define a flag para sair da fase APÓS a tela de vitória
            sair_fase = true;
        }

      }
    }
    else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE || 
            (ev.type == ALLEGRO_EVENT_KEY_DOWN && ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)) 
    {
      sair_fase = true;
    }
    
//-------------DEBUG DO F1 MATANDO INIMIGOS------------------------------

//    else if (ev.type == ALLEGRO_EVENT_KEY_DOWN && ev.keyboard.keycode == ALLEGRO_KEY_F1)
//    {
      // --- LÓGICA DE DEBUG: MATAR TODOS OS INIMIGOS ---
    
      // Itera por todos os inimigos
//      for (int i = 0; i < MAX_INIMIGOS; i++) 
//      {
        // Verifica se o inimigo está ativo e não está morrendo para evitar ações repetidas
//        if (inimigos[i].ativo && inimigos[i].estado_atual != INIMIGO_MORRENDO) 
//        {
//          inimigos[i].vida = 0;
//          inimigos[i].estado_atual = INIMIGO_MORRENDO;
//          inimigos[i].frame_atual = 0;
//          inimigos[i].tempo_no_estado = 0;
//          inimigos[i].tempo_frame = 0;
//        }
//      }
      // Define o contador de mortos para o máximo para acionar a seta "GO"
//      inimigos_mortos_cont = MAX_INIMIGOS;
//    }

//----------- DEBUG: PULAR PARA O FINAL DA FASE--------------------------------------
//    else if (ev.type == ALLEGRO_EVENT_KEY_DOWN && ev.keyboard.keycode == ALLEGRO_KEY_F2)
//    {
//        // Só teleporta se o jogador ainda não estiver na arena do chefe.
//        if (!em_arena_boss) {
//            player.x = LARGURA_NIVEL - 10;
//        }
//    }

    // --- RENDERIZAÇÃO ---
    if (redesenhar && al_is_event_queue_empty(queue)) 
    {
      redesenhar = false;
      al_clear_to_color(al_map_rgb(0, 0, 0));

        // Desenho do fundo...
      if (fundo && fundo_boss) {
          float largura_tela = al_get_display_width(display);
          float altura_tela = al_get_display_height(display);

          if (!em_arena_boss) {
              // MODO SCROLLING: Desenha o fundo da fase com efeito parallax
              float altura_fundo = al_get_bitmap_height(fundo);
              float largura_fatia = altura_fundo * (largura_tela / altura_tela);
              float fator_parallax = 0.3f;
              al_draw_scaled_bitmap(fundo, camera_x * fator_parallax, 0, largura_fatia, altura_fundo, 0, 0, largura_tela, altura_tela, 0);
          } 
          else 
          {
              // MODO ARENA: Desenha o fundo do chefe, estático e preenchendo a tela
              al_draw_scaled_bitmap(fundo_boss, 0, 0, al_get_bitmap_width(fundo_boss), al_get_bitmap_height(fundo_boss), 0, 0, largura_tela, altura_tela, 0);
          }

          if(em_arena_boss) 
          {
            desenhar_boss(&boss);
            desenhar_projeteis_boss(projeteis_boss, 0); // camera_x é 0 na arena
            // (aproveitamos a fonte_go que já passamos para a função)
            desenhar_barra_vida_boss(boss.vida, VIDA_MAX_BOSS, "Flying Demon", fonte_go);
          }
      }
                
      // Desenho dos elementos do jogo
      desenhar_player(&player, camera_x);
      desenhar_barra_vida(&player);
      desenhar_balas(&player, flecha);
      desenhar_projeteis_player(projeteis_player, camera_x, flecha);
      desenhar_inimigos(inimigos, camera_x);
      desenhar_projeteis_inimigo(projeteis_inimigo, camera_x, flecha_inimigo);
    //desenhar_debug_hitboxes(&player, inimigos, projeteis_player, projeteis_inimigo, &boss, projeteis_boss, camera_x); <- para ver as hitboxes
      desenhar_contador_inimigos(inimigos_mortos_cont);
      desenhar_seta_go(seta_go, fonte_go, (inimigos_mortos_cont >= MAX_INIMIGOS && !em_arena_boss));

      al_flip_display();
    }
    
    if (player.vida <= 0) 
    {
      // Carrega as fontes com um tamanho maior para mais impacto
      ALLEGRO_FONT* fonte_game_over = al_load_ttf_font("alagard.ttf", 120, 0);
      ALLEGRO_FONT* fonte_instrucao = al_load_ttf_font("alagard.ttf", 45, 0);
      
      if (!fonte_game_over || !fonte_instrucao) {
          fprintf(stderr, "Erro ao carregar fonte para a tela de Game Over.\n");
          // Mesmo com erro na fonte, o loop abaixo ainda funciona para sair
      }

      bool enter_pressionado = false;
      while(!enter_pressionado) {
          // Pega as dimensões da tela dinamicamente a cada quadro
          float largura_tela = al_get_display_width(display);
          float altura_tela = al_get_display_height(display);

          // Desenha a tela
          al_clear_to_color(al_map_rgb(0,0,0));

          // Usa as variáveis de dimensão para garantir uma centralização perfeita
          al_draw_text(fonte_game_over, al_map_rgb(200,0,0), largura_tela / 2, altura_tela / 2 - 100, ALLEGRO_ALIGN_CENTER, "GAME OVER");
          al_draw_text(fonte_instrucao, al_map_rgb(200,200,200), largura_tela / 2, altura_tela / 2 + 60, ALLEGRO_ALIGN_CENTER, "Press ENTER to return to the menu");
          
          al_flip_display();

          ALLEGRO_EVENT ev_go;
          al_wait_for_event(queue, &ev_go);
          
          if ((ev_go.type == ALLEGRO_EVENT_KEY_DOWN && ev_go.keyboard.keycode == ALLEGRO_KEY_ENTER) || ev_go.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
              enter_pressionado = true;
          }
      }
      
      // Limpa os recursos que foram carregados apenas para esta tela
      if(fonte_game_over) al_destroy_font(fonte_game_over);
      if(fonte_instrucao) al_destroy_font(fonte_instrucao);
    }
  }

  // Limpeza de recursos da fase
  al_stop_timer(timer_fase);
  al_destroy_timer(timer_fase);
  al_destroy_bitmap(fundo);
  al_destroy_bitmap(fundo_boss);

  if (instancia_musica_boss) 
  {
    al_destroy_sample_instance(instancia_musica_boss);
  }
  if (musica_boss) 
  {
    al_destroy_sample(musica_boss);
  }
}

void encerrar_fase1() 
{
  if (instancia_musica) 
  {
    al_stop_sample_instance(instancia_musica);
    al_destroy_sample_instance(instancia_musica);
    instancia_musica = NULL;
  }

  if (musica_fase1) 
  {
    al_destroy_sample(musica_fase1);
    musica_fase1 = NULL;
  }
}

bool mostrar_tela_instrucoes(ALLEGRO_DISPLAY *display, ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_FONT *fonte_titulo, ALLEGRO_FONT *fonte_texto)
{
  float largura_tela = al_get_display_width(display);
  float altura_tela = al_get_display_height(display);

  // limpa qualquer evento de teclado que possa estar na fila para evitar pulos acidentais
  al_flush_event_queue(queue);

  while (true) 
  {
    // -----------DESENHO DA TELA----------------------
    al_clear_to_color(al_map_rgb(0, 0, 0)); 

    ALLEGRO_COLOR cor_titulo = al_map_rgb(200, 200, 200);
    ALLEGRO_COLOR cor_texto = al_map_rgb(180, 180, 180);
    ALLEGRO_COLOR cor_destaque = al_map_rgb(255, 255, 0);

    al_draw_text(fonte_titulo, cor_titulo, largura_tela / 2, altura_tela * 0.15, ALLEGRO_ALIGN_CENTER, "Kill 10 enemies to go to the boss");

    float y_base_controles = altura_tela * 0.35;
    float x_central_controles = largura_tela / 2;
    float altura_linha = al_get_font_line_height(fonte_texto) + 10; // Adiciona um espaço extra

    al_draw_text(fonte_titulo, cor_titulo, x_central_controles, y_base_controles, ALLEGRO_ALIGN_CENTER, "Controls:");
        
    al_draw_text(fonte_texto, cor_texto, x_central_controles - 20, y_base_controles + altura_linha * 2, ALLEGRO_ALIGN_RIGHT, "< >");
    al_draw_text(fonte_texto, cor_texto, x_central_controles, y_base_controles + altura_linha * 2, ALLEGRO_ALIGN_LEFT, "-  LEFT AND RIGHT");
        
    al_draw_text(fonte_texto, cor_texto, x_central_controles - 20, y_base_controles + altura_linha * 3, ALLEGRO_ALIGN_RIGHT, "UP/DOWN");
    al_draw_text(fonte_texto, cor_texto, x_central_controles, y_base_controles + altura_linha * 3, ALLEGRO_ALIGN_LEFT, "-  JUMP AND CROUCH");

    al_draw_text(fonte_texto, cor_texto, x_central_controles - 20, y_base_controles + altura_linha * 4, ALLEGRO_ALIGN_RIGHT, "X");
    al_draw_text(fonte_texto, cor_texto, x_central_controles, y_base_controles + altura_linha * 4, ALLEGRO_ALIGN_LEFT, "-  SHOOT");

    al_draw_text(fonte_texto, cor_texto, x_central_controles - 20, y_base_controles + altura_linha * 5, ALLEGRO_ALIGN_RIGHT, "ESC");
    al_draw_text(fonte_texto, cor_texto, x_central_controles, y_base_controles + altura_linha * 5, ALLEGRO_ALIGN_LEFT, "-  MENU");

    float y_base_instrucao_final = altura_tela * 0.85;
    al_draw_text(fonte_texto, cor_destaque, largura_tela / 2, y_base_instrucao_final, ALLEGRO_ALIGN_CENTER, "Press ENTER to continue to the level 1");
    al_draw_text(fonte_texto, cor_texto, largura_tela / 2, y_base_instrucao_final + altura_linha, ALLEGRO_ALIGN_CENTER, "Press ESC to go back to the menu");
        
    al_flip_display();

    ALLEGRO_EVENT ev;
    al_wait_for_event(queue, &ev);

    if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) 
    {
      return false; // se fechar a janela volta para o menu
    }
    if (ev.type == ALLEGRO_EVENT_KEY_DOWN) 
    {
      if (ev.keyboard.keycode == ALLEGRO_KEY_ENTER) 
      {
        return true; // retorna true para prosseguir para a fase
      }
      if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) 
      {
        return false; // retorna false para voltar ao menu
      }
    }
  }
}