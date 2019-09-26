all:
	@echo Hello world !

build: gen-passwd

gen-passwd:
	g++ -o gen-passwd -std=c++17 random.hpp sha256.cpp gen-passwd.cpp passwd-utils.hpp

help:
	@echo "Commands: make"
	@echo "\tbuild\t\tBuild all code"
	@echo "\tgen-passwd\tBuild gen-passwd"
