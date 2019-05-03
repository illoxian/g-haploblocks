

all: trie-compile pbwt-compile direct-compile

clean: trie-clean

remove: trie-remove pbwt-remove direct-remove test-clean

test: generate see-file test-sa test-trie test-pbwt test-direct
	
test-clean:
	rm -f tests/hap_gen*

generate:
	python generate/gen_haplotype.py tests/ 4 15 1 >/dev/null

see-file:
	cat tests/hap_gen_4_15.txt

trie-compile:
	$(MAKE) -C trie/

trie-clean:
	$(MAKE) -C trie/ clean

trie-remove:
	$(MAKE) -C trie/ remove

test-trie:
	trie/haploblocks tests/hap_gen_4_15.txt 5

pbwt-compile:
	$(MAKE) -C pbwt/

pbwt-remove:
	$(MAKE) -C pbwt/ remove

test-pbwt:
	pbwt/haplotype-pbwt-lite -f tests/hap_gen_4_15.txt -b 5 2> /dev/null	

test-sa:
	python suffix_arrays/haplotype-sa.py -f tests/hap_gen_4_15.txt -b 5

direct-compile:
	$(MAKE) -C direct/

direct-remove:
	$(MAKE) -C direct/ clean
	
test-direct:
	direct/hap -f tests/hap_gen_4_15.txt -b 5 2> /dev/null

.PHONY: all trie generate test
