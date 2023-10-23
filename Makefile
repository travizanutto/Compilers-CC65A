.SILENT: clean

all: src/lex.l src/parse.y src/ast.c src/ast.h
	@mkdir -p bin/jason
	@cd build/ ; cmake . ; make -s
	@cd ..

clean:
	rm -rf bin/*
	rm -rf out/*
	mkdir tmp
	mv build/CMakeLists.txt tmp
	rm -rf build/*
	mv tmp/CMakeLists.txt build
	rm -rf tmp
