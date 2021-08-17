import numpy as np
import pandas as pd
from sklearn.metrics import roc_auc_score


def acc(pred, ans):
    s = np.sum(pred==ans)
    print(s, len(pred))
    return np.sum(pred==ans)/len(pred)

def valid():
    df = pd.read_csv("output_path/result.csv", header=0, index_col=0)
    ans = np.loadtxt("dataset/test_answer.txt", dtype="int")
    pred = df.to_numpy()
    print(roc_auc_score(ans, pred, average="weighted", multi_class="ovr"))

if __name__ == "__main__":
    valid()
    # pred = np.loadtxt("output_path/result.txt", dtype="str")
    # ans = np.loadtxt("dataset/test_answer.txt", dtype="str")
    # print(acc(pred, ans))
