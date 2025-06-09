#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>

#include <stdio.h>

#include "menu.h"

void menu_aparencia(int opcoes, ALLEGRO_DISPLAY *disp, ALLEGRO_BITMAP *background,
                    ALLEGRO_FONT *fonte_titulo, ALLEGRO_FONT *fonte_itens, ALLEGRO_BITMAP *mao)
{
  int largura_tela = al_get_display_width(disp);
  int altura_tela = al_get_display_height(disp);

  if (background != NULL) 
  {
    al_draw_scaled_bitmap(background, 
                          0, 0, al_get_bitmap_width(background), al_get_bitmap_height(background),
                          0, 0, largura_tela, altura_tela, 0);
  }

  // ---------------------BORDA DAS FONTES-------------------------------------
    ALLEGRO_COLOR cor_borda = al_map_rgb(0, 0, 0); 
    int tamanho_borda = 3; 

  //--------------------------TITULO---------------------------------------
  const char *titulo = "Dracula's Curse";
  ALLEGRO_COLOR cor_titulo = al_map_rgb(220, 20, 60); 
  float x_titulo_pos = largura_tela / 2.0f;
  float y_titulo_pos = altura_tela * 0.20f;

  
  if (fonte_titulo) // desenha a borda do titulo
  {
    al_draw_text(fonte_titulo, cor_borda, x_titulo_pos - tamanho_borda, y_titulo_pos - tamanho_borda, ALLEGRO_ALIGN_CENTER, titulo);
    al_draw_text(fonte_titulo, cor_borda, x_titulo_pos + tamanho_borda, y_titulo_pos - tamanho_borda, ALLEGRO_ALIGN_CENTER, titulo);
    al_draw_text(fonte_titulo, cor_borda, x_titulo_pos - tamanho_borda, y_titulo_pos + tamanho_borda, ALLEGRO_ALIGN_CENTER, titulo);
    al_draw_text(fonte_titulo, cor_borda, x_titulo_pos + tamanho_borda, y_titulo_pos + tamanho_borda, ALLEGRO_ALIGN_CENTER, titulo);
    al_draw_text(fonte_titulo, cor_borda, x_titulo_pos - tamanho_borda, y_titulo_pos, ALLEGRO_ALIGN_CENTER, titulo);
    al_draw_text(fonte_titulo, cor_borda, x_titulo_pos + tamanho_borda, y_titulo_pos, ALLEGRO_ALIGN_CENTER, titulo);
    al_draw_text(fonte_titulo, cor_borda, x_titulo_pos, y_titulo_pos - tamanho_borda, ALLEGRO_ALIGN_CENTER, titulo);
    al_draw_text(fonte_titulo, cor_borda, x_titulo_pos, y_titulo_pos + tamanho_borda, ALLEGRO_ALIGN_CENTER, titulo);

    al_draw_text(fonte_titulo, cor_titulo, x_titulo_pos, y_titulo_pos, ALLEGRO_ALIGN_CENTER, titulo);
  }

  //-----------------------OPCOES DO MENU-----------------------------------------
  const char *menu_itens[] = {"play", "options", "quit"};
  int total_itens = 3;

  float y_base = altura_tela / 2 - 50;
  float x_centro_tela = largura_tela / 2;

  int deslocamento_selecionado = -20; 
  int espaco_entre_texto_e_mao = 15;
  float escala_mao = 3.0f;

  float max_largura_texto = 0;
  for (int i = 0; i < total_itens; i++) 
  {
    float largura_atual = al_get_text_width(fonte_itens, menu_itens[i]);
    if (largura_atual > max_largura_texto) 
    {
      max_largura_texto = largura_atual;
    }
  }

  float x_inicio_bloco_texto = x_centro_tela - (max_largura_texto / 2);
  float espacamento_item_vertical = (fonte_itens ? al_get_font_line_height(fonte_itens) : 40) + 30;

  for (int i = 0; i < total_itens; i++)
  {
    ALLEGRO_COLOR cor_texto;
    float x_pos_desenho_texto = x_inicio_bloco_texto; // Todos os textos começam aqui para alinhamento à esquerda do bloco

    if (i == opcoes) // Se esta é a opção selecionada
    {
      cor_texto = al_map_rgb(255, 0, 0); // Vermelho
      x_pos_desenho_texto += deslocamento_selecionado; // Aplica o deslocamento para a esquerda
    }
    else
    {
      cor_texto = al_map_rgb(255, 255, 255); // Branco
    }

    float texto_y = y_base + i * espacamento_item_vertical;

    if (fonte_itens) // desenha a borda das opcoes
    {
      al_draw_text(fonte_itens, cor_borda, x_pos_desenho_texto - tamanho_borda, texto_y - tamanho_borda, ALLEGRO_ALIGN_LEFT, menu_itens[i]);
      al_draw_text(fonte_itens, cor_borda, x_pos_desenho_texto + tamanho_borda, texto_y - tamanho_borda, ALLEGRO_ALIGN_LEFT, menu_itens[i]);
      al_draw_text(fonte_itens, cor_borda, x_pos_desenho_texto - tamanho_borda, texto_y + tamanho_borda, ALLEGRO_ALIGN_LEFT, menu_itens[i]);
      al_draw_text(fonte_itens, cor_borda, x_pos_desenho_texto + tamanho_borda, texto_y + tamanho_borda, ALLEGRO_ALIGN_LEFT, menu_itens[i]);
      al_draw_text(fonte_itens, cor_borda, x_pos_desenho_texto - tamanho_borda, texto_y, ALLEGRO_ALIGN_LEFT, menu_itens[i]);
      al_draw_text(fonte_itens, cor_borda, x_pos_desenho_texto + tamanho_borda, texto_y, ALLEGRO_ALIGN_LEFT, menu_itens[i]);
      al_draw_text(fonte_itens, cor_borda, x_pos_desenho_texto, texto_y - tamanho_borda, ALLEGRO_ALIGN_LEFT, menu_itens[i]);
      al_draw_text(fonte_itens, cor_borda, x_pos_desenho_texto, texto_y + tamanho_borda, ALLEGRO_ALIGN_LEFT, menu_itens[i]);

      al_draw_text(fonte_itens, cor_texto, x_pos_desenho_texto, texto_y, ALLEGRO_ALIGN_LEFT, menu_itens[i]);
    }

    if (i == opcoes && mao != NULL && fonte_itens != NULL)
    {
      float largura_texto_atual = al_get_text_width(fonte_itens, menu_itens[i]);
      float mao_x = x_pos_desenho_texto + largura_texto_atual + espaco_entre_texto_e_mao;
      float mao_largura_original = al_get_bitmap_width(mao);
      float mao_altura_original = al_get_bitmap_height(mao);
      float mao_largura_escalada = mao_largura_original * escala_mao;
      float mao_altura_escalada = mao_altura_original * escala_mao;
      float mao_y = texto_y + (al_get_font_line_height(fonte_itens) / 2.0f) - (mao_altura_escalada / 2.0f);
      
      al_draw_scaled_bitmap(mao, 0, 0, mao_largura_original, mao_altura_original, mao_x, mao_y, mao_largura_escalada, mao_altura_escalada, 0);
    }
  }

  al_flip_display();
}

