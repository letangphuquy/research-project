import os
import json
import csv

programs = ['SGA', 'RGA', 'IGA']
# resf << program_name << " " << testname << " " << optimal << " " << time_run/1e6 << " " << time_input/1e6 << '\n';
result_fields = ['optimal', 'time_run', 'time_input']

input_path = '../tests_results/'
output_path = '../statistics_results/'
exact_result_path = '../statistics_results/crawled_result.json'
exact_result = json.load(open(exact_result_path, 'r'))

if not os.path.exists(output_path):
    os.makedirs(output_path)

def statistics_for_program(program_name):
    result = {}
    path = input_path + 'results_'+program_name + '.txt'
    with open(path, 'r') as file:
        lines = file.readlines()
        for line in lines:
            fields = line.split(' ')
            if len(fields) != 5:
                continue
            # IGA w13c29 543 174150.511800 1607.971700
            testname = fields[1]
            optimal = fields[2]
            time_run = fields[3]
            time_input = fields[4]
            time_input = time_input.split('\n')[0]
            try:
                result[testname] = {'optimal': optimal + ' ' + str(round(int(exact_result[testname])/int(optimal)*100,2)) + '%',
                                     'time_run': time_run, 'time_input': time_input}
            except:
                continue
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