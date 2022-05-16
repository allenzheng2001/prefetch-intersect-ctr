#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <unordered_set>

#define LIST_LEN 21 // number of prefetchers

using namespace std;

string out_path = "results_footprint/";
string out_format = ".txt";

struct Prefetch
{
    //uint64_t instr_id;
    uint64_t addr;
    //uint64_t level;
    Prefetch(uint64_t address)
    {
        //instr_id = id;
        addr = address;
    }

    //hash idea: https://stackoverflow.com/a/64191463
    bool operator==(const Prefetch& other) const
    {
        return this->addr == other.addr;
        //return (this->instr_id == other.instr_id) && (this->addr == other.addr);
    }

    struct HashFunction
    {
        size_t operator()(const Prefetch& prefetch) const
        {
            //size_t inst_idHash = std::hash<uint64_t>()(prefetch.instr_id);
            size_t addrHash = std::hash<uint64_t>()(prefetch.addr);
            return addrHash;
        }
    };
};

class SetTracker
{
    private: 
        unordered_map<Prefetch, unordered_set<uint64_t>*, Prefetch::HashFunction>* unique_addresses;
        string trace_name;

        int num_prefetchers;
        uint64_t* total_by_me; //each prefetcher's total
        uint64_t* only_me; //unique
        uint64_t** intersection_matrix; //shared
    public:
        SetTracker(string trace)
        {
            unique_addresses  = new unordered_map<Prefetch, unordered_set<uint64_t>*, Prefetch::HashFunction>();
            trace_name = trace;
            
            num_prefetchers = LIST_LEN;

            total_by_me = new uint64_t[LIST_LEN];
            only_me = new uint64_t[LIST_LEN];
            intersection_matrix = new uint64_t*[LIST_LEN];

            for(int i = 0; i < num_prefetchers; i++)
            {
                total_by_me[i] = 0;
                only_me[i] = 0;
                intersection_matrix[i] = new uint64_t[LIST_LEN];
                for(int j = 0; j < num_prefetchers; j++)
                    intersection_matrix[i][j] = 0;
            }
        }

        ~SetTracker()
        {
            if(unique_addresses->size() > 0)
                for(auto entry: *unique_addresses)
                {
                    auto temp = entry.second;
                    temp->clear();
                    delete temp;
                }
            unique_addresses->clear();
            delete unique_addresses;
            delete[] total_by_me;
            delete[] only_me;
            for(int i = 0; i < num_prefetchers; i++)
                delete[] intersection_matrix[i];
            delete[] intersection_matrix;
        }  

        void add(Prefetch prefetch, int cur_prefetcher)
        {   
            auto entry = unique_addresses->find(prefetch);
            if(entry == unique_addresses->end())
            {
                auto set = new unordered_set<uint64_t>();
                set->emplace(cur_prefetcher);
                unique_addresses->emplace(prefetch, set);
                only_me[cur_prefetcher]++;
                total_by_me[cur_prefetcher]++;
            }
            else
            {
                if(entry->second->find(cur_prefetcher) != entry->second->end())
                {
                    return;
                }
                for(int other: *(entry->second))
                {
                    if(entry->second->size() == 1)
                    {
                        only_me[other] = (only_me[other] == 0) ? 0: only_me[other] - 1;
                    }
                    intersection_matrix[cur_prefetcher][other]++;
                }
                entry->second->emplace(cur_prefetcher);
                total_by_me[cur_prefetcher]++;
            }
        }

        void random_flag_output()
        {
            ofstream out(out_path + trace_name + out_format);
            
           
            out.close();
        }

        void output_results(vector<string> prefs_list, uint64_t total_prefetches, uint64_t added_prefetches)
        {
            ofstream out(out_path + trace_name + out_format);
            if(added_prefetches > 0)
                out << "**NOTE: This trace had ~1% of its prefetches picked randomly to conserve memory!\n" << endl;

            out << "Final Count for Trace " << trace_name << endl;
            out << "\t Total Prefetches (including duplicates): " << total_prefetches << " Evaluated Prefetches: " << ((added_prefetches == 0) ? total_prefetches : added_prefetches) << endl;    
            out << "\t Total Unique Prefetches By All: " << unique_addresses->size() << endl; 
            out << "~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
            out << endl;

            for(int i = 0; i < num_prefetchers; i++)
            {
                out << "Total Prefetches by " << prefs_list.at(i) << ": " << total_by_me[i] << endl;
            }
            for(int i = 0; i < num_prefetchers; i++)
            {
                out << "Unique Prefetches by " << prefs_list.at(i) << ": " << only_me[i] << endl;
            }

            for(int r = 0; r < num_prefetchers; r++)
                for(int c = r + 1; c < num_prefetchers; c++)
                {
                    out << "Prefetches Shared by (" << prefs_list.at(r) << ", " << prefs_list.at(c) << " ): " << intersection_matrix[r][c] + intersection_matrix[c][r] << endl;
                    //in an unordered set, pairs will occur in both orderings
                }

            out.close();
        }
};