#include "player.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro.h>

#define DIREITA 1
#define ESQUERDA -1

static ALLEGRO_BITMAP *sprite_andando = NULL;
static ALLEGRO_BITMAP *sprite_mirando = NULL;
static ALLEGRO_BITMAP *sprite_pulando = NULL;
static ALLEGRO_BITMAP *sprite_agachado = NULL;
static ALLEGRO_BITMAP *sprite_atacando_pulando = NULL;
static ALLEGRO_BITMAP *sprite_atacando_agachado = NULL;

void player_carregar_sprites() 
{
  sprite_andando = al_load_bitmap("spritesheet andando.png");
  sprite_mirando = al_load_bitmap("spritesheet mirando.png");
  sprite_pulando = al_load_bitmap("spritesheet pulando.png");
  sprite_agachado = al_load_bitmap("spritesheet agachado.png");
  sprite_atacando_pulando = al_load_bitmap("spritesheet pulando e atirando.png");
  sprite_atacando_agachado = al_load_bitmap("spritesheet agachado atirando.png");
}

void player_destruir_sprites() 
{
  al_destroy_bitmap(sprite_andando);
  al_destroy_bitmap(sprite_mirando);
  al_destroy_bitmap(sprite_pulando);
  al_destroy_bitmap(sprite_agachado);
  al_destroy_bitmap(sprite_atacando_pulando);
  al_destroy_bitmap(sprite_atacando_agachado);
}

void inicializar_player(Player *player)
{
    player->x = 100;
    player->y = CHAO_Y; // Posição inicial no "chão"
    player->vida = 100;
    player->ultimo_tiro = 0; // Pode atirar imediatamente
    player->escala = 0.55f;
    player->dy = 0;
    player->no_chao = true;

    player->balas = MAX_BALAS;
    player->recarregando = false;
    player->tempo_recarga = 0.0f;

    player->spritesheet_andando = sprite_andando;
    player->spritesheet_atirando = sprite_mirando;
    player->spritesheet_pulando = sprite_pulando;
    player->spritesheet_agachado = sprite_agachado;
    player->spritesheet_atacando_pulando = sprite_atacando_pulando;
    player->spritesheet_atacando_agachado = sprite_atacando_agachado;

    player->frame_atual = 0;
    player->max_frames = 14; // spritesheet tem 7 frames de animação por linha
    player->tempo_frame = 0;
    player->duracao_frame = 0.1; // Cada frame dura 0.1 segundos
    player->andando = false;
    player->direcao = DIREITA; 
    
    player->frame_largura_andando = 200; 
    player->frame_altura_andando = 365; 
    
    player->frame_largura_mirando = 302;
    player->frame_altura_mirando = 367;

    player->frame_largura_pulo = 301;
    player->frame_altura_pulo = 367;

    player->frame_largura_agachado = 250;
    player->frame_altura_agachado = 337;

    player->frame_largura_atacando_pulando = 350;
    player->frame_altura_atacando_pulando = 370;

    player->frame_largura_atacando_agachado = 302;
    player->frame_altura_atacando_agachado = 337;

    player->linha_atual = 0;

    player->estado_atual = PARADO;

    player->pode_avancar_fase = false;
}

void inicializar_projeteis_player(ProjetilPlayer projeteis[]) {
    for (int i = 0; i < MAX_PROJETEIS_PLAYER; i++) {
        projeteis[i].ativo = false;
    }
}

bool atirar_projetil_player(Player *player, ProjetilPlayer projeteis[], bool esta_agachado)
{
  if (player->recarregando || player->balas <= 0) return false;
  if (al_get_time() - player->ultimo_tiro < COOLDOWN_TIRO) return false;

  for (int i = 0; i < MAX_PROJETEIS_PLAYER; i++) {
    if (!projeteis[i].ativo) {
        projeteis[i].ativo = true;
        projeteis[i].x = (player->direcao == DIREITA) ? player->x + 50 : player->x;
        
        // Usa o novo parâmetro para decidir a altura
        if (esta_agachado) {
            projeteis[i].y = player->y + 60;
        } else {
            projeteis[i].y = player->y + 25;
        }
        
        projeteis[i].dx = (player->direcao == DIREITA) ? 30 : -30;
        player->ultimo_tiro = al_get_time();
        player->balas--;
        if (player->balas <= 0) {
            player->recarregando = true;
            player->tempo_recarga = 0.0f;
        }
        return true; 
    }
  }
  return false;
}

