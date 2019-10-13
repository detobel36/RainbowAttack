# TODO to final compilation: -O3
all: help

build: gen-passwd check-passwd rainbow-table

# BUILD
gen-passwd:
	g++ -o -O3 gen-passwd -std=c++17 src/random.hpp src/sha256.cpp src/gen-passwd.cpp src/passwd-utils.hpp

check-passwd:
	g++ -o -O3 check-passwd -std=c++17 src/random.hpp src/sha256.cpp src/check-passwd.cpp src/passwd-utils.hpp

rainbow-table:
	g++ -o -O3 rainbow-table -std=c++17 -Wall -Wextra src/random.hpp src/sha256.cpp src/rainbow-table.cpp src/passwd-utils.hpp

# SIMPLIFY COMMANDS
create-passwd: gen-passwd
	mkdir -p test
	./gen-passwd 100 6 6 test/generate_pass.txt test/generate_hash.txt

gen-rainbow-table: rainbow-table
	mkdir -p test
	./rainbow-table -t test/rainbow-table

run-rainbow-table: rainbow-table
	mkdir -p test
	./rainbow-table -s test/rainbow-table -f test/generate_pass.txt -o test/output.txt

clean: clear

clear:
	rm -f rainbow-table check-passwd gen-passwd

help:
	@echo "Commands: make"
	@echo "\tbuild\t\t\tBuild all code"
	@echo "\tgen-passwd\t\tBuild gen-passwd"
	@echo "\tcheck-passwd\t\tBuild check-passwd"
	@echo "\tcreate-passwd\t\tCreate password"
	@echo "\tgen-rainbow-table\tGenerate a rainbow table"
	@echo "\trun-rainbow-table\tTry to find password of generated hash (see 'create-passwd')"
	@echo "\tclear\t\t\tClear build"
	@echo "\tclean\t\t\tSame as clear"
