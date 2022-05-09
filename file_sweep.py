import os

def get_trace(file_name):
    return file_name[:file_name.find('-')]

def get_pref(file_name):
    prefix = get_trace(file_name) + "-perceptron-no-no-"
    truncated = file_name[len(prefix):]
    return truncated[:truncated.find('_') + 1]

def main():
    path = "/scratch/cluster/azheng/pref_zipped_traces/"
    traces = []
    files = []
    prefs = []
    traces_file = open("traces/benchmarks.txt", "w")
    files_file = open("traces/all_files.txt", "w")
    prefs_file = open("traces/prefetchers_KEY.txt", "w")

    # read the entries
    with os.scandir(path) as listOfEntries:
        for entry in listOfEntries:
            if entry.is_file():
                name = entry.name
                traces.append(get_trace(name))
                files.append(name) 
                prefs.append(get_pref(name))
    
    traces = list(set(traces))
    traces.sort()
    files = list(set(files))
    files.sort()
    prefs = list(set(prefs))
    prefs.sort()

    ctr = 0

    for trace in traces:
        traces_file.write(trace + "\n")
    for file in files:
        files_file.write(file + "\n")
    for pref in prefs:
        prefs_file.write(str(ctr) + ": " + pref + "\n")
        ctr = ctr + 1
    
    traces_file.close()
    files_file.close()
    prefs_file.close()

                

main()
