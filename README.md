# Samantha Tang CMSC701 Project 1

This project was written in C++. 

Multiple sources were consulted for this project:
- I utilized the starter project at [https://github.com/rob-p/cpp_sa_starter_proj](https://github.com/rob-p/cpp_sa_starter_proj).
- I consulted [https://www.w3schools.com/cpp/cpp_files.asp](https://www.w3schools.com/cpp/cpp_files.asp) for information regarding file input/output in C++.
- For more detailed information about binary files, I consulted [https://www.tutorialspoint.com/reading-and-writing-binary-file-in-c-cplusplus](https://www.tutorialspoint.com/reading-and-writing-binary-file-in-c-cplusplus).
- I consulted both [https://cplusplus.com/reference/](https://cplusplus.com/reference/) and [https://en.cppreference.com/](https://en.cppreference.com/) for the names and descriptions of various string and file related functions in C++ that I needed to refresh myself on.
- As a refresher on implementing binary search, I referenced [https://www.geeksforgeeks.org/binary-search/](https://www.geeksforgeeks.org/binary-search/).
- For general troubleshooting or "quick" implementation questions such as "Passing unique_ptr<int[]> to a function", I utilized [https://stackoverflow.com/](https://stackoverflow.com/).
- For information regarding Max RSS and RAM: [https://en.wikipedia.org/wiki/Resident_set_size](https://en.wikipedia.org/wiki/Resident_set_size).

Please note, there are some reference and produced files included in this repository in addition to the code. Within the FASTA directory, there are 3 FASTA files for the genomes I utilized in my testing and report (s.fna, ecoli.fa, and gg.fa). Additionally in the FASTA directory, there are 3 files with queries I utilized for their corresponding reference files (qs.fa for s.fna, qecoli.fa for ecoli.fa, and qgg.fa for gg.fa). Within the outputs directory are the outputs of each part of the project. The .bin files are named to correspond with the FASTA files for which the buildsa executable creates binary files with the appropriate output (these were rewritten for run of builsa whether it included a prefix table or not). The .txt files are the output of the querysa executable which runs the queries from the qs.fa, qecoli.fa, and qgg.fa files using the s.bin, ecoli.bin, and gg.bin files respectively (these were rewritten for run of querysa whether naive or simpaccel is utilized). 