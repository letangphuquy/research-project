import os
import requests
import threading
import tarfile
from bs4 import BeautifulSoup

testset_list_url = "https://steinlib.zib.de/download.php"
url = "https://steinlib.zib.de/"

output_path = '../tests_temp/'

if not os.path.exists(output_path):
    os.makedirs(output_path)

def download_test(testname):
    try:
        response = requests.get(url + testname)
        testname = testname.split("/")[-1]
        with open(f"{output_path}{testname}", "wb") as file:
            file.write(response.content)
        print(f"Downloaded {testname}")
        with tarfile.open(f"{output_path}{testname}") as tar:
            tar.extractall(f"{output_path}")
        print(f"Extracted {testname}")
        os.remove(f"{output_path}{testname}")
    except:
        print(f"Failed to download {testname}")
        raise


response = requests.get(testset_list_url)
soup = BeautifulSoup(response.text, "html.parser")
table = soup.find_all("table")[0]

href_list = []
for row in table.find_all("tr")[1:]:
    columns = row.find_all("td")
    href = columns[2].find("a").get("href")
    if href[-4:] == ".tgz":
        href_list.append(href)
print(href_list)

threads = []
for href in href_list:
    thread = threading.Thread(target=download_test, args=(href,))
    thread.start()
    threads.append(thread)
for thread in threads:
    thread.join()

# for href in href_list:
#     download_test(href)

