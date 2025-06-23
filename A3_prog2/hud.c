#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <stdio.h>

#include "hud.h"

static ALLEGRO_BITMAP* rosto_normal = NULL;
static ALLEGRO_BITMAP* rosto_machucado = NULL;
static ALLEGRO_BITMAP* rosto_destruido = NULL;
static ALLEGRO_FONT* fonte_reload = NULL;
static ALLEGRO_FONT* vida = NULL;
static ALLEGRO_BITMAP* caveira = NULL;

static float seta_go_offset_x = 0.0f;
static float seta_go_dir = 1.0f;

void carregar_hud() 
{
  rosto_normal = al_load_bitmap("personagem.png");
  rosto_machucado = al_load_bitmap("personagem machucado.png");
  rosto_destruido = al_load_bitmap("personagem muito machucado.png");
  vida = al_load_ttf_font("alagard.ttf", 60, 0);
  caveira = al_load_bitmap("caveira.png");

  if (!rosto_normal || !rosto_machucado || !rosto_destruido) 
  {
    printf("Erro ao carregar as imagens do HUD.\n");
  }

  fonte_reload = al_load_ttf_font("alagard.ttf", 30, 0);
  if (!fonte_reload) 
  {
    fprintf(stderr, "Erro ao carregar a fonte alagard.ttf\n");
  }
}

void desenhar_barra_vida(Player* player) 
{
  if (!rosto_normal || !rosto_machucado || !rosto_destruido) 
    return;

  ALLEGRO_BITMAP* rosto_atual;

  // decide qual rosto mostrar com base na vida
  if (player->vida >= 66 ) 
  {
    rosto_atual = rosto_normal;
  } 
  else if (player->vida >= 33 && player->vida < 66) 
  {
    rosto_atual = rosto_machucado;
  }
  else
  {
    rosto_atual = rosto_destruido;
  }

  // rosto no canto superior esquerdo
  al_draw_scaled_bitmap(
    rosto_atual,
    0, 0,                         
    al_get_bitmap_width(rosto_atual), al_get_bitmap_height(rosto_atual),  
    20, 20,                         
    128, 128,                         
    0                              
);

  float largura_maxima = 200.0;
  float altura_barra = 20.0;
  float x_barra = 170.0;
  float y_barra = 40.0;

  float largura_vida = (player->vida / 100.0) * largura_maxima;
  if (largura_vida < 0) largura_vida = 0;

  al_draw_filled_rectangle(x_barra, y_barra, x_barra + largura_maxima, y_barra + altura_barra, al_map_rgb(50, 50, 50));
  
  ALLEGRO_COLOR cor_vida;
  if (player->vida > 66) 
  {
    cor_vida = al_map_rgb(0, 255, 0);  
  } 
  else if (player->vida > 33) 
  {
    cor_vida = al_map_rgb(255, 255, 0); 
  } 
  else 
  {
    cor_vida = al_map_rgb(255, 0, 0); 
  }

  char texto_vida[16];
  sprintf(texto_vida, "%d%%", player->vida);
  
  // borda do valor da vida
  al_draw_text(vida, al_map_rgb(0, 0, 0), 380 - 2, 25, 0, texto_vida);
  al_draw_text(vida, al_map_rgb(0, 0, 0), 380 + 2, 25, 0, texto_vida);
  al_draw_text(vida, al_map_rgb(0, 0, 0), 380, 25 - 2, 0, texto_vida);
  al_draw_text(vida, al_map_rgb(0, 0, 0), 380, 25 + 2, 0, texto_vida);
  al_draw_text(vida, al_map_rgb(0, 0, 0), 380 - 2, 25 - 2, 0, texto_vida);
  al_draw_text(vida, al_map_rgb(0, 0, 0), 380 + 2, 25 - 2, 0, texto_vida);
  al_draw_text(vida, al_map_rgb(0, 0, 0), 380 - 2, 25 + 2, 0, texto_vida);
  al_draw_text(vida, al_map_rgb(0, 0, 0), 380 + 2, 25 + 2, 0, texto_vida);
  
  // valor da vida
  al_draw_text(vida, cor_vida, 380, 25, 0, texto_vida);

  // barra de vida
  al_draw_filled_rectangle(x_barra, y_barra, x_barra + largura_vida, y_barra + altura_barra, cor_vida);

  // borda da barra
  al_draw_rectangle(x_barra, y_barra, x_barra + largura_maxima, y_barra + altura_barra, al_map_rgb(20, 20, 20), 4.0);

  al_draw_rectangle(20, 20, 148, 148, al_map_rgb(150, 150, 150), 5.0);
}

