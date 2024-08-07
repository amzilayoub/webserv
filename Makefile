COMPILER=clang++

FLAGS= -Wall -Wextra -Werror

STD98=-std=c++98

NAME=webserv

SRCS = main.cpp 										\
	./src/Parser/CharacterReader/CharacterReader.cpp	\
	./src/Parser/Lexer/Lexer.cpp						\
	./src/Config/Store/Store.cpp						\
	./src/Parser/Parser/Parser.cpp						\
	./src/Config/Config.cpp								\
	./src/Networking/Socket/Socket.cpp					\
	./src/Networking/Server/Server.cpp					\
	./src/Networking/Client/Client.cpp					\
	./src/Networking/Kqueue/Kqueue.cpp					\
	./src/Networking/Exchange/Header/Header.cpp			\
	./src/Networking/Exchange/Request/Request.cpp		\
	./src/Networking/Exchange/Response/Response.cpp		\
	./src/Utils/split.cpp								\
	./src/Utils/str_to_lower.cpp						\
	./src/Utils/str_to_upper.cpp						\
	./src/Utils/unlink_file.cpp							\
	./src/Utils/replace.cpp								\
	./src/Utils/replace_last.cpp						\
	./src/Utils/ends_with.cpp							\
	./src/Logger/Logger.cpp								\


OBJS=$(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(COMPILER) $(FLAGS) $(STD98) $(OBJS) -o $(NAME)

debug:
	$(COMPILER) $(FLAGS) -g $(STD98) $(SRC) -o $(NAME)

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re : fclean all