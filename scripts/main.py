import random
import string
import datetime

# Number of records per file
num_records = 100000

file_names = [
    f"/Users/kunaltiwari/TickStream/data/data_part{i}.txt"
    for i in range(17)
]

def random_exchange():
    return random.choice(["NYSE_ARCA", "NSX", "NYSE", "NASDAQ"])

def random_type():
    return random.choice(["Ask", "Bid", "TRADE"])

def random_timestamp():
    base_time = datetime.datetime(2021, 3, 1, 0, 0, 0)
    delta = datetime.timedelta(days=random.randint(0, 15), milliseconds=random.randint(0, 86399999))
    return (base_time + delta).strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]

def generate_market_data_line():
    _timestamp = random_timestamp()
    _price = round(random.uniform(46.0, 47.0), 2)  # Float price
    _size = random.randint(100, 150)  # uint32_t size
    _exchange = random_exchange()  # Exchange name
    _type = random_type()  # Market data type

    # Format the values
    return f"{_timestamp}, {_price}, {_size}, {_exchange}, {_type}\n"

# Generate and write each file
for file_name in file_names:
    with open(file_name, "w") as f:
        print(f"Writing to file: {file_name}")
        for _ in range(num_records):
            f.write(generate_market_data_line())
