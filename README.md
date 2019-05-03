# Finding all maximal perfect haplotype blocks

## Problem definition

Given a *matrix_file* of *k* sequences, each of length *n*, over the alphabet {0,1}, find all maximal haplotype blocks (exact repeats starting at the same index position in two or more sequences) of size (height x width) at least *b*.

## Output

Each maximal haplotype block is represented by a line

    size (start-end,witness:#sequences)

where
 - *start*: the first column of the block
 - *end*: the last column of the block
 - *witness*: a line corresponding to this block
 - *#sequences*: number of sequences of the block
 - *size*: the size of the block ( = (*end*-*start*+1) x *#sequences*)


## Example

For the following sequences

> 1 1 1 0 0 1 1 0 1 0 1 0 1 1 0  
> 0 0 1 0 0 0 1 1 0 1 0 1 0 0 0  
> 0 0 1 0 0 1 1 1 0 1 0 1 0 0 0  
> 0 1 0 0 0 1 0 0 1 1 1 0 1 0 0

We have *k = 4* and *n = 15*. For *b = 10*, we have **3** maximal haplotype blocks:
 - 10 (0-4,1:2)
 > 1 1 1 0 0 1 1 0 1 0 1 0 1 1 0  
 > **<span style="color:#74c151">0 0 1 0 0</span>** 0 1 1 0 1 0 1 0 0 0  
 > **<span style="color:#74c151">0 0 1 0 0</span>** 1 1 1 0 1 0 1 0 0 0  
 > 0 1 0 0 0 1 0 0 1 1 1 0 1 0 0
 - 10 (2-6,2:2)
 > 1 1 **<span style="color:#74c151">1 0 0 1 1</span>** 0 1 0 1 0 1 1 0  
 > 0 0 1 0 0 0 1 1 0 1 0 1 0 0 0  
 > 0 0 **<span style="color:#74c151">1 0 0 1 1</span>** 1 0 1 0 1 0 0 0  
 > 0 1 0 0 0 1 0 0 1 1 1 0 1 0 0
 - 18 (6-14,1:2)
 > 1 1 1 0 0 1 1 0 1 0 1 0 1 1 0  
 > 0 0 1 0 0 0 **<span style="color:#74c151">1 1 0 1 0 1 0 0 0</span>**  
 > 0 0 1 0 0 1 **<span style="color:#74c151">1 1 0 1 0 1 0 0 0</span>**  
 > 0 1 0 0 0 1 0 0 1 1 1 0 1 0 0

# Algorithms and implementations

## Trie algorithm

Compiling

```bash
# "Go to the directory"
cd trie
# "Compile"
make
```

Find blocks

```bash
./haploblocks <matrix_file> <b>
```

Example

```bash
./haploblocks ../tests/example-small.bm 10
 ```

## Suffix Array algorithm

This python script implements the simplest solution ever (?). 

1. Addition of distinct flags each '*b*' positions in all sequences: thus the same flag occurs at the same position in all sequences
2. Concatenation of all these decorated sequences in a sequence *s*
3. Creation of the suffix array of *s* (including the LCP)
4. Detection of maximal repeats of size at least *b* 

Find blocks

```bash
python haplotype-sa.py -f <matrix_file> -b <b> -o <outfile>
```

## Direct algorithm

```bash
# "Go to the directory"
cd direct
# "Compile"
make
```

Find block

```bash
./hap -f <matrix_file> -b <b> -m <row_rise_type>
```
where *row_rise_type* corresponds how you want to read your data column by column. 
 - **mmap**: *mmap* functions
 - **precomp**: precompute all the data in the ram
 - **seek**: use seek and get
 - **col**:  

Example

 ```bash
 ./hap -f ../tests/example-small.bm -b 10
 ```

## pBWT algorithm

Compiling

```bash
# "Go to the directory"
cd pbwt
# "Compile"
make
```

Find block

```bash
./haplotype-pbwt-lite -f <matrix_file> -b <b> -n <buffer_size>
```
where *buffer_size* corresponds to the size of buffer of each file. The maximum number of file that you need corresponds to *x = k+1*, you can increase the number with


```bash
ulimit -n <x>
```

Example

 ```bash
 ./haplotype-pbwt-lite -f ../tests/example-small.bm -b 10
 ```

## Generate data

Generating file

```bash
# "Go to the directory"
cd generate
# "Generate file"
python gen_haplotype.py Directory/ m n error_rate
```

Example

 ```bash
 python gen_haplotype.py ../tests/ 4 20 1
 ```

## General Makefile

```bash
# Compile all the c++ files
make
# Test all the implementations
make test
```
