all:
	@echo Hello world !

build: gen-passwd check-passwd

gen-passwd:
	g++ -o gen-passwd -std=c++17 src/random.hpp src/sha256.cpp src/gen-passwd.cpp src/passwd-utils.hpp

check-passwd:
	g++ -o check-passwd -std=c++17 src/random.hpp src/sha256.cpp src/check-passwd.cpp src/passwd-utils.hpp

test-pass: gen-passwd
	@echo "Generate password and hash"
	@echo "Next step is to generate the rainbow table"
	./gen-passwd 5 6 6 test/generate_pass.txt test/generate_hash.txt

help:
	@echo "Commands: make"
	@echo "\tbuild\t\tBuild all code"
	@echo "\tgen-passwd\tBuild gen-passwd"
	@echo "\tcheck-passwd\tBuild check-passwd"
	@echo ""
	@echo "\ttest-pass\tTest generate password"
