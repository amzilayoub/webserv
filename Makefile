SRC = main.cpp 											\
	./src/Parser/CharacterReader/CharacterReader.cpp	\
	./src/Parser/Lexer/Lexer.cpp						\
	./src/Parser/Store/Store.cpp						\
	./src/Parser/Parser/Parser.cpp						\
	./src/Config/Config.cpp								\
	./src/Networking/Socket/Socket.cpp					\
	./src/Networking/Server/Server.cpp					\


all:
	clang++ -std=c++98 $(SRC)
