#  



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
python maximal_perfect_haplotype_blocks_via_suffix_array.py -h
-----------------------------------------------------------------------------
maximal_perfect_haplotype_blocks_via_suffix_array.py  : Finding all maximal perfect haplotype blocks. 
-----------------------------------------------------------------------------
usage:  maximal_perfect_haplotype_blocks_via_suffix_array.py
  -n number of sequences
  -m: sequence size
  -b: minimal block size
  -h: help
-----------------------------------------------------------------------------
```



