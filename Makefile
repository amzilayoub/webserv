SRC = main.cpp 											\
	./src/Parser/CharacterReader/CharacterReader.cpp	\
	./src/Parser/Lexer/Lexer.cpp						\
	./src/Parser/Store/Store.cpp						\
	./src/Parser/Parser/Parser.cpp						\
	./src/Config/Config.cpp								\
	./src/Networking/Socket/Socket.cpp					\


all:
	clang++ -g -std=c++98 $(SRC)