void menu_opcoes(ALLEGRO_DISPLAY *disp, ALLEGRO_BITMAP *opcoes_background, ALLEGRO_FONT *fonte, ALLEGRO_EVENT_QUEUE *queue,
                 ALLEGRO_SAMPLE_INSTANCE *musica_instancia, int *volume_atual)
{
  bool opcao_selecionada_eh_fullscreen = (al_get_display_flags(disp) & ALLEGRO_FULLSCREEN_WINDOW) ? true : false;
  bool sair_opcoes = false;

  const int LARGURA_JANELA_DESEJADA = 1280;
  const int ALTURA_JANELA_DESEJADA = 800;

  int largura_tela;
  int altura_tela;
  float y_linha_opcao;
  float y_linha_instrucao;
  int selecao_linha = 0;

  while (!sair_opcoes)
  {
    largura_tela = al_get_display_width(disp);
    altura_tela = al_get_display_height(disp);

    if (opcoes_background != NULL) 
    {
      al_draw_scaled_bitmap(opcoes_background, 
                            0, 0, al_get_bitmap_width(opcoes_background), al_get_bitmap_height(opcoes_background),
                            0, 0, largura_tela, altura_tela, 0);
    }

    ALLEGRO_COLOR cor_label_selecionada = al_map_rgb(255, 255, 0);
    ALLEGRO_COLOR cor_label_normal = al_map_rgb(200, 200, 200);
    ALLEGRO_COLOR cor_valor = al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR cor_instrucao = al_map_rgb(200, 200, 200);

    // --- LINHA 1: OPÇÃO DE TELA (SCREEN) ---
    float y_linha_screen = altura_tela / 2.0f - 50;
    // ... (lógica de desenho para a opção de tela, como estava) ...
    const char *texto_label_screen = "Screen:";
    const char *texto_valor_screen = opcao_selecionada_eh_fullscreen ? "< Fullscreen >" : "< Windowed >";
    al_draw_text(fonte, selecao_linha == 0 ? cor_label_selecionada : cor_label_normal, largura_tela / 2.0f - 150, y_linha_screen, ALLEGRO_ALIGN_RIGHT, texto_label_screen);
    al_draw_text(fonte, cor_valor, largura_tela / 2.0f - 130, y_linha_screen, ALLEGRO_ALIGN_LEFT, texto_valor_screen);


    // --- LINHA 2: OPÇÃO DE VOLUME ---
    float y_linha_volume = altura_tela / 2.0f + 10;
    // ... (lógica de desenho para a opção de volume, como estava) ...
    const char *texto_label_volume = "Volume:";
    char texto_valor_volume[8];
    sprintf(texto_valor_volume, "< %d >", *volume_atual);
    al_draw_text(fonte, selecao_linha == 1 ? cor_label_selecionada : cor_label_normal, largura_tela / 2.0f - 150, y_linha_volume, ALLEGRO_ALIGN_RIGHT, texto_label_volume);
    al_draw_text(fonte, cor_valor, largura_tela / 2.0f - 130, y_linha_volume, ALLEGRO_ALIGN_LEFT, texto_valor_volume);

    
    // --- LINHA 3: INSTRUÇÕES (APENAS TEXTO) ---
    float y_base_instrucao = altura_tela * 0.85f - 70; // Posição inicial para o bloco de instruções
    float x_pos_instrucao = largura_tela / 2.0f;  // Posição X centralizada
    float espaco_entre_linhas = al_get_font_line_height(fonte); // Espaço entre as linhas

    
    const char *instrucao_linha_1 = "UP/DOWN - Select | < > - Switch";
    const char *instrucao_linha_2 = "ENTER - Apply | ESC - Back";

    // Desenhando cada linha separadamente
    al_draw_text(fonte, cor_instrucao, x_pos_instrucao, y_base_instrucao, ALLEGRO_ALIGN_CENTER, instrucao_linha_1);
    al_draw_text(fonte, cor_instrucao, x_pos_instrucao, y_base_instrucao + espaco_entre_linhas, ALLEGRO_ALIGN_CENTER, instrucao_linha_2);


    al_flip_display();

    ALLEGRO_EVENT ev;
    al_wait_for_event(queue, &ev);

    if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
    {
      switch (ev.keyboard.keycode)
      {
        case ALLEGRO_KEY_UP:
        case ALLEGRO_KEY_DOWN:
            selecao_linha = (selecao_linha + 1) % 2; // Alterna entre as 2 opções
            break;
        case ALLEGRO_KEY_LEFT:
            if (selecao_linha == 0) { // Alterando SCREEN
                if (opcao_selecionada_eh_fullscreen) opcao_selecionada_eh_fullscreen = false;
            } 
            else if (selecao_linha == 1) { // Alterando VOLUME
                if (*volume_atual > 0) (*volume_atual)--;
                if (musica_instancia) al_set_sample_instance_gain(musica_instancia, (float)(*volume_atual) / 10.0f);
            }
            break;
        case ALLEGRO_KEY_RIGHT:
            if (selecao_linha == 0) { // Alterando SCREEN
                if (!opcao_selecionada_eh_fullscreen) opcao_selecionada_eh_fullscreen = true;
            } 
            else if (selecao_linha == 1) { // Alterando VOLUME
                if (*volume_atual < 10) (*volume_atual)++;
                if (musica_instancia) al_set_sample_instance_gain(musica_instancia, (float)(*volume_atual) / 10.0f);
            }
            break;
        case ALLEGRO_KEY_ENTER:
        {
          if (selecao_linha == 0) { // ENTER só afeta a opção de tela
            bool modo_aplicado = (al_get_display_flags(disp) & ALLEGRO_FULLSCREEN_WINDOW);
            if (opcao_selecionada_eh_fullscreen != modo_aplicado) {
                al_toggle_display_flag(disp, ALLEGRO_FULLSCREEN_WINDOW, opcao_selecionada_eh_fullscreen);
                // CORREÇÃO: Adicionando o resize para corrigir bug da resolução
                if (!opcao_selecionada_eh_fullscreen) { 
                    al_resize_display(disp, LARGURA_JANELA_DESEJADA, ALTURA_JANELA_DESEJADA); 
                }
            }
          }
        }
        break;
        case ALLEGRO_KEY_ESCAPE:
          sair_opcoes = true;
          break;
        default:
          break;
      }
    }
    else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) { sair_opcoes = true; }
  }
}