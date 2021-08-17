import pickle
import numpy as np
import random

def main():
    with open("dataset/Challenge.fa", "r") as f:
        lines = f.readlines()
    xs = [">B.1.427",">B.1.1.7",">P.1",">B.1.526"]
    # indices = np.loadtxt("data/test_indices.csv").astype("int").tolist()
    indices = np.arange(8000)

    x = []
    y = []
    lens = []
    for i in range(len(lines)):
        if i % 2 == 0:
            x.append(lines[i])
        else:
            tmp = lines[i].split("\n")[0]
            lens.append(len(tmp))
            y.append(tmp)

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
        df_lists.append(df_list)
    
    # ftest = open("dataset/Challenge_test.fa", "w")
    ftest_ans = open("dataset/test_answer.txt", "w")
    for i in indices:
        # ftest.write(str(df_lists[i][0])+"\n")
        # ftest.write(df_lists[i][2]+"\n")
        ftest_ans.write(str(df_lists[i][0])+"\n")
    
    # ftest.close()
    ftest_ans.close()


if __name__ == "__main__":
    main()