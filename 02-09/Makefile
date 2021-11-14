.PHONY: lint

a.out: main.cpp ../external/lodepng/lodepng.cpp *.h
	clang++ -Ofast -ltbb -std=c++17 main.cpp ../external/lodepng/lodepng.cpp

lint: main.cpp ../external/lodepng/lodepng.cpp *.h
	clang-format -i *.cpp *.h
