import random

# Number of records per file
num_records = 1000000


file_names = []
for i in range(10000):
    file_name = f"data_part{i}.txt"
    file_names.append(file_name)

def generate_market_data_line():
    # Each field is a random uint32_t value (0 to 2^32-1)
    # You can adjust the ranges as needed.
    timestamp = random.randint(1, 1_000_000)
    token = random.randint(1000, 9999)
    bidPrice = random.randint(100, 500)
    bidQuantity = random.randint(1, 1000)
    askPrice = random.randint(101, 600)
    askQuantity = random.randint(1, 1000)
    lastTradedPrice = random.randint(100, 500)
    lastTradedQuantity = random.randint(1, 1000)

    # Format the values as a single space-separated line
    return f"{timestamp} {token} {bidPrice} {bidQuantity} {askPrice} {askQuantity} {lastTradedPrice} {lastTradedQuantity}\n"

# Generate and write each file
for file_name in file_names:
    with open(file_name, "w") as f:
        for _ in range(num_records):
            f.write(generate_market_data_line())