void processar_input_player(Player *player, ALLEGRO_KEYBOARD_STATE *teclado, ProjetilPlayer projeteis[]) 
{
    bool movendo = al_key_down(teclado, ALLEGRO_KEY_LEFT) || al_key_down(teclado, ALLEGRO_KEY_RIGHT);
    bool atacando = al_key_down(teclado, ALLEGRO_KEY_X);
    bool pulando = al_key_down(teclado, ALLEGRO_KEY_UP);
    bool agachado = al_key_down(teclado, ALLEGRO_KEY_DOWN);

    if (pulando && player->no_chao) {
        player->dy = FORCA_PULO;
        player->no_chao = false;
        player->estado_atual = PULANDO;
        player->frame_atual = 0;
        return; 
    }

    if (!player->no_chao) {
        if (atacando) {
            if (atirar_projetil_player(player, projeteis, false)) { // No ar: não está agachado
                player->estado_atual = ATACANDO_PULANDO;
                player->frame_atual = 0;
                player->tempo_no_estado = 0;
            }
        }
    } 
    else {
        if (agachado) {
            if (player->estado_atual == ATACANDO_AGACHADO) {}
            else if (atacando) {
                if (atirar_projetil_player(player, projeteis, true)) { // No chão e agachado: está agachado
                    player->estado_atual = ATACANDO_AGACHADO;
                    player->frame_atual = 0;
                    player->tempo_no_estado = 0;
                }
            }
            else if (movendo) { player->estado_atual = ANDANDO_AGACHADO; } 
            else { player->estado_atual = AGACHADO; }
        } 
        else if (player->estado_atual == AGACHADO || player->estado_atual == ANDANDO_AGACHADO || player->estado_atual == ATACANDO_AGACHADO) {
            player->estado_atual = PARADO;
        }
        else if (atacando) {
            if (atirar_projetil_player(player, projeteis, false)) { // Em pé: não está agachado
                player->estado_atual = ATACANDO;
                player->frame_atual = 0;
                player->tempo_no_estado = 0;
            }
        }
        else if (movendo) { player->estado_atual = ANDANDO; }
        else { player->estado_atual = PARADO; }
    }

    if (al_key_down(teclado, ALLEGRO_KEY_LEFT)) player->direcao = ESQUERDA;
    if (al_key_down(teclado, ALLEGRO_KEY_RIGHT)) player->direcao = DIREITA;
}

void atualizar_player(Player *player, ALLEGRO_KEYBOARD_STATE *teclado) 
{
    // 1. APLICA FÍSICA (GRAVIDADE)
    player->dy += GRAVIDADE;
    player->y += player->dy;

    // 2. VERIFICA COLISÃO COM O CHÃO (LÓGICA DE ATERRISSAGEM)
    if (player->y >= CHAO_Y) {
        player->y = CHAO_Y;
        player->dy = 0;
        player->no_chao = true;
        // Se o estado era aéreo (PULANDO ou ATACANDO_PULANDO), agora que está no chão, volta a ser PARADO.
        if (player->estado_atual == PULANDO || player->estado_atual == ATACANDO_PULANDO) {
            player->estado_atual = PARADO;
        }
    } else {
        // Se não está no chão, está no ar.
        player->no_chao = false;
    }

    // 3. ATUALIZA O TEMPO PARA ANIMAÇÃO
    player->tempo_frame += 1.0 / 60.0;

    // 4. ATUALIZA A RECARGA (se aplicável)
    if (player->recarregando) 
    {
      player->tempo_recarga += 1.0f / 60.0f;
      if (player->tempo_recarga >= 2.0f) 
      {
        player->balas = MAX_BALAS;
        player->recarregando = false;
        player->tempo_recarga = 0.0f;
      }
    }

    // 5. ATUALIZA ANIMAÇÃO E POSIÇÃO BASEADO NO ESTADO ATUAL (sem mais lógicas conflitantes)
    switch (player->estado_atual) {
        case ANDANDO:
            player->x += VELOCIDADE_PLAYER * player->direcao;
            if (player->tempo_frame >= player->duracao_frame) {
              player->tempo_frame -= player->duracao_frame;
              player->frame_atual = (player->frame_atual + 1) % 14; 
            }

            break;

        case ATACANDO:
            player->tempo_no_estado += 1.0 / 60.0;
            if (player->tempo_frame >= player->duracao_frame) {
              player->tempo_frame -= player->duracao_frame;
              player->frame_atual = (player->frame_atual + 1) % 3;
            }
            if (player->tempo_no_estado > 0.3) {
                player->estado_atual = PARADO;
            }

            break;

        case ATACANDO_AGACHADO:
            player->tempo_no_estado += 1.0 / 60.0;
            player->frame_atual = 0;

            // Após a animação, retorna para o estado AGACHADO
            if (player->tempo_no_estado > 0.3) {
                player->estado_atual = AGACHADO;
            }

            break;
            
        case AGACHADO:
            player->frame_atual = 0;
            
            break;

    case ANDANDO_AGACHADO:
        // Move o personagem com a nova velocidade reduzida
        player->x += VELOCIDADE_AGACHADO * player->direcao;
        
        // Animação de andar agachado
        if (player->tempo_frame >= player->duracao_frame) {
            player->tempo_frame -= player->duracao_frame;

            // ---- LÓGICA ATUALIZADA PARA 5 FRAMES DE ANIMAÇÃO (1 a 5) ----
            // Se o frame atual não está no range da animação (1-5), reinicia no 1
            if (player->frame_atual < 1 || player->frame_atual > 5) {
                player->frame_atual = 1;
            } else {
                // Avança para o próximo frame e volta para o 1 se passar do 5
                player->frame_atual++;
                if (player->frame_atual > 5) {
                    player->frame_atual = 1;
                }
            }
        }
        break;

        case ATACANDO_PULANDO:
            player->tempo_no_estado += 1.0 / 60.0;
            player->frame_atual = 0; // Frame único
            if (al_key_down(teclado, ALLEGRO_KEY_RIGHT)) player->x += VELOCIDADE_PLAYER * player->direcao;
            if (al_key_down(teclado, ALLEGRO_KEY_LEFT)) player->x += VELOCIDADE_PLAYER * player->direcao;
            if (player->tempo_no_estado > 0.3) player->estado_atual = PULANDO;
            break;    

        case PULANDO:
            // Animação do pulo (sobe e para no último frame de pulo)
            if (player->tempo_frame >= player->duracao_frame) {
              if (player->frame_atual < 3) player->frame_atual++;
              player->tempo_frame -= player->duracao_frame;
            }
            // Controle no ar
            if (al_key_down(teclado, ALLEGRO_KEY_RIGHT)) player->x += VELOCIDADE_PLAYER;
            if (al_key_down(teclado, ALLEGRO_KEY_LEFT)) player->x -= VELOCIDADE_PLAYER;
            break; 

        case PARADO:
            player->frame_atual = 0;
            break;
    }

    if (player->x <= -10) 
    {
      player->x = -10;
    }
    
    if (!player->pode_avancar_fase) 
    {
      if (player->x > LARGURA_NIVEL - (player->frame_largura_andando * player->escala / 2))
      {
        player->x = LARGURA_NIVEL - (player->frame_largura_andando * player->escala / 2);
      }
    }

}



void atualizar_projeteis_player(ProjetilPlayer projeteis[], float camera_x) {
    for (int i = 0; i < MAX_PROJETEIS_PLAYER; i++) {
        if (projeteis[i].ativo) {
            projeteis[i].x += projeteis[i].dx;
            // Desativar projétil se sair da área visível da câmera
            if (projeteis[i].x < camera_x - 100 || projeteis[i].x > camera_x + 2000) { // 1280 é a largura da tela
                projeteis[i].ativo = false;
            }
        }
    }
}

