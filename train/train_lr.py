import pickle
import numpy as np
import sys
import pandas as pd

from sklearn.model_selection import KFold, train_test_split, cross_val_score
from sklearn.linear_model import LogisticRegression

from feature_preprocess import feature_preprocess

if __name__ == "__main__":
  ## load data
  comp_dim = 200
  data_dir = "data"
  X, y = feature_preprocess(comp_dim=comp_dim)

  data_file_name = f"{data_dir}/pca_{comp_dim}.pkl"
  with open(data_file_name, "rb") as f:
    executor = pickle.load(f)

  executor.whiten = True
  X = executor.transform(X)
  clf = LogisticRegression(random_state=0, max_iter=300, tol=0.00001).fit(X, y)
  score = clf.score(X, y)
  bias = clf.intercept_[:, None]
  coef = clf.coef_

  np.save(f"{data_dir}/coef_{comp_dim}.npy", coef.astype("float32"))
  np.save(f"{data_dir}/bias_{comp_dim}.npy", bias.astype("float32"))

  print("score is:", score)
