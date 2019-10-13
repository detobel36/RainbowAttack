# TODO to final compilation: -O3
all:
	@echo Hello world !

install:
	@echo TODO !

build: gen-passwd check-passwd rainbow-table

gen-passwd:
	g++ -o gen-passwd -std=c++17 src/random.hpp src/sha256.cpp src/gen-passwd.cpp src/passwd-utils.hpp

check-passwd:
	g++ -o check-passwd -std=c++17 src/random.hpp src/sha256.cpp src/check-passwd.cpp src/passwd-utils.hpp

rainbow-table:
	g++ -o rainbow-table -std=c++17 -Wall -Wextra src/random.hpp src/sha256.cpp src/rainbow-table.cpp src/passwd-utils.hpp

test-pass: gen-passwd
	@echo "Generate password and hash"
	./gen-passwd 5 6 6 test/generate_pass.txt test/generate_hash.txt
	@echo "Next step is to generate the rainbow table"

test-check: check-passwd rainbow-table
	@echo "Run the search"
	# Generate result in "test/results.txt"
	./rainbow-table -s test_table -f test/generate_hash.txt -o test/results.txt
	@echo "Check the results"
	./check-passwd test/generate_pass.txt test/results.txt

#test: clean rainbow-table
#	./rainbow-table -h

clean-test:
	rm -f test-repartition

test: clean-test
	 g++ -o test-repartition -std=c++17 -Wall -Wextra src/sha256.cpp src/test-repartition.cpp
	./test-repartition

clean: clear

clear:
	rm -f rainbow-table check-passwd gen-passwd

help:
	@echo "Commands: make"
	@echo "\tbuild\t\tBuild all code"
	@echo "\tinstall\t\tInstall all required packages"
	@echo "\tgen-passwd\tBuild gen-passwd"
	@echo "\tcheck-passwd\tBuild check-passwd"
	@echo ""
	@echo "\ttest-pass\tTest generate password and the Rainbow table"
	@echo "\ttest-check\tTry to find hash of password and check"
