#include <string>
#include <fstream>
#include <unordered_map>
#include <unordered_set>

#define LIST_LEN 4

using namespace std;

string out_path = "results/";
string out_format = ".txt";

struct Prefetch
{
    uint64_t instr_id;
    uint64_t addr;
    //uint64_t level;
    Prefetch(uint64_t id, uint64_t address)
    {
        instr_id = id;
        addr = address;
    }

    bool equals(Prefetch* other)
    {
        return (instr_id == other->instr_id) && (addr == other->addr);
    }
};

class SetTracker
{
    private: 
        unordered_map<uint64_t, unordered_set<uint64_t>*>* unique_addresses;
        string trace_name;

        int num_prefetchers;
        uint64_t* only_me; //unique
        uint64_t** intersection_matrix; //shared
    public:
        SetTracker(string trace)
        {
            unique_addresses  = new unordered_map<uint64_t, unordered_set<uint64_t>*>();
            trace_name = trace;
            
            num_prefetchers = LIST_LEN;
            only_me = new uint64_t[LIST_LEN];
            intersection_matrix = new uint64_t*[LIST_LEN];

            for(int i = 0; i < num_prefetchers; i++)
            {
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
                    unique_addresses->erase(entry.first);
                    delete temp;
                }
            delete unique_addresses;
            for(int i = 0; i < num_prefetchers; i++)
                delete[] intersection_matrix[i];
            delete[] intersection_matrix;
        }  

        void add(uint64_t address, int cur_prefetcher)
        {   
            auto entry = unique_addresses->find(address);
            if(entry == unique_addresses->end())
            {
                auto set = new unordered_set<uint64_t>();
                set->emplace(cur_prefetcher);
                unique_addresses->emplace(address, set);
                only_me[cur_prefetcher]++;
            }
            else
            {
                for(int other: *(entry->second))
                {
                    if(entry->second->size() == 1)
                        only_me[other]--;
                    intersection_matrix[cur_prefetcher][other]++;
                }
                entry->second->emplace(cur_prefetcher);
            }
        }
        
        void output_results()
        {
            ofstream out(out_path + trace_name + out_format);

            out << "Final Count for Trace " << trace_name << endl;
            out << "~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
            for(int i = 0; i < num_prefetchers; i++)
            {
                out << "Unique Prefetches by " << i << ": " << only_me[i] << endl;
            }

            for(int r = 0; r < num_prefetchers; r++)
                for(int c = r + 1; c < num_prefetchers; c++)
                {
                    out << "Prefetches Shared by (" << r << ", " << c << " ): " << intersection_matrix[r][c] + intersection_matrix[c][r] << endl;
                    //in an unordered set, pairs will occur in both orderings
                }

            out.close();
        }
};