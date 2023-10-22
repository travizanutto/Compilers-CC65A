.SILENT: clean

all: src/lex.l src/parse.y
	@mkdir jason
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