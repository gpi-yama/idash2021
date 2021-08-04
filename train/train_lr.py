import pickle
import numpy as np
import sys
import pandas as pd

from sklearn.model_selection import KFold, train_test_split, cross_val_score
from sklearn.linear_model import LogisticRegression

from feature_preprocess import feature_preprocess

if __name__ == "__main__":
  ## load data
  data_dir = "data"
  X, y = feature_preprocess()

  ## load trained pca class
  data_file_name = f"{data_dir}/pca_200.pkl"
  with open(data_file_name, "rb") as f:
    executor = pickle.load(f)

  executor.whiten = True
  X = executor.transform(X)
  np.save(f"{data_dir}/X_feature.npy", X)
  X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.25, shuffle=True,)

  clf = LogisticRegression(random_state=0, max_iter=300).fit(X_train, y_train)
  score = clf.score(X_test, y_test)
  bias = clf.intercept_[:, None]
  coef = clf.coef_

  np.save(f"{data_dir}/coef.npy", coef)
  np.save(f"{data_dir}/bias.npy", bias)

  print("score is:", score)
