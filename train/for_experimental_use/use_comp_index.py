import numpy as np

def get_index(components, percentile=20):
    stds = []
    std = components.reshape(-1, 200, 4).transpose(1, 0, 2).reshape(-1, 4*200).std(axis=1)
    threshold = np.percentile(std, percentile)
    usable_index = np.where(std > threshold)
    print("X_COLUMN:", len(usable_index[0]), threshold)
    return usable_index[0]


if __name__ == "__main__":
    path="data/pca_200_components.npy",
    components = np.load(path)
    usable_index = get_index(components, 20)
    np.save("data/usable_index_200.npy", usable_index.astype("int32"))
    


