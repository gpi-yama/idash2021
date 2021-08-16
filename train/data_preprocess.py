import sys
import numpy as np
import pandas as pd
import pickle

from pandas.core.frame import DataFrame


MAX_NUCLEOTIDE_LENGTH = 30000

"""
IUPAC nucleotide code:	Base
A:  Adenine
C:  Cytosine
G:  Guanine
T:  (or U)	Thymine (or Uracil)
R:  A or G
Y:  C or T
S:  G or C
W:  A or T
K:  G or T
M:  A or C
B:  C or G or T
D:  A or G or T
H:  A or C or T
V:  A or C or G
N:  any base
"""
nucleotide_map = {
  #     A    C    G    T
  "A": [1,   0,   0,   0],
  "C": [0,   1,   0,   0],
  "G": [0,   0,   1,   0],
  "T": [0,   0,   0,   1],
  "R": [0.5, 0,   0.5, 0],   # A or G
  "Y": [0,   0.5, 0,   0.5], # C or T
  "S": [0,   0.5, 0.5, 0],  # G or C
  "W": [0.5, 0,   0,   0.5],  # A or T
  "K": [0,   0,   0.5, 0.5],  # G or T
  "M": [0.5, 0.5, 0,   0],  # A or C
  "B": [0,   0.33,0.33,0.33],  # C or G or T
  "D": [0.33,0,   0.33,0.33],  # A or G or T
  "H": [0.33,0.33,0,   0.33],  # A or C or T
  "V": [0.33,0.33,0.33,0],  # A or C or G
  "N": [0.25,0.25,0.25,0.25],  # any base
  "O": [0,0,0,0],  # any base
}


def onehot_by_nucleotide(df):
  return df.applymap(lambda x: nucleotide_map[x])


def load_df(filename = "dataset/Challenge.fa"):
  with open(filename, "r") as f:
    test = f.readlines()

  x = []
  y = []
  lens = []
  for i in range(len(test)):
    if i % 2 == 0:
      x.append(test[i])
    else:
      tmp = test[i].split("\n")[0]
      lens.append(len(tmp))
      y.append(tmp)

  for i in range(len(y)):
    diff = MAX_NUCLEOTIDE_LENGTH - len(y[i])
    for j in range(diff):
      y[i] += "N"

  xs = [">B.1.427", ">B.1.1.7", ">P.1", ">B.1.526"]
  print(f'seq_len: {len(y[0])}')


  l = []
  c0, c1, c2, c3 = [], [], [], []
  df_lists = []

  for i in range(len(x)):
    df_list = []
    if x[i].startswith(xs[0]):
      df_list.append(0)
      tmp = x[i].split("_")[1].split("\n")[0]
      df_list.append(tmp)
      df_list.append(y[i])
    elif x[i].startswith(xs[1]):
      df_list.append(1)
      tmp = x[i].split("_")[1].split("\n")[0]
      df_list.append(tmp)
      df_list.append(y[i])
    elif x[i].startswith(xs[2]):
      df_list.append(2)
      tmp = x[i].split("_")[1].split("\n")[0]
      df_list.append(tmp)
      df_list.append(y[i])
    elif x[i].startswith(xs[3]):
      df_list.append(3)
      tmp = x[i].split("_")[1].split("\n")[0]
      df_list.append(tmp)
      df_list.append(y[i])
    else:
      raise

    df_lists.append(df_list)

  return df_lists


def process(data_dir="data", map_by_nucleotide=True):
  df_list = load_df()
  columns = ["class", "id", "seq"]
  df = pd.DataFrame(data=df_list, columns=columns)
  print(df.head())


  # each column in a dataframe is called a series
  classes = df.loc[:, 'class']
  classes.value_counts()


  sequences = df.loc[:, 'seq'].tolist()

  dataset = {}

  # Loop throught the sequences and split into individual nucleotides
  for i, seq in enumerate(sequences):
      # split into nucleotides, remove tab characters
      nucleotides = list(seq)
      if map_by_nucleotide:
        nucleotides = [nucleotide_map[x] for x in nucleotides if x != '\t']
      else:  
        nucleotides = [x for x in nucleotides if x != '\t']
      
      # add to dataset
      dataset[i] = nucleotides
      
  df = pd.DataFrame(dataset).T

  with open(f'{data_dir}/df.pkl', 'wb') as f:
    pickle.dump(df, f)

  with open(f'{data_dir}/df.pkl', 'rb') as f:
    df = pickle.load(f)

  if not map_by_nucleotide:
    df = pd.get_dummies(df)
  
  df["Class"] = classes
  
  with open(f'{data_dir}/numerical_df.pkl', 'wb') as f:
    pickle.dump(df, f)


if __name__ == "__main__":
  process()

  