void destruir_hud() 
{
  al_destroy_bitmap(rosto_normal);
  al_destroy_bitmap(rosto_machucado);
  al_destroy_bitmap(rosto_destruido);
  al_destroy_bitmap(caveira);

  al_destroy_font(fonte_reload);
  al_destroy_font(vida);
}

void desenhar_balas(Player *player, ALLEGRO_BITMAP *icone_bala) {
    float x_base = 175;  
    float y_base = 110;  

    float escala = 1.2f;  
    float angulo = ALLEGRO_PI * 1.5;  

    for (int i = 0; i < MAX_BALAS; i++) {
        float x = x_base + i * 30;  
        float y = y_base;

        float cx = al_get_bitmap_width(icone_bala) / 2.0f;    
        float cy = al_get_bitmap_height(icone_bala) / 2.0f;   

        ALLEGRO_BITMAP *temp = al_create_bitmap(
            al_get_bitmap_width(icone_bala),
            al_get_bitmap_height(icone_bala)
        );

        if (player->recarregando) 
        {
          al_draw_text(fonte_reload, al_map_rgb(255, 255, 255), x_base, y_base - 15, 0, "RELOADING...");
        }
        else if (i < player->balas) 
        {
            // Bala cheia
            al_draw_tinted_scaled_rotated_bitmap(
                icone_bala,
                al_map_rgba_f(1, 1, 1, 1),  // Sem escurecer
                cx, cy,
                x, y,
                escala, escala,
                angulo,
                0
            );
        } 
        else 
        {
            // bala gasta (escurecida)
            al_draw_tinted_scaled_rotated_bitmap(
                icone_bala,
                al_map_rgba_f(0.3, 0.3, 0.3, 1.0),  // escurecido
                cx, cy,
                x, y,
                escala, escala,
                angulo,
                0
            );
        }
    }
}

void desenhar_contador_inimigos(int inimigos_mortos) 
{
    if (!fonte_reload || !caveira) return; // não desenha se os recursos não carregaram


    float largura_tela = 1280;
    float altura_tela = 800;
    
  
    char texto[32];
    sprintf(texto, "Enemies killed: %d", inimigos_mortos);

    ALLEGRO_COLOR cor_texto = al_map_rgb(255, 255, 255);
   
    float escala_caveira = 0.1f;
    float largura_caveira = al_get_bitmap_width(caveira) * escala_caveira;
    float altura_caveira = al_get_bitmap_height(caveira) * escala_caveira;
    float x_caveira = largura_tela - largura_caveira + 600;
    float y_caveira = altura_tela - altura_caveira - 700;
    al_draw_scaled_bitmap(caveira, 0, 0, al_get_bitmap_width(caveira), al_get_bitmap_height(caveira), x_caveira, y_caveira, largura_caveira, altura_caveira, 0);

    float espaco_entre_texto_e_icone = 5;
    float x_texto = x_caveira - espaco_entre_texto_e_icone;
    float y_texto = y_caveira + (altura_caveira / 2) - (al_get_font_line_height(fonte_reload) / 2);

    al_draw_text(fonte_reload, cor_texto, x_texto, y_texto, ALLEGRO_ALIGN_RIGHT, texto);
}

void desenhar_seta_go(ALLEGRO_BITMAP *seta_go, ALLEGRO_FONT *fonte_go, bool mostrar)
{
    // se a condição para mostrar não for verdadeira, ou se os recursos não foram carregados, a função não faz nada.
    if (!mostrar || !seta_go || !fonte_go) {
        return;
    }

    float escala_seta = 0.6f;

    seta_go_offset_x += seta_go_dir * 0.5f; 
    if (seta_go_offset_x > 10.0f || seta_go_offset_x < -10.0f) {
        seta_go_dir *= -1; 
    }

    float largura_original = al_get_bitmap_width(seta_go);
    float altura_original = al_get_bitmap_height(seta_go);
    float largura_escalada = largura_original * escala_seta;
    float altura_escalada = altura_original * escala_seta;
    
    float seta_go_y = 80.0f;
    
    float pos_x_base = al_get_display_width(al_get_current_display()) - largura_escalada - 40;
    
    float pos_x_final = pos_x_base + seta_go_offset_x;
    
    al_draw_scaled_bitmap(seta_go,
                          0, 0,
                          largura_original, altura_original,
                          pos_x_final, seta_go_y,
                          largura_escalada, altura_escalada,
                          0);

    float texto_x = pos_x_final + largura_escalada / 2.0f - 10;
    float texto_y = seta_go_y + (altura_escalada / 2.0f) - (al_get_font_ascent(fonte_go) / 2.0f) - 80;
    
    const char* texto_go = "GO";
    ALLEGRO_COLOR cor_borda = al_map_rgb(0, 0, 0);
    ALLEGRO_COLOR cor_texto = al_map_rgb(255, 30, 30);
    int tamanho_borda = 2;

    // desenha a borda 
    al_draw_text(fonte_go, cor_borda, texto_x - tamanho_borda, texto_y, ALLEGRO_ALIGN_CENTER, texto_go);
    al_draw_text(fonte_go, cor_borda, texto_x + tamanho_borda, texto_y, ALLEGRO_ALIGN_CENTER, texto_go);
    al_draw_text(fonte_go, cor_borda, texto_x, texto_y - tamanho_borda, ALLEGRO_ALIGN_CENTER, texto_go);
    al_draw_text(fonte_go, cor_borda, texto_x, texto_y + tamanho_borda, ALLEGRO_ALIGN_CENTER, texto_go);
    al_draw_text(fonte_go, cor_borda, texto_x - tamanho_borda, texto_y - tamanho_borda, ALLEGRO_ALIGN_CENTER, texto_go);
    al_draw_text(fonte_go, cor_borda, texto_x + tamanho_borda, texto_y - tamanho_borda, ALLEGRO_ALIGN_CENTER, texto_go);
    al_draw_text(fonte_go, cor_borda, texto_x - tamanho_borda, texto_y + tamanho_borda, ALLEGRO_ALIGN_CENTER, texto_go);
    al_draw_text(fonte_go, cor_borda, texto_x + tamanho_borda, texto_y + tamanho_borda, ALLEGRO_ALIGN_CENTER, texto_go);

    al_draw_text(fonte_go, cor_texto, texto_x, texto_y, ALLEGRO_ALIGN_CENTER, texto_go);
}

void desenhar_barra_vida_boss(int vida_atual, int vida_max, const char* nome, ALLEGRO_FONT *fonte) {
    if (vida_atual <= 0 || !fonte) return;

    float largura_tela = al_get_display_width(al_get_current_display());
    float largura_max_barra = largura_tela * 0.6; // Barra ocupa 60% da tela
    float altura_barra = 25;
    float x_barra = (largura_tela - largura_max_barra) / 2.0; // Centraliza
    float y_barra = 200;
    float y_texto = y_barra - 60;

    al_draw_text(fonte, al_map_rgb(255,255,255), largura_tela / 2, y_texto, ALLEGRO_ALIGN_CENTER, nome);

    float largura_vida = ((float)vida_atual / vida_max) * largura_max_barra;
    al_draw_filled_rectangle(x_barra, y_barra, x_barra + largura_max_barra, y_barra + altura_barra, al_map_rgb(50, 0, 0));
    al_draw_filled_rectangle(x_barra, y_barra, x_barra + largura_vida, y_barra + altura_barra, al_map_rgb(200, 0, 0));
    al_draw_rectangle(x_barra, y_barra, x_barra + largura_max_barra, y_barra + altura_barra, al_map_rgb(255,255,255), 3.0);
}