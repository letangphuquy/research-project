input_table_path = '../statistics_results/diff.csv'
output_table_path = '../statistics_results/diff_latex.txt'

with open(input_table_path, 'r') as csv_file:
    lines = csv_file.readlines()

with open(output_table_path, 'w') as latex_file:
    latex_file.write('\\begin{table}[htbp]\n')
    latex_file.write('    \\centering\n')
    latex_file.write('    \\caption{Caption here}\n')
    latex_file.write('    \\label{tab:my-table}\n')
    cols_str = 'c' * len(lines[0].split(',')) 
    latex_file.write('    \\begin{tabular}{'+cols_str+'}\n')
    latex_file.write('      \\toprule\n')

    for line_index, line in enumerate(lines):
        
        row_data = line.strip().split(',')
        row_data = [data.replace('_', '\\textunderscore ') for data in row_data]
        latex_file.write('      ' + ' & '.join(row_data))
        latex_file.write(' \\\\\n')  
        
        if line_index == 0:
            latex_file.write('      \\midrule\n')

    latex_file.write('      \\bottomrule\n')
    latex_file.write('    \\end{tabular}\n')
    latex_file.write('\\end{table}')
