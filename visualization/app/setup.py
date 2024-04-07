import json

class setup:
    def __init__(self, filename):
        f = open(filename)
        data = json.load(f)
        print(data)