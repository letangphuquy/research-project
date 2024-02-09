import os
import json
import csv

programs = ['SGA', 'RGA', 'IGA']
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
            if len(fields) != 5:
                continue
            # IGA w13c29 543 174150.511800 1607.971700
            testname = fields[1]
            optimal, time_run, time_input = [float(x) for x in fields[2:]]
            try:
                ratio = int(exact_result[testname]) / int(optimal)
                diff = 1 / ratio - 1
                if testname in result:
                    avg_optimal, avg_diff, avg_time_run, avg_time_input = result[testname]
                    result[testname] = {
                        'optimal': new_avg(avg_optimal, count[testname], optimal),
                        'diff': new_avg(avg_diff, count[testname], diff),
                        'time_run': new_avg(avg_time_run, count[testname], time_run), 
                        'time_input': new_avg(avg_time_input, count[testname], time_input)
                    }
                    count[testname] += 1
                else:
                    result[testname] = {
                        'optimal': optimal,
                        'diff': diff,
                        'time_run': time_run, 
                        'time_input': time_input
                    }
                    count[testname] = 1

            except:
                continue
    def beautify(x): return str(round(100 * x, 2))
    for testname in result.keys():
        result[testname]['diff'] = beautify(result[testname]['diff'])
    return result

def to_csv(field, statistics_data):
    with open(f"{output_path}{field}.csv", "w", newline='') as csvfile:
        writer = csv.writer(csvfile)        
        writer.writerow(["testname", 'SGA', 'RGA', 'IGA'])
        for testname in statistics_data['SGA']:
            try:
                writer.writerow([testname, statistics_data['SGA'][testname][field], statistics_data['RGA'][testname][field], statistics_data['IGA'][testname][field]])
            except:
                continue
statistics_results = {}
for program in programs:
    statistics_results[program] = statistics_for_program(program)

json.dump(statistics_results, open(output_path + 'statistics.json', 'w'))

for field in result_fields:
    to_csv(field, statistics_results)