import os
import json
import csv

programs = ['SGA', 'RGA', 'IGA', 'IGA_F']
# resf << program_name << " " << testname << " " << optimal << " " << time_run/1e6 << " " << time_input/1e6 << '\n';
result_fields = ['optimal', 'diff', 'time_run', 'time_input']

input_path = '../tests_results/'
output_path = '../statistics_results/'
exact_result_path = '../statistics_results/crawled_result.json'
exact_result = json.load(open(exact_result_path, 'r'))

if not os.path.exists(output_path):
    os.makedirs(output_path)

def new_avg(old_avg, old_count, new_val): 
    return (old_avg * old_count + new_val) / (old_count + 1)

def statistics_for_program(program_name):
    result = {}
    count = {}
    path = input_path + 'results_'+program_name + '.txt'
    with open(path, 'r') as file:
        lines = file.readlines()
        for line in lines:
            fields = line.split(' ')
            if len(fields) != 5: continue
            # IGA w13c29 543 174150.511800 1607.971700
            testname = fields[1]
            if testname not in exact_result: continue # noise
            optimal, time_run, time_input = [float(x) for x in fields[2:]]
            try:
                # print(line)
                ratio = int(exact_result[testname]) / int(optimal)
                diff = 1 / ratio - 1
                new_value = {
                    'optimal': int(optimal),
                    'diff': float(diff),
                    'time_run': float(time_run), 
                    'time_input': float(time_input)
                }
                if testname in result: 
                    for key in new_value.keys():
                        result[testname][key] = new_avg(result[testname][key], count[testname], new_value[key])
                    count[testname] += 1
                else:
                    result[testname] = new_value
                    count[testname] = 1

            except:
                continue
    def beautify(x): return str(round(x, 2))
    for testname in result.keys():
        # print(program_name, ' run ', testname, ' for ', count[testname], ' times')
        result[testname]['diff'] *= 100
        for key in result[testname].keys():
            result[testname][key] = beautify(result[testname][key])
    return result

testnames_path = "../tests_temp/"
testnames = []
testsets = os.listdir(testnames_path)
for testset in testsets:
    files = os.listdir(testnames_path + testset)
    for file in files:
        if file[-4:] == '.stp':
            testnames.append(file[:-4])

print(testnames)

focused_programs = ['SGA', 'IGA', 'IGA_F']
focused_programs = ['IGA', 'IGA_F']
def to_csv(field, statistics_data):
    with open(f"{output_path}{field}.csv", "w", newline='') as csvfile:
        writer = csv.writer(csvfile)
        header_row = ['testname']
        header_row.extend(focused_programs)        
        writer.writerow(header_row)
        for testname in testnames:
            try:
                row = [testname]
                for progname in focused_programs: 
                    row.append(statistics_data[progname][testname][field])
                writer.writerow(row)
            except:
                continue
        print(f"Written to file {field}")

statistics_results = {}
for program in programs:
    statistics_results[program] = statistics_for_program(program)

json.dump(statistics_results, open(output_path + 'statistics.json', 'w'))

for field in result_fields:
    to_csv(field, statistics_results)