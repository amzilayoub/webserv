SRC = main.cpp 											\
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
	./src/Logger/Logger.cpp								\


all:
	clang++ -fsanitize=address -std=c++98 $(SRC)

debug:
	clang++ -g -fsanitize=address -std=c++98 $(SRC)

find:
	lsof -t -i:80 
