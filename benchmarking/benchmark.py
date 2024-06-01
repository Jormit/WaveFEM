from scipy.io import mmread
from matspy import spy

file_A = "benchmarking\Horn Antenna\A.mtx"
file_b = "benchmarking\Horn Antenna\A.mtx"

A = mmread(file_A)
b = mmread(file_b)

spy(A, buckets=1000)

print(A[0,0])

