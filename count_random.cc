#include "count.h"

#include <string>
#include <stdlib.h> 
#include <time.h>  
#include <bits/stdc++.h>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <assert.h>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <vector>

#include <unordered_map>
#include <unordered_set>

using namespace std;

int main()
{  
    // string test = "test_inputs/";
    // string base = ".txt";
    string PATH = "/scratch/cluster/azheng/pref_zipped_traces/";

    ifstream names_fp("traces/benchmarks.txt");
    ifstream large_names_fp("traces/large_benchmarks.txt");
    ifstream prefs_fp("traces/prefetchers_KEY.txt");
    ifstream files_fp("traces/all_files.txt");
    
    vector<string> benchmarks;
    unordered_set<string> large_benchmarks;
    vector<string> prefs;
    vector<string> files;

    string benchmark_name;
    string large_benchmark_name;
    string pref_name;
    string file_name;

    srand(1337); // random number generator, if needed for large traces

    while(getline(names_fp, benchmark_name))
        benchmarks.push_back(benchmark_name);

    while(getline(large_names_fp, large_benchmark_name))
        large_benchmarks.emplace(large_benchmark_name);

    while(getline(prefs_fp, pref_name))
        prefs.push_back(pref_name);

    while(getline(files_fp, file_name))
        files.push_back(file_name);

    names_fp.close();
    large_names_fp.close();
    prefs_fp.close();
    files_fp.close();

    ofstream out("traces/progress_random.txt");

    for(int cur_benchmark = 0; cur_benchmark < benchmarks.size(); cur_benchmark++)
    {
        string cur_trace = benchmarks.at(cur_benchmark);
        SetTracker* tracker = new SetTracker(cur_trace);
        bool rand_flag = (large_benchmarks.find(cur_trace) != large_benchmarks.end());
        
        if(rand_flag)
            out << "Testing Isect Counter on " << cur_trace << " randomly sampling 1%" << endl;
        else
            out << "Testing Isect Counter on " << cur_trace << endl;

        uint64_t instr_id;
        uint64_t address;

        uint64_t added_prefetches = 0;
        uint64_t total_prefetches = 0;


        for(int cur_prefetcher = 0; cur_prefetcher < LIST_LEN; cur_prefetcher++)
        {
            out << "Isect Counter Reading " << cur_trace << " for " << prefs.at(cur_prefetcher) << endl;
            ifstream cur_fp;
            string cur_file = files.at(cur_benchmark*LIST_LEN + cur_prefetcher);

            out << "File: " << cur_file << endl;

            cur_fp.open(PATH + cur_file.c_str());

            string input_line;

            while(getline(cur_fp, input_line))
            {
                stringstream line_stream(input_line);

                uint64_t instr_id = 0;
                string token;
                int token_num = 0;

                while(getline(line_stream, token, ' ')) {  // Parse line (format: `instr_id call_id pc addr1,addr2,...,addrn level1,level2,...,leveln`)
                    stringstream token_stream(token);

                    if (token_num == 0) {        // Get instruction ID
                        instr_id = stol(token);
                    } else if (token_num == 1) { // Get call ID
                        
                    } else if (token_num == 2) { // Get PC

                    } else if (token_num == 3) { // Get addresses
                        string addr_str;
                        uint64_t addr;

                        while (getline(token_stream, addr_str, ',')) {
                            addr = stol(addr_str, nullptr, 16);
                            total_prefetches++;
                            if(rand_flag)
                            {
                                int random = rand() % 100;
                                if(random < 1)
                                {
                                    tracker->add(Prefetch(instr_id, addr), cur_prefetcher);
                                    added_prefetches++;
                                }
                            }
                            else
                            {
                                tracker->add(Prefetch(instr_id, addr), cur_prefetcher);
                            }
                        }
                    } else if (token_num == 4) { // Get levels

                    }
                    token_num++;
                }
            }
            cur_fp.close();
        }

        tracker->output_results(prefs, total_prefetches, added_prefetches);
        delete tracker;
    }
    
    out.close();
    return 0;
}
