import pickle
import numpy as np
import sys
import pandas as pd

from sklearn.model_selection import KFold, train_test_split, cross_val_score
from sklearn.linear_model import LogisticRegression

if __name__ == "__main__":
    data_dir = "data"
    comp_dim = 200
    X = np.load(f"{data_dir}/X_feature.npy")
    y = np.load(f"{data_dir}/y.npy")
    indices = np.arange(len(X))
    usable = np.load(f"{data_dir}/usable_index_200.npy") - 1
    usable_indices = np.zeros(len(X[0]))
    usable_indices[usable] = 1

    ## load trained pca class
    data_file_name = f"{data_dir}/pca_{comp_dim}.pkl"
    with open(data_file_name, "rb") as f:
        executor = pickle.load(f)

    executor.whiten = True
    X = executor.transform(X)
    X = X[usable_indices]
    X_train, X_test, y_train, y_test, train_indices, test_indices = train_test_split(X, y, indices, test_size=0.1, shuffle=True,)
    np.savetxt("{data_dir}/test_indices.csv", test_indices, delimiter=",")
    clf = LogisticRegression(random_state=0, max_iter=300, tol=0.00001, l1_ratio=0.0001).fit(X_train, y_train)
    score = clf.score(X_test, y_test)
    bias = clf.intercept_[:, None]
    coef = clf.coef_

    np.save(f"{data_dir}/coef_{comp_dim}.npy", coef.astype("float32"))
    np.save(f"{data_dir}/bias_{comp_dim}.npy", bias.astype("float32"))

    print("score is:", score)