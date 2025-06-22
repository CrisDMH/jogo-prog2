#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <stdio.h>

#include "hud.h"

// Bitmaps do rosto
static ALLEGRO_BITMAP* rosto_normal = NULL;
static ALLEGRO_BITMAP* rosto_machucado = NULL;
static ALLEGRO_BITMAP* rosto_destruido = NULL;
static ALLEGRO_FONT* fonte_reload = NULL;
static ALLEGRO_FONT* vida = NULL;

void carregar_hud() 
{
  rosto_normal = al_load_bitmap("personagem.png");
  rosto_machucado = al_load_bitmap("personagem machucado.png");
  rosto_destruido = al_load_bitmap("personagem muito machucado.png");
  vida = al_load_ttf_font("alagard.ttf", 60, 0);

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

  // Decide qual rosto mostrar com base na vida
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

  // Desenha o rosto no canto superior esquerdo
  al_draw_scaled_bitmap(
    rosto_atual,
    0, 0,                          // Parte da imagem fonte (x, y)
    al_get_bitmap_width(rosto_atual), al_get_bitmap_height(rosto_atual),  // Largura e altura da fonte
    20, 20,                         // Posição na tela (destino)
    128, 128,                         // Novo tamanho (exemplo: 64x64 pixels)
    0                               // Flags
);

  // Parâmetros da barra
  float largura_maxima = 200.0;
  float altura_barra = 20.0;
  float x_barra = 170.0;
  float y_barra = 40.0;

  // Calcula a largura proporcional da vida
  float largura_vida = (player->vida / 100.0) * largura_maxima;
  if (largura_vida < 0) largura_vida = 0;

  // Desenha o fundo da barra (cinza escuro)
  al_draw_filled_rectangle(x_barra, y_barra, x_barra + largura_maxima, y_barra + altura_barra, al_map_rgb(50, 50, 50));
  
  // Cor da vida (verde, amarelo ou vermelho dependendo do percentual)
  ALLEGRO_COLOR cor_vida;
  if (player->vida > 66) 
  {
    cor_vida = al_map_rgb(0, 255, 0);  // Verde
  } 
  else if (player->vida > 33) 
  {
    cor_vida = al_map_rgb(255, 255, 0);  // Amarelo
  } 
  else 
  {
    cor_vida = al_map_rgb(255, 0, 0);  // Vermelho
  }

  char texto_vida[16];
  sprintf(texto_vida, "%d%%", player->vida);
  
  // Desenha a borda do valor da vida
  al_draw_text(vida, al_map_rgb(0, 0, 0), 380 - 2, 25, 0, texto_vida);
  al_draw_text(vida, al_map_rgb(0, 0, 0), 380 + 2, 25, 0, texto_vida);
  al_draw_text(vida, al_map_rgb(0, 0, 0), 380, 25 - 2, 0, texto_vida);
  al_draw_text(vida, al_map_rgb(0, 0, 0), 380, 25 + 2, 0, texto_vida);
  al_draw_text(vida, al_map_rgb(0, 0, 0), 380 - 2, 25 - 2, 0, texto_vida);
  al_draw_text(vida, al_map_rgb(0, 0, 0), 380 + 2, 25 - 2, 0, texto_vida);
  al_draw_text(vida, al_map_rgb(0, 0, 0), 380 - 2, 25 + 2, 0, texto_vida);
  al_draw_text(vida, al_map_rgb(0, 0, 0), 380 + 2, 25 + 2, 0, texto_vida);
  
  // Valor da vida
  al_draw_text(vida, cor_vida, 380, 25, 0, texto_vida);

  // Desenha a barra de vida
  al_draw_filled_rectangle(x_barra, y_barra, x_barra + largura_vida, y_barra + altura_barra, cor_vida);

  // (Opcional) Borda da barra
  al_draw_rectangle(x_barra, y_barra, x_barra + largura_maxima, y_barra + altura_barra, al_map_rgb(20, 20, 20), 4.0);

  al_draw_rectangle(20, 20, 148, 148, al_map_rgb(150, 150, 150), 5.0);
}

void destruir_hud() 
{
  al_destroy_bitmap(rosto_normal);
  al_destroy_bitmap(rosto_machucado);
  al_destroy_bitmap(rosto_destruido);

  al_destroy_font(fonte_reload);
  al_destroy_font(vida);
}

void desenhar_balas(Player *player, ALLEGRO_BITMAP *icone_bala) {
    float x_base = 175;  // Posição inicial na tela
    float y_base = 110;  // Logo abaixo da barra de vida (ajuste conforme sua HUD)

    float escala = 1.2f;  
    float angulo = ALLEGRO_PI * 1.5;  

    for (int i = 0; i < MAX_BALAS; i++) {
        float x = x_base + i * 30;  // Ajuste o espaçamento para a nova largura (porque a largura e altura trocam ao rotacionar)
        float y = y_base;

        float cx = al_get_bitmap_width(icone_bala) / 2.0f;    // Centro X da imagem original
        float cy = al_get_bitmap_height(icone_bala) / 2.0f;   // Centro Y da imagem original

        ALLEGRO_BITMAP *temp = al_create_bitmap(
            al_get_bitmap_width(icone_bala),
            al_get_bitmap_height(icone_bala)
        );

        // Criar um bitmap temporário se quiser aplicar tint (opcional, só se for usar a versão escurecida)
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
            // Bala gasta (escurecida)
            al_draw_tinted_scaled_rotated_bitmap(
                icone_bala,
                al_map_rgba_f(0.3, 0.3, 0.3, 1.0),  // Escurecido
                cx, cy,
                x, y,
                escala, escala,
                angulo,
                0
            );
        }
    }
}