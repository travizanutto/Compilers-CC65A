.PHONY: all clean

all: src/lex.l src/parse.y
	cd build/ ; cmake . ; make
	cd ..

clean:
	rm -rf bin/*
	mkdir tmp
	mv build/CMakeLists.txt tmp
	rm -rf build/*
	mv tmp/CMakeLists.txt build
	rm -rf tmp