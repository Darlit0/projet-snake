# Makefile

NAME = snake

SRCS = main.c

all :  $(NAME)

$(NAME): $(SRCS)
	gcc $(SRCS) -o $(NAME) `sdl2-config --cflags --libs` -lSDL2_image -lSDL2_mixer

fclean:
	rm -f $(NAME)

re: fclean all