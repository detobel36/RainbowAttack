all:
	@echo Hello world !

build: gen-passwd check-passwd

gen-passwd:
	g++ -o gen-passwd -std=c++17 src/random.hpp src/sha256.cpp src/gen-passwd.cpp src/passwd-utils.hpp

check-passwd:
	g++ -o check-passwd -std=c++17 src/random.hpp src/sha256.cpp src/check-passwd.cpp src/passwd-utils.hpp

help:
	@echo "Commands: make"
	@echo "\tbuild\t\tBuild all code"
	@echo "\tgen-passwd\tBuild gen-passwd"
	@echo "\tcheck-passwd\tBuild check-passwd"
	