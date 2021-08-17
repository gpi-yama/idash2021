import numpy as np
import pandas as pd
import pickle
from sklearn.decomposition import PCA


def feature_preprocess(comp_dim=5, data_dir="data"):
  executor = PCA(n_components=comp_dim)
  data_file_name = f"{data_dir}/pca_{comp_dim}.pkl"

  with open(f'{data_dir}/numerical_df.pkl', 'rb') as f:
    df = pickle.load(f)

  
  X = np.array(df.drop(["Class"], axis=1).values.tolist())
  X = X.reshape(X.shape[0], -1)
  print(X.shape)
  y = df['Class'].to_numpy()

  executor.fit(X)

  with open(data_file_name, "wb") as f:
    pickle.dump(executor, f)

  np.save(f'{data_dir}/pca_{comp_dim}_mean.npy', executor.mean_.astype("float32"))
  np.save(f'{data_dir}/pca_{comp_dim}_components.npy', executor.components_.reshape(-1).astype("float32"))
  np.save(f'{data_dir}/pca_{comp_dim}_variance.npy', executor.explained_variance_.astype("float32"))
  return X, y

if __name__ == "__main__":
  feature_preprocess()