void desenhar_player(Player *player, float camera_x) 
{
    ALLEGRO_BITMAP *sheet_usado = NULL;
    int largura_frame = 0;
    int altura_frame = 0;
    int fx = 0, fy = 0; // Source X e Source Y no spritesheet
    float deslocamento_y = 0;

    // Decide qual spritesheet e dimensões usar baseado no estado
    switch (player->estado_atual) {
        case PARADO:
            sheet_usado = player->spritesheet_andando;
            largura_frame = player->frame_largura_andando;
            altura_frame = player->frame_altura_andando;
    
            break;

        case ANDANDO:
            sheet_usado = player->spritesheet_andando;
            largura_frame = player->frame_largura_andando;
            altura_frame = player->frame_altura_andando;
            
            // --- CÁLCULO DA LINHA E COLUNA PARA ANDAR (14 frames) ---
            // O seu spritesheet tem 7 frames por linha.
            int coluna_andar = player->frame_atual % 7; // Módulo 7 nos dá a coluna (0 a 6)
            int linha_andar = player->frame_atual / 7;  // Divisão inteira por 7 nos dá a linha (0 ou 1)
            
            fx = coluna_andar * largura_frame;
            fy = linha_andar * altura_frame;

            break;

        case AGACHADO:
            sheet_usado = player->spritesheet_agachado;
            largura_frame = player->frame_largura_agachado;
            altura_frame = player->frame_altura_agachado;
            // Frame 0 é o personagem agachado e parado
            fx = 0 * largura_frame;
            fy = 0;
            deslocamento_y = 28;

            break;

        case ANDANDO_AGACHADO:
          sheet_usado = player->spritesheet_agachado;
          largura_frame = player->frame_largura_agachado;
          altura_frame = player->frame_altura_agachado;
          deslocamento_y = 28;

          // ---- LÓGICA SIMPLIFICADA PARA UMA ÚNICA LINHA ----
          // Como todos os frames estão na mesma linha, fy é sempre 0.
          // A coluna (fx) é simplesmente o frame atual multiplicado pela largura.
          fx = player->frame_atual * largura_frame;
          fy = 0;

          break;

        case ATACANDO_AGACHADO:
            sheet_usado = player->spritesheet_atacando_agachado;
            largura_frame = player->frame_largura_atacando_agachado;
            altura_frame = player->frame_altura_atacando_agachado;
            deslocamento_y = 28; 

            break;          


        case ATACANDO_PULANDO:
            sheet_usado = player->spritesheet_atacando_pulando; 
            largura_frame = player->frame_largura_atacando_pulando;
            altura_frame = player->frame_altura_atacando_pulando;
            
            break;

        case ATACANDO:
            sheet_usado = player->spritesheet_atirando;
            largura_frame = player->frame_largura_mirando;
            altura_frame = player->frame_altura_mirando;
            
            // A animação de ataque tem apenas 3 frames em uma única linha
            int coluna_ataque = player->frame_atual; // frame_atual aqui vai de 0 a 2
            
            fx = coluna_ataque * largura_frame;
            fy = 0; // Sempre a primeira (e única) linha

            break;

        case PULANDO: 
            sheet_usado = player->spritesheet_pulando;
            largura_frame = player->frame_largura_pulo;
            altura_frame = player->frame_altura_pulo;
            fx = player->frame_atual * largura_frame;

            break;
    }   

    if (sheet_usado) 
    {
        int flag = (player->direcao == -1) ? ALLEGRO_FLIP_HORIZONTAL : 0;
        float dx = player->x - camera_x;
        float dy = player->y; 
        float largura_desenho = largura_frame * player->escala;
        float altura_desenho = altura_frame * player->escala;

        al_draw_scaled_bitmap(sheet_usado, fx, fy, largura_frame, altura_frame,
                              dx, dy + 145 + deslocamento_y, largura_desenho, altura_desenho, flag);
    }
}

void desenhar_projeteis_player(ProjetilPlayer projeteis[], float camera_x, ALLEGRO_BITMAP *flecha) {
    // --- CONTROLE DE TAMANHO ---
    // 1.0 = tamanho original, 2.0 = dobro do tamanho, etc.
    // Experimente valores como 1.5, 2.0, 2.5 para ver o que fica melhor.
    float escala_projetil = 1.5f;

    for (int i = 0; i < MAX_PROJETEIS_PLAYER; i++) {
        if (projeteis[i].ativo) {
            if (flecha) {
                float px = projeteis[i].x - camera_x;
                float py = projeteis[i].y;

                // Dimensões originais da imagem da flecha
                float largura_original = al_get_bitmap_width(flecha);
                float altura_original = al_get_bitmap_height(flecha);

                // Novas dimensões baseadas na escala
                float largura_escalada = largura_original * escala_projetil;
                float altura_escalada = altura_original * escala_projetil;
                
                // Posição para desenhar (centralizando o sprite escalado)
                float pos_x = px - (largura_escalada / 2.0f) + 50;
                float pos_y = py - (altura_escalada / 2.0f) + 180;

                // Desenha o bitmap com a nova escala
                int flag = (projeteis->dx  < 0) ? ALLEGRO_FLIP_HORIZONTAL : 0;
                al_draw_scaled_bitmap(flecha, 
                                      0, 0, largura_original, altura_original, // Região de origem (imagem inteira)
                                      pos_x, pos_y, largura_escalada, altura_escalada, // Destino e tamanho na tela
                                      flag); // Flags
            }
        }
    }
}
