import random
import math
from suffix_array import suffix_array_ManberMyers,linear_lcp
import sys
import getopt
import time


def usage():
    '''Usage'''
    print("-----------------------------------------------------------------------------")
    print(sys.argv[0]," : Finding all maximal perfect haplotype blocks. ")
    print("-----------------------------------------------------------------------------")
    print("usage: ",sys.argv[0])
    print("  -f inputfile (one line per sequence. n sequences (n lines) of the same length [Mandatory]")
    print("  -b: minimal block size (>1) [Mandatory]")
    print("  -o: output file name")
    print("  -h: help")
    print("-----------------------------------------------------------------------------")
    sys.exit(2)




tabletters="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"   #needed for generating flags


### Generates a random sequence of 0/1s of size n. Not used but exists for testing purposes
def random01(n):
    word=""
    for i in range(n):
        word+=random.choice("01")
    return word
    
### Debug purpose
def print_sa(SA,LCP,s):
    print ("i\tSA\tBWT\tLCP\tsuffix")
    for i in range (len(s)):
        print (i,"\t",SA[i],'\t',s[SA[i]-1],'\t',LCP[i],'\t',s[SA[i]:SA[i]+40])


seqs = []                                   # sequences of 0/1. Random generated foro testing purposes or initialized in the main function
new_seqs =[]                                # horrible global variable for word generation
previous_pos = 0                            # horrible global variable for word generation
min_len_blocks = 0                          # horrible global variable for word generation

### Given input sequences, add every min_len_blocks position a distinct flag (aa,ab, ac, ba, bb, bc, ca, cb, cc, ...)
def add_flags(n,word=""):
    global seqs             #sorry
    global new_seqs         #sorry (again)
    global previous_pos     #and... sorry
    if previous_pos>len(seqs[0]): 
        return
    if n == 0:
        for i in range(len(seqs)):
            new_seqs[i]+=word+seqs[i][previous_pos:previous_pos+min_len_blocks]
            # seqs[i]=seqs[i][:pos]+word+seqs[i][pos:]
        previous_pos+=min_len_blocks
        
    else:
        for i in range(len(tabletters)):
            add_flags(n-1,word+tabletters[i])

### Creation fo the SA and LCP of the concatenation of the sequences
def create_sa(seqs):
    
    print ("\tconcatenate sequences ")
    b=time.clock()
    cat_seqs="$"
    for seq in seqs:
        cat_seqs+=seq+"$"
    del seqs
    print (f"\t\tconcatenated in {round(time.clock()-b,4)} seconds")
    
    
    print ("\tSA with ManberMyers construction ")
    b=time.clock()
    SA=suffix_array_ManberMyers(cat_seqs)
    print (f"\t\tcomputed in {round(time.clock()-b,4)} seconds")
    
    print ("\tLCP construction ")
    b=time.clock()
    LCP=linear_lcp(cat_seqs,SA)
    print (f"\t\tcomputed in {round(time.clock()-b,4)} seconds")    
    
    return SA,LCP,cat_seqs

        
### given an lcp-interval (righ maximal repeat) checks if it's left maximal (a least 2 distinct previous characters)
def is_interval_left_maximal(interval,s,SA):
    #interval = lcp, i_start, i_stop
    bwts=set()
    for j in range(interval[1],interval[2]+1):
        bwts.add(s[SA[j]-1])
        if len(bwts)>1: return True
    return False
    
### As flags are coded using several characters, one has to check if a maximal repeats is not truncated in the middle of a flag. 
def contains_a_full_flag(interval,s,SA, min_len_blocks, size_flags):
    pos=SA[interval[1]]
    if s[pos] in "01": # we start by a 0 or a 1 and the size of the repeat had already been tested as > min_len_blocks+size_flags
        return True
        
    # here we have to differenciate the case:
    # ..a0010b.. (eg with size_flags==2). In this case the repeat does not contain a full flag. 
    # from 
    # ..a0010ba01 (still with size_flags==2). In this case the repeat contain a full flag. 
    
    len_first_flag=0
    while s[pos+len_first_flag] not in "01":
        len_first_flag+=1
    if len_first_flag==size_flags: return True                      # starts by a full flag
    return (interval[0]-len_first_flag)>(min_len_blocks+size_flags) # check the rest of the sequence once removed the non-full first flag
    
### Adding flags we modify the sequence position. This functions retreives a position in the original sequences
def from_sa_pos_to_real_pos (pos_in_decorated_concatenated,size_decorated_seq,min_len_blocks,size_flags):
    pos_in_decorated=pos_in_decorated_concatenated%(size_decorated_seq+1)   # position in its own decorated sequence. 
                                                                            # The '+1' accounts for the initial '$' of each sequence on the concatenation
                                                                            
    if pos_in_decorated==0: return 0
    pos_in_decorated-=1                                                     # remove the first '$'
    block_id=pos_in_decorated//(min_len_blocks+size_flags)                  # a block is a flag+sequence of 01
    # in block id i, we have seen ixsize_flags letters corresponding to flags in previous blocks: 
    pos_bloc_in_non_decorated=block_id*min_len_blocks
    # in a bloc id: if we are in the first 'size_flags' characters, we are in the flag, the reported position i the first on the right with no flag.
    #               else, we remove the flag size of our own block:
    pos_in_bloc=pos_in_decorated%(min_len_blocks+size_flags)
    if pos_in_bloc>size_flags:
        return pos_bloc_in_non_decorated+pos_in_bloc-size_flags
    else:
        return pos_bloc_in_non_decorated
    
### Print a Maximal Hap Block
def print_interval(interval,s,SA,min_len_blocks,size_decorated_seq,size_flags):
    seq01 = ""
    shift_seq_id=0
    for l in s[SA[interval[1]]:SA[interval[1]]+interval[0]]:
        if l in "01": seq01+=l
        if l=="$" :                                             # end of the sequence. 
            if len(seq01) < min_len_blocks:                     # the repeat length is not sufficient. We start back on next sequence
                seq01=""
                shift_seq_id=1                                  # the sequence id will be shifted by one
            else: break                                         # we already found a repeat is sufficient size that do not span two sequences
    if len(seq01) < min_len_blocks: return                      # the repeat length is not sufficient. Possible if the repeat spans two sequences
    print ("pos ",from_sa_pos_to_real_pos(SA[interval[1]],size_decorated_seq,min_len_blocks,size_flags),"",end="")

    print (" len ", len(seq01),"",end="")
    print (" sequences ", end='')
    for j in range(interval[1],interval[2]+1):
        print (shift_seq_id+(SA[j]//size_decorated_seq),"",end='')
    print()

### print maximal repeats (todo: avoid repeats that overlap the concatenation of two sequences)
### "Replacing suffix trees with enhanced suffix arrays"
### Mohamed Ibrahim Abouelhoda Stefan Kurtz Enno Ohlebusch
### https://doi.org/10.1016/S1570-8667(03)00065-0
def get_maximal_repeats(SA,LCP,s,size_decorated_seq,min_len_blocks,size_flags):
    min_decorated_repeat_len=min_len_blocks+size_flags              # A reapeat must be of size min_len_blocks+size_flags. This is a (unsufficient) necessary condition to contain a flag.
    stack=[]
    stack.append([0,0,None])
    LCP.append(-1)
    for i in range (1,len(s)+1):
        lb=i-1
        while len(stack)>0 and LCP[i]<stack[-1][0]:
            stack[-1][2]=i-1        
            interval=stack.pop()
            # if interval[0]>=min_decorated_repeat_len:
            #     print(min_decorated_repeat_len)
            #     print(interval)
            #     print(is_interval_left_maximal(interval,s,SA))
            #     print (contains_a_full_flag(interval,s,SA,min_len_blocks,size_flags))
            if interval[0]>=min_decorated_repeat_len and is_interval_left_maximal(interval,s,SA) and contains_a_full_flag(interval,s,SA,min_len_blocks,size_flags):
                print_interval(interval,s,SA,min_len_blocks,size_decorated_seq,size_flags)
            lb = interval[1]
        if len(stack)>0 and LCP[i]>stack[-1][0]:
            stack.append([LCP[i],lb,None])
    

### Load sequences from file. 
def get_seqs(sequence_file_name):
    seqs=[]
    sequence_file = open(sequence_file_name)
    size_seq=-1
    for line in sequence_file:
        seqs.append(line.strip())
        if size_seq!=-1:
            assert len(line.strip()) == size_seq, "all sequences must have the same length"
        else:
            size_seq=len(line.strip())
    return seqs,size_seq
                

def main():
    global seqs
    global pos
    global min_len_blocks
    global new_seqs
    try:
        opts, args = getopt.getopt(sys.argv[1:], "hf:b:o:" )
    except getopt.GetoptError as err:
        # print help information and exit:
        print(str(err))  # will print something like "option -a not recognized"
        usage()
        sys.exit(2)
    
    #### parameters
    sequence_file_name=None
    size_seq=None                                 # 0/1 seq size
    min_len_blocks=None                           # minimal size of searched sequences
    name_file_out=None
    for opt, arg in opts:
        if opt in ("-h", "--help"):
            usage()
            sys.exit()

        elif opt in ("-o"):
            name_file_out = arg
        elif opt in ("-f"):
            sequence_file_name = arg
        elif opt in ("-b"):
            min_len_blocks = int(arg)
            assert min_len_blocks>1, "min len block must be >1"
                
        else:
            assert False, "unhandled option"
            
    if sequence_file_name == None or min_len_blocks == None:
        print("options -f and -b are all mandatory")
        usage()
        sys.exit(2)
        
    print ("load sequences ")
    b=time.clock()
    seqs,size_seq=get_seqs(sequence_file_name)
    for i in range (len(seqs)):
        new_seqs.append("")
    print (f"\t loaded in {round(time.clock()-b,4)} seconds")
    
    pos=size_seq
    nb_flags=size_seq/min_len_blocks+1                              # determine the number of needed flags 
    size_flags=math.ceil(math.log(nb_flags,len(tabletters)))        # determine the length of the flags. Can be improved by using also smaller flgs
    
    print ("decorate sequences ")
    b=time.clock()
    add_flags(size_flags)                                           # add the flags to the set of sequences new_seqs
    print (f"\tdecorated in {round(time.clock()-b,4)} seconds")
    del seqs
    
    print ("create suffix array & LCP ")
    SA,LC,s=create_sa(new_seqs)                                     # generate SA & LCPs from the concatenation of new_seqs
    print (f"\tSA & LCP created in {round(time.clock()-b,4)} seconds")
    size_decorated_seq = len(new_seqs[0])

    print ("find maximal repeats ")
    b=time.clock()
    if name_file_out:
        prev_out=sys.stdout
        sys.stdout = open(name_file_out, 'w')
    get_maximal_repeats(SA,LC,s, size_decorated_seq,min_len_blocks,size_flags) # show the maximal repeats of length at leat min_len_blocks
    if name_file_out:
        sys.stdout.close()
        sys.stdout=prev_out
    print (f"\tfound in {round(time.clock()-b,4)} seconds")
    
if __name__ == "__main__":
    main()
                   

