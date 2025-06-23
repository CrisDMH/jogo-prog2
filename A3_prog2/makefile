# Nome do executável
TARGET = JOGO

# Compilador
CC = gcc

# Flags de compilação do Allegro
ALLEGRO_FLAGS = $(shell pkg-config --libs --cflags allegro-5 allegro_main-5 allegro_font-5 allegro_ttf-5 allegro_primitives-5 allegro_image-5 allegro_audio-5 allegro_acodec-5)

# Arquivos-fonte do projeto
SRCS = main.c menu.c player.c fase1.c hud.c inimigos.c boss.c

# Regra padrão
all: $(TARGET)

# Como construir o executável
$(TARGET): $(SRCS)
	$(CC) $(SRCS) -o $(TARGET) $(ALLEGRO_FLAGS) -lm

# Limpeza de arquivos gerados
clean:
	rm -f $(TARGET)