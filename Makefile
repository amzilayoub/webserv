SRC = main.cpp 											\
	./src/Parser/CharacterReader/CharacterReader.cpp	\
	./src/Parser/Lexer/Lexer.cpp						\
	./src/Parser/Store/Store.cpp


all:
	clang++ $(SRC)
