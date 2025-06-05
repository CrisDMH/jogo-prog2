#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>

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

// Em menu.c
// ... (includes e outras funções) ...

void menu_opcoes(ALLEGRO_DISPLAY *disp, ALLEGRO_BITMAP *opcoes_background, ALLEGRO_FONT *fonte, ALLEGRO_EVENT_QUEUE *queue)
{
  bool opcao_selecionada_eh_fullscreen = (al_get_display_flags(disp) & ALLEGRO_FULLSCREEN_WINDOW) ? true : false;
  bool sair_opcoes = false;

  const int LARGURA_JANELA_DESEJADA = 1280;
  const int ALTURA_JANELA_DESEJADA = 800;

  int largura_tela;
  int altura_tela;
  float y_linha_opcao;
  float y_linha_instrucao;

  ALLEGRO_COLOR cor_label = al_map_rgb(255, 255, 0); // Amarelo para "Screen:"
  ALLEGRO_COLOR cor_valor = al_map_rgb(255, 255, 255); // Branco para "Fullscreen"/"Windowed"
  ALLEGRO_COLOR cor_instrucao = al_map_rgb(200, 200, 200); // Cinza para instruções

  while (!sair_opcoes)
  {
    largura_tela = al_get_display_width(disp);
    altura_tela = al_get_display_height(disp);

    y_linha_opcao = altura_tela / 2 - 30;
    y_linha_instrucao = altura_tela / 2 + 300;

    if (opcoes_background != NULL) 
    {
      al_draw_scaled_bitmap(opcoes_background, 
                            0, 0, al_get_bitmap_width(opcoes_background), al_get_bitmap_height(opcoes_background),
                            0, 0, largura_tela, altura_tela, 0);
    }

    const char *texto_label = "Screen:";
    const char *texto_valor = opcao_selecionada_eh_fullscreen ? "< Fullscreen" : "Windowed >";

    float largura_label = al_get_text_width(fonte, texto_label);
    float espaco_entre_label_e_valor = 10;
    float largura_valor = al_get_text_width(fonte, texto_valor);
    float largura_total_opcao = largura_label + espaco_entre_label_e_valor + largura_valor;
        
    float x_inicio_label = (largura_tela / 2) - (largura_total_opcao / 2);
    float x_inicio_valor = x_inicio_label + largura_label + espaco_entre_label_e_valor;

    al_draw_text(fonte, cor_label, x_inicio_label, y_linha_opcao, ALLEGRO_ALIGN_LEFT, texto_label);
    al_draw_text(fonte, cor_valor, x_inicio_valor, y_linha_opcao, ALLEGRO_ALIGN_LEFT, texto_valor);

    // Atualiza o texto de instrução
    al_draw_text(fonte, cor_instrucao, largura_tela / 2, y_linha_instrucao, ALLEGRO_ALIGN_CENTER, "< > - Select | ENTER - Apply | ESC - Back");

    al_flip_display();

    ALLEGRO_EVENT ev;
    al_wait_for_event(queue, &ev);

    if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
    {
      switch (ev.keyboard.keycode)
      {
        case ALLEGRO_KEY_LEFT:
        // Apenas alterna a SELEÇÃO para "Windowed" se "Fullscreen" estiver selecionado
        if (opcao_selecionada_eh_fullscreen)
        {
          opcao_selecionada_eh_fullscreen = false;
        }
        break;
        case ALLEGRO_KEY_RIGHT:
        // Apenas alterna a SELEÇÃO para "Fullscreen" se "Windowed" estiver selecionado
        if (!opcao_selecionada_eh_fullscreen)
        {
          opcao_selecionada_eh_fullscreen = true;
        }
        break;
        case ALLEGRO_KEY_ENTER:
        {
          // Verifica qual é o modo de tela REALMENTE aplicado no momento
          bool modo_aplicado_no_display_eh_fullscreen = (al_get_display_flags(disp) & ALLEGRO_FULLSCREEN_WINDOW) ? true : false;
                        
          // Só tenta alterar o modo da tela se a opção selecionada for DIFERENTE do modo já aplicado
          if (opcao_selecionada_eh_fullscreen != modo_aplicado_no_display_eh_fullscreen)
          {
            al_toggle_display_flag(disp, ALLEGRO_FULLSCREEN_WINDOW, opcao_selecionada_eh_fullscreen);
            // Após tentar aplicar, a variável opcao_selecionada_eh_fullscreen agora reflete
            // o novo estado desejado (e esperançosamente aplicado).
            // A próxima leitura de al_get_display_flags() confirmaria.
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
    else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
    {
      sair_opcoes = true;
    }
  }
}
