# Finding all maximal perfect haplotype blocks in linear time

Given *n* sequences each of size *m* on the alphabet {0,1}, find all Maximal Perfect Haplotype Blocks [1] of size at least *b*. 

[1] Cunha, L., Diekmann, Y., Kowada, L., & Stoye, J. (2018, October). Identifying Maximal Perfect Haplotype Blocks. In *Brazilian Symposium on Bioinformatics* (pp. 26-37). Springer, Cham.

This python script implements the simplest solution ever (?). 

1. Addition of distinct flags each '*b*' positions in all sequences: thus the same flag occurs at the same position in all sequences
2. Concatenation of all these decorated sequences in a sequence *s*
3. Creation of the suffix array of *s* (including the LCP)
4. Detection of maximal repeats of size at least *b* 

## Usage: 

```bash
python haplotype-sa.py -h
-----------------------------------------------------------------------------
haplotype-sa.py  : Finding all maximal perfect haplotype blocks. 
-----------------------------------------------------------------------------
usage:  haplotype-sa.py
  -f inputfile (one line per sequence. n sequences (n lines) of the same length [Mandatory]
  -b: minimal block size (>1) [Mandatory]
  -o: output file name
  -h: help
-----------------------------------------------------------------------------
```
