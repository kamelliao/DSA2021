import os
import math
import random
import time
import numpy as np
import pandas as pd
from subprocess import Popen, PIPE, STDOUT

class Data():
    def __init__(self, max_n, max_q, mode):
        self.n = random.randint(1, max_n)
        self.q = random.randint(1, max_q)
        self.arr = list(np.random.choice(np.arange(-100000, 100000), self.n , replace=True))
        self.input = f'{self.n} {self.q}\n{" ".join([str(item) for item in self.arr])}'
        self.instructions = list()
        self.answers = list()
        self.mode = mode
        self.idx_query = list()

        if mode == 'q_only':
            instruction_set = ['Query']
        else:
            instruction_set = ['Query', 'Insert', 'Delete', 'Reverse']

        for i in range(self.q):
            if (self.n > 0):
                instruction = random.choice(instruction_set)
            else:
                instruction = 'Insert'
            
            if instruction == 'Query':
                start = random.randint(1, self.n)
                end = random.randint(start, self.n)
                k = random.randint(1, end - start + 1)

                self.instructions.append(f'Query {start} {end} {k}')
                self.answers.append(self.query(start, end, k))
                self.idx_query.append(i)
            elif instruction == 'Insert':
                position = random.randint(1, self.n + 1)
                element = random.randint(-100000, 100000)

                self.insert(position, element)
                self.instructions.append(f'Insert {position} {element}')
            elif instruction == 'Delete':
                position = random.randint(1, self.n)

                self.delete(position)
                self.instructions.append(f'Delete {position}')
            else:
                start = random.randint(1, self.n)
                end = random.randint(start, self.n)

                self.reverse(start, end)
                self.instructions.append(f'Reverse {start} {end}')
    
    def query(self, start, end, k):
        return sorted(self.arr[start-1:end])[k - 1]

    def insert(self, position, element):
        self.arr.insert(position - 1, element)
        self.n += 1

    def delete(self, position):
        self.arr.pop(position - 1)
        self.n -= self.n
    
    def reverse(self, start, end):
        self.arr = self.arr[:start - 1] + self.arr[start - 1:end][::-1] + self.arr[end:]
    
    def str_input(self):
        str_ins = "\n".join(self.instructions)
        
        return f'{self.input}\n{str_ins}'


def get_output(command, inputs):
    program = Popen(command, stdout=PIPE, stdin=PIPE, stderr=STDOUT)
    output = program.communicate(input=f'{inputs}'.encode('utf-8'))[0]
    program.terminate()
    
    return [int(i) for i in output.decode().split('\r\n')[:-1]]

def test(config, WA):
    # generate test data
    data = Data(config['max_n'], config['max_q'], config['mode'])
    
    # get output
    start = time.time()
    output = get_output(config['command'], data.str_input())
    end = time.time()
    runtime = end - start

    # test correctness
    error = 0
    for i, (out, ans) in enumerate(zip(output, data.answers)):
        if out != ans:
            error += 1
            instructions_without_other_Q = "\n".join([i for i in data.instructions[:data.idx_query[i]] if i.find('Query') == -1] + [data.instructions[data.idx_query[i]]])
            WA = WA.append({'input': data.input, 'instructions': instructions_without_other_Q, 'answers': ans, 'output': out}, ignore_index=True)

    return runtime, error, WA

def execute(max_n, max_q, repeat, mode='all'):
    logs = dict()
    WA = pd.DataFrame(columns=['input', 'instructions', 'answers', 'output'])
    for i in range(repeat):
        config = {'max_n': max_n, 'max_q': max_q, 'mode': mode, 'command': ["main.exe"]}
        runtime, error, WA = test(config, WA)
        logs[i] = {'error': error, 'time': runtime}

        # show progress
        if (i % (repeat // 10) == 0):
            print(f'處理中 {round(((i / repeat) + 0.1)*100, 3)}%')

    err = np.sum([log['error'] for _, log in logs.items()])
    avg_time = np.mean([log['time'] for _, log in logs.items()])

    if (err > 1):
        WA.to_csv('WA.csv')

    print(f'發現 {err} 個錯誤, 平均執行時間為 {round(avg_time, 3)} 秒')

if __name__ == '__main__':
    inputs = input("輸入n q repeat mode('q_only'或'all'): ").split(' ')
    max_n = int(inputs[0])
    max_q = int(inputs[1])
    repeat = int(inputs[2])
    mode = inputs[3]

    execute(max_n, max_q, repeat, mode)