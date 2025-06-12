#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <stdbool.h>
#include <stdio.h>

#include "fase1.h"
#include "player.h" // Inclui o nosso novo header do player

// --- CONSTANTES DA FASE ---
#define LARGURA_NIVEL 4000
#define ALTURA_NIVEL 800
#define MAX_INIMIGOS 10

// Struct do inimigo permanece aqui (ou poderia ir para seus próprios arquivos inimigo.h/c)
typedef struct {
    float x, y;
    int vida;
    bool ativo;
    float cooldown_tiro;
} Inimigo;


void iniciar_fase1(ALLEGRO_DISPLAY *display, ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_BITMAP *flecha, ALLEGRO_BITMAP *sprite_player) 
{
    ALLEGRO_BITMAP *fundo = al_load_bitmap("fundo_fase1.png");
    ALLEGRO_BITMAP *spritesheet_andando = al_load_bitmap("spritesheet andando.png");
    ALLEGRO_BITMAP *spritesheet_mirando = al_load_bitmap("spritesheet mirando.png");
    ALLEGRO_BITMAP *spritesheet_pulando = al_load_bitmap("spritesheet pulando.png");

    if (!fundo) {
        fprintf(stderr, "ERRO FATAL: Falha ao carregar 'fundo_fase1.png'.\n");
        // Se esta mensagem aparecer no seu terminal, o problema é com o arquivo.
        return; // Sai da fase, mas não fecha o jogo.
    }
    
    // Declara as variáveis do jogador e seus projéteis
    Player player;
    ProjetilPlayer projeteis_player[MAX_PROJETEIS_PLAYER];
    Inimigo inimigos[MAX_INIMIGOS];

    // Passa o spritesheet carregado em main.c para o jogador
    inicializar_player(&player, spritesheet_andando, spritesheet_mirando, spritesheet_pulando);
    inicializar_projeteis_player(projeteis_player);

    for(int i = 0; i < MAX_INIMIGOS; i++) { inimigos[i].ativo = false; }
    inimigos[0] = (Inimigo){ 800, 600, 10, true, 0 };
    inimigos[1] = (Inimigo){ 1500, 600, 10, true, 0 };

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

        // --- ATUALIZAÇÃO DA LÓGICA (60 vezes por segundo) ---
        if (ev.type == ALLEGRO_EVENT_TIMER) {
            redesenhar = true;
            
            al_get_keyboard_state(&teclado);
            
            processar_input_player(&player, &teclado, projeteis_player);

            // --- CORREÇÃO CRÍTICA: CHAMANDO A FUNÇÃO DE ATUALIZAÇÃO ---
            atualizar_player(&player, &teclado);

            atualizar_projeteis_player(projeteis_player, camera_x);
            
            // Câmera...
            float largura_tela = al_get_display_width(display);
            
            // Define os limites da "zona morta". Ex: entre 40% e 60% da tela.
            // Você pode ajustar estes valores para mudar o comportamento da câmera.
            float limite_scroll_esquerda = largura_tela * 0.4f;
            float limite_scroll_direita = largura_tela * 0.6f;

            // Calcula a posição do jogador NA TELA (relativa à câmera)
            float player_pos_tela_x = player.x - camera_x;

            // Se o jogador passar do limite direito, move a câmera para a direita
            if (player_pos_tela_x > limite_scroll_direita) {
                camera_x += player_pos_tela_x - limite_scroll_direita;
            }
            // Se o jogador passar do limite esquerdo, move a câmera para a esquerda
            else if (player_pos_tela_x < limite_scroll_esquerda) {
                camera_x -= limite_scroll_esquerda - player_pos_tela_x;
            }

            // Garante que a câmera não passe dos limites do nível
            if (camera_x < 0) camera_x = 0;
            if (camera_x > LARGURA_NIVEL - largura_tela) {
                camera_x = LARGURA_NIVEL - largura_tela;
            }
            
            // Colisões...
            // (seu código de colisão aqui)
        }
        else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE || 
                (ev.type == ALLEGRO_EVENT_KEY_DOWN && ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {
            sair_fase = true;
        }

        // --- RENDERIZAÇÃO ---
        if (redesenhar && al_is_event_queue_empty(queue)) {
            redesenhar = false;
            al_clear_to_color(al_map_rgb(0, 0, 0));

            // Desenho do fundo...
            if (fundo) {
                float altura_fundo = al_get_bitmap_height(fundo);
                float largura_tela = al_get_display_width(display);
                float altura_tela = al_get_display_height(display);
                float largura_fatia = altura_fundo * (largura_tela / altura_tela);
                al_draw_scaled_bitmap(fundo, camera_x, 0, largura_fatia, altura_fundo, 0, 0, largura_tela, altura_tela, 0);
            }
            
            // Desenho dos elementos do jogo
            desenhar_player(&player, camera_x);
            desenhar_projeteis_player(projeteis_player, camera_x, flecha);
            // ... (desenho de inimigos)

            al_flip_display();
        }
    }

    // Limpeza de recursos da fase
    al_stop_timer(timer_fase);
    al_destroy_timer(timer_fase);
    al_destroy_bitmap(fundo);
}
