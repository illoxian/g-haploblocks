#include "utils.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <numeric>
#include <chrono>

/* Returns how many individuals are contained in vcf_file. */
unsigned int get_individual_count(const char *vcf_path)
{
	std::cerr << "Counting number of individuals in vcf file ..." << std::endl;
	std::ifstream vcf_file;
	vcf_file.open(vcf_path);
	if (vcf_file.is_open())
	{
		std::string line;
		while (std::getline(vcf_file, line))
		{
			if (line.front() == '#')
				continue;
			auto split_line = split_string(line, '\t');
			return split_line.size() - 9;
		}
	}
	
	// if nothing was returned ...
	std::cerr << "Could not find or found unexpected content in file " << vcf_path << std::endl;
	return EXIT_FAILURE;
}

/* Reads vcf file from vcf_path and writes binary matrix to out_path */
void vcf_to_binmat(const char *vcf_path, const char *out_path, unsigned int at_once,
	unsigned int &individual_count)
{
	std::ifstream vcf_file;
	std::ofstream out_file;

	// clear output file
	out_file.open(out_path, std::ofstream::out | std::ofstream::trunc);
	out_file.close();

	std::cerr << individual_count << " individuals considered." << std::endl;

	std::vector<std::shared_ptr<std::vector<bool>>> bin_rows;
	for (size_t i = 0; i < at_once*2; ++i)
		bin_rows.push_back(std::make_shared<std::vector<bool>>());

	// the first nine entries of a vcf file line are not of interest here
	unsigned int offset = 9;
    std::vector<unsigned int> indices(individual_count);
    std::iota(indices.begin(), indices.end(), 0);
	
	unsigned int total_runs = individual_count / at_once;
	std::cerr << "Start parsing vcf, will read " << total_runs + 1 << " times in total." << std::endl;
	for (size_t run = 0; run <= total_runs; ++run)
	{
		if (individual_count < run * at_once) break;

		vcf_file.open(vcf_path);
		std::string line;
		while (std::getline(vcf_file, line))
		{
			if (line.front() == '#')
				continue;

			auto split_line = split_string(line, '\t', individual_count+offset);
			// skip everything that is not a SNP
			if (split_line[3].size() != 1 || split_line[4].size() != 1)
				continue;
			
			// extract info from valid line
			unsigned int individual_id = 0;
			for (size_t idx = run * at_once; idx < (run + 1) * at_once; ++idx)
			{
				if (idx >= indices.size()) // we're done
					break;
				
				auto id = indices[idx]+offset;
				if (split_line[id].size() > 3)
					std::cerr << "Got overly long haplotype in run " << run << " for individual "
						<< individual_id << ": " << split_line[id] << std::endl;

				try {
					// chromosome 1 of individual
					bin_rows[2 * individual_id]->push_back(split_line[id].at(0) != '0');
					// chromosome 2 of individual
					bin_rows[2 * individual_id + 1]->push_back(split_line[id].at(2) != '0');
					individual_id++;
				}
				catch (const std::out_of_range& e)
				{
					std::cerr << e.what() << std::endl;
					std::cerr << "Thrown when trying to parse " << split_line[id] << "."
						<< " Possibly not diploid?" << std::endl;
					std::cerr << line << std::endl;
					std::cerr << "Individual: " << individual_id << ", line: "
						<< bin_rows[2 * individual_id]->size() << std::endl;
					exit(EXIT_SUCCESS);
				}
			}
		}
		vcf_file.close();
		
		// write the matrix lines to output file
		out_file.open(out_path, std::ofstream::out | std::ofstream::app);
		for (auto ptr : bin_rows)
		{
			if (ptr->size() == 0) continue;
			for (bool b : *ptr)
				out_file.write(b ? "1" : "0", 1);
			out_file.write("\n", 1);
			ptr->clear();
		}
		out_file.close();

		// print some info to see that we are still up and running
		std::cerr << "Run " << run + 1 << " of " << total_runs + 1 << " completed" << std::endl;
	}
	
	
}

int main(int argc, char *argv[])
{
	// parse arguments
	if (has_cmd_option(argv, argv+argc, "-h") ||
		!has_cmd_option(argv, argv+argc, "-v") ||
		!has_cmd_option(argv, argv+argc, "-o"))
	{
		std::cout << "Call with <prog> -v vcf_file -o out_file [-a at_once] [-h]" << std::endl;
		return EXIT_FAILURE;
	}
	auto vcf_path = get_cmd_option(argv, argv+argc, "-v");
	auto out_path = get_cmd_option(argv, argv+argc, "-o");
	
	unsigned short at_once = 10;
	if (has_cmd_option(argv, argv+argc, "-a"))
		at_once = std::stoi(get_cmd_option(argv, argv+argc, "-a"));
	
	// get relevant indices for pop or super_pop
	auto individual_count = get_individual_count(vcf_path);
    
	// actual computation of binary matrix
	auto begin = std::chrono::steady_clock::now();
	vcf_to_binmat(vcf_path, out_path, at_once, individual_count);
	auto end = std::chrono::steady_clock::now();
	std::cout << "Parsing took (s) " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << std::endl;
	return EXIT_SUCCESS;
}
