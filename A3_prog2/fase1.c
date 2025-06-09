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


void iniciar_fase1(ALLEGRO_DISPLAY *display, ALLEGRO_EVENT_QUEUE *queue) 
{
    ALLEGRO_BITMAP *fundo = al_load_bitmap("fundo_fase1.png");

    if (!fundo) {
        fprintf(stderr, "ERRO FATAL: Falha ao carregar 'fundo_fase1.png'.\n");
        // Se esta mensagem aparecer no seu terminal, o problema é com o arquivo.
        return; // Sai da fase, mas não fecha o jogo.
    }
    
    // Declara as variáveis do jogador e seus projéteis
    Player player;
    ProjetilPlayer projeteis_player[MAX_PROJETEIS_PLAYER];
    
    Inimigo inimigos[MAX_INIMIGOS];

    // Usa as novas funções de inicialização
    inicializar_player(&player);
    inicializar_projeteis_player(projeteis_player);

    // Inicializar inimigos de exemplo
    for(int i = 0; i < MAX_INIMIGOS; i++) { inimigos[i].ativo = false; }
    inimigos[0] = (Inimigo){ 800, 600, 10, true, 0 };
    inimigos[1] = (Inimigo){ 1500, 600, 10, true, 0 };

    float camera_x = 0;
    bool sair_fase = false;
    ALLEGRO_KEYBOARD_STATE teclado;

    while(!sair_fase) 
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(queue, &ev);
        al_get_keyboard_state(&teclado);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE || al_key_down(&teclado, ALLEGRO_KEY_ESCAPE)) {
            sair_fase = true;
        }

        // --- PROCESSAR INPUT ---
        processar_input_player(&player, &teclado, projeteis_player);


        // --- ATUALIZAR LÓGICA ---
        // Player (se tivesse gravidade, etc, chamaria atualizar_player() aqui)
        atualizar_projeteis_player(projeteis_player, camera_x);

        // Câmera
        camera_x = player.x - (al_get_display_width(display) / 2);
        if (camera_x < 0) camera_x = 0;
        if (camera_x > LARGURA_NIVEL - al_get_display_width(display)) {
            camera_x = LARGURA_NIVEL - al_get_display_width(display);
        }

        // Inimigos e Colisões
        for (int i = 0; i < MAX_INIMIGOS; i++) {
            if (inimigos[i].ativo) {
                for (int j = 0; j < MAX_PROJETEIS_PLAYER; j++) {
                    if (projeteis_player[j].ativo) {
                        if (projeteis_player[j].x > inimigos[i].x && projeteis_player[j].x < inimigos[i].x + 50 &&
                            projeteis_player[j].y > inimigos[i].y && projeteis_player[j].y < inimigos[i].y + 50) 
                        {
                            inimigos[i].vida -= 1;
                            projeteis_player[j].ativo = false;
                            if (inimigos[i].vida <= 0) { inimigos[i].ativo = false; }
                        }
                    }
                }
            }
        }


        // --- RENDERIZAR (DESENHAR) ---
        al_clear_to_color(al_map_rgb(0, 0, 0));
        if (fundo) {
            al_draw_bitmap_region(fundo, camera_x, 0, al_get_display_width(display), ALTURA_NIVEL, 0, 0, 0);
        }
        
        // Usa as novas funções de desenho
        desenhar_player(&player, camera_x);
        desenhar_projeteis_player(projeteis_player, camera_x);

        // Desenhar Inimigos
        for (int i = 0; i < MAX_INIMIGOS; i++) {
            if (inimigos[i].ativo) {
                al_draw_filled_rectangle(inimigos[i].x - camera_x, inimigos[i].y, inimigos[i].x - camera_x + 50, inimigos[i].y + 50, al_map_rgb(255, 0, 0));
            }
        }

        al_flip_display();
    }

    al_destroy_bitmap(fundo);
}