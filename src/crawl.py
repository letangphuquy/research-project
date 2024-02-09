import requests
from bs4 import BeautifulSoup
import json
import os

url = "https://steinlib.zib.de/"
testset_list_url = "https://steinlib.zib.de/testset.php"

output_path = '../statistics_results/'

def get_test_info(testset_url):
    response = requests.get(testset_url)
    soup = BeautifulSoup(response.text, "html.parser")
    table = soup.find_all("table")[0]
    result = {}
    try:
        for row in table.find_all("tr")[1:]:
            columns = row.find_all("td")
            result[columns[0].get_text()[1:]] = columns[5].get_text()[:-1]
            result[columns[0].get_text()[1:].lower()] = columns[5].get_text()[:-1]
            result[columns[0].get_text()[1:].upper()] = columns[5].get_text()[:-1]
    except:
        print(testset_url)
    return result

response = requests.get(testset_list_url)
soup = BeautifulSoup(response.text, "html.parser")

href_list = []
for link in soup.find_all("a"):
    href = link.get("href")
    href_list.append(href)

href_list = href_list[3:-2]

result = {}

for href in href_list:
    result.update(get_test_info(url + href))
if not os.path.exists(output_path):
    os.makedirs(output_path)
with open(f"{output_path}crawled_result.json", "w") as file:
    file.write(json.dumps(result, indent=4))


