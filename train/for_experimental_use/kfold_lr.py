import pickle
import numpy as np
import sys
import pandas as pd

from sklearn.model_selection import KFold, train_test_split, cross_val_score
from sklearn.linear_model import LogisticRegression
from sklearn.decomposition import PCA
from sklearn.model_selection import ShuffleSplit
from sklearn.model_selection import cross_val_score

if __name__ == "__main__":
    scores = []
    f = open("kfold_results.txt", "w")
    print("n_component acc")
    data_dir = "data"
    X = np.load(f"{data_dir}/X_embedded.npy", mmap_mode = 'r')
    y = np.load(f"{data_dir}/y.npy", mmap_mode = 'r').astype("int")
    for C in np.arange(1):
        folds = KFold(n_splits=4, random_state=0, shuffle=True)
        executor = PCA(n_components=200)
        executor.whiten = True
        x_feat = executor.fit_transform(X)
        clf = LogisticRegression(tol=0.00001, max_iter=300)
        cv = ShuffleSplit(n_splits=4, random_state=0)
        score = cross_val_score(clf, x_feat, y, cv=cv)
        print(C, np.average(score), file=f, flush=True)
    f.close()


