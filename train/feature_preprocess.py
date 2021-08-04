import numpy as np
import pandas as pd
import pickle
from sklearn.decomposition import PCA


def feature_preprocess(comp_dim=200, data_dir="data"):
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
  
  np.savetxt(f'{data_dir}/pca_{comp_dim}_mean.csv', executor.mean_)
  np.savetxt(f'{data_dir}/pca_{comp_dim}_components.csv', executor.components_)
  np.savetxt(f'{data_dir}/pca_{comp_dim}_variance.csv', executor.explained_variance_)

  return X, y

if __name__ == "__main__":
  feature_preprocess()