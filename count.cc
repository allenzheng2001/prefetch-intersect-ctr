#include "gzstream.h"
#include "count.h"

#include <string>
#include <bits/stdc++.h>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <assert.h>
#include <cstdlib>
#include <iostream>
#include <limits>

#include <unordered_map>
#include <unordered_set>

using namespace std;

static bool endsWith(const string& str, const string& suffix)
{
    // For detecting file extensions
    // https://stackoverflow.com/questions/874134/find-out-if-string-ends-with-another-string-in-c
    return str.size() >= suffix.size() && 0 == str.compare(str.size()-suffix.size(), suffix.size(), suffix);
}

int addr_from_str(string s)
{
    int ctr = 0;
    stringstream str(s);
    string cur_token;
    while(getline(str, cur_token, ' '))
    {
        if(++ctr == 2)
            return stoi(cur_token);
    }
    return -1;
}

int main()
{  
    string test = "test_inputs/";
    string base = ".txt";
    string PATH = "/scratch/cluster/azheng/pref_zipped_traces/";
    string files[LIST_LEN] = {
        "astar_163B-perceptron-no-no-bingo_1-ship-1core-2048llc_sets_llc.gz",
        "astar_163B-perceptron-no-no-bingo,scooby_1,1-ship-1core-2048llc_sets_llc.gz",
        "astar_163B-perceptron-no-no-bingo,sisb_1,8-ship-1core-2048llc_sets_llc.gz",
        "astar_163B-perceptron-no-no-bingo,sms_1,7-ship-1core-2048llc_sets_llc.gz"
    };

    SetTracker* tracker = new SetTracker("test");
    //SetTracker* tracker = new SetTracker("astar_test");
    //cout << "Testing Isect Counter on Astar" << endl;

    for(int i = 0; i < LIST_LEN; i++)
    {
        // igzstream cur_zip;
        // cur_zip.open((PATH + files[i]).c_str());
        ifstream cur_fp;
        cur_fp.open((test+to_string(i+1)+base).c_str());

        string input_line;
        int ctr = 0;

        while(getline(cur_fp, input_line))
        {
            // if(ctr++ > 10)
            //     break;
            // cout << input_line << endl;
            auto cur_addr = stoi(input_line);
            tracker->add(cur_addr, i);
        }
    }

    tracker->output_results();
    delete tracker;

    return 0;
}