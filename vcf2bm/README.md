vcf2bm - small C++ program to convert .vcf files to a matrix of 0/1 for haploblocks (only SNPs).

Build: type "make" in the folder.

Usage: "./vcf2bm -v <vcf_file> -o <output_file> [-a at_once] [-h]"

Arguments:
-v <vcf_file>: Mandatory. Path to the vcf file that should be converted. While this is intended for data provided by phase 3 of the 1000 genomes project [ftp://ftp.1000genomes.ebi.ac.uk/vol1/ftp/release/20130502/], any file formatted in the same way will work. That means the comment field of a vcf line should consists solely of the phased calls (e.g. "0|0") of the contained individuals, separated by tabs. Note that only raw vcf files are accepted by the program. The vcf file may contain non-SNPs, these will be ignored.
-o <output_file>: Mandatory. Path to the output file. File will be created if it does not yet exist, otherwise it will be overwritten.
-a <at_once>: Optional integer value. This is how many individuals are read in one iteration over the vcf file. Higher values mean that the program will be faster but use more RAM. Default value is 10.
-h: Optional. Prints short help.
