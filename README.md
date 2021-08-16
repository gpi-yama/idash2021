# IDASH2021
IDASH 2021 Strain Classification source codes.  
(C)2021 Eaglys inc.  

Contributors: R. Yamaguchi, K. Mihara, K. Sato, Y. Tsuchiya, Y. Maruyama (Eaglys Inc.)

# Usage
you should be in `path/to/idash2021`

## Preparation
Prease prelare docker container.  

First of all, make docker image
```
docker build . -t capsule_gene
```

and then please run docker container.

```
docker run -it -v /path/to/idash2021:/idash2021 capsule_gene bash
```

## Compile code
you should be in the container and pwd is `/idash2021`

1. cmake
```bash
mkdir build && cd build && cmake ../src/
```

2. compile
```bash
make -j
```

## Computation
1. run preprocessing
```bash
./bin/preprocessing /path/to/Challenge.fa
```

2. run computations
```bash
./bin/pred_rna /path/to/output_folder
```

3. finally you can get probabilities from `/path/to/output_folder/result.csv` and computation times from `/path/to/output_folder/metrics.csv`


# Algorithms
## Overview
PCA and logistic regression (LR) is used to predict labels of RNAs. 
In the LR, we employed CKKS scheme with Microsoft SEAL.  
Specifficaly, the weights and features are encoded as coefficients of cannonical embedding.  
Then, you don't need to rotate the ciphertext to sum.  
With this system you can get acc. of 0.99 with subseconds of prediction and subminute of preprocessing.

coefficient based encoding is implemented in kenmaro3's repository, a fork of Microsoft/SEAL:  
https://github.com/kenmaro3/SEAL/tree/ckks_coeff_365

The specific commit is:  
https://github.com/kenmaro3/SEAL/commit/7ff9d3505cf7888ec846586d974562bfb53f513c  


## Preprocessing
First of all, input RNAs are embedded into the 4dim existance ratio vector as following map function.  

```py
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
```

where `O` represents 0 padding to fix the length of RNAs.   

Then single RNA is embedded into a vector like:
```py
                    #-- A ---#, #-- T ---#, # --
embedding_func("ATR") = [1, 0, 0, 0, 0, 0, 0, 1, 0.5, 0, 0.5, 0]
```

Then you can get embedded vector of RNA.  
However it is too long to predict the label from it.  
To fix this problem, PCA is employed to reduce the dimention.  
PCA is commom method for ML even in the RNAs emmbedding.  

We employed 200 as number of components for PCA considering calculation cost and accuracy of the model.

Finally you can get feature vector of RNA with size of 200.

In this task, 8000 RNAs are passed to this system.  
Therefore, the RNAs are converted to the matrix with size of (8000, 200).

## Encryption methods
In the encryption phase, the input vectors are embedded into a ciphertext until it exceeds the slot size.  
In this system, 2048 is employed as the slot size because polynominal modulus degree is 4096.  
Therefore, you can process 10 RNAs within single ciphertext in this system.  
We need 800 ciphertext to process 8000 RNAs.

For encrypting these vectors, we embedded weights and features as coefficent of plaintext in the cannonical embedding phase.  
After that these plaintexts are encrypted as CKKS ciphertext.  

## Logistic regression  
During the logistic regression, the weights plaintext $W={w_0, ..., w_N}$ and feature ciphertext $X={x_0, ..., x_N}$ is multiplied.  
As mentioned in the previous section, the values are embedded as coefficients.  Therefore, by multipling these features and weights, you can get dot product result.  
For example, $W = {2, 4, 1}$, $X = {1, 2, 2}$, then these vectors are embedded into ploynoinal space as:
```
X = x^2 + 2x + 2
W = x^2 + 4x + 2
```
Note that the weights are embedded in reversed order.    

Then multiply these polynominals, you can get
```
x^4 + 6x^3 + 12x^2 + 12x + 4
```
where the coefficient of x^2 is the result of dot product.  

After calculating dot(W, X), relinialization is applied and bias term is added.  

## Postprocessing
By decrypting this ciphertext at client side and applying argmax, you can get result of logistic regression.  

## Advantages of this method
If you comment out main_coef and use main_batch in main.cpp, you can use basic SEAL encryption version's logistic regression.  
But by using coefficient encoding method, it is 10 times faster than basic SEAL logistic regression.  
```cpp
int main{
    ...
    ...
    // if you want to run with basic lr method please use this function
    main_batch(parameter_folder, input_path, output_path);
    // if you want to use coefficient encoding lr pelase use this function
    // main_coef(parameter_folder, input_path, output_path);
}
    
```

# Security settings
MicroSoft SEAL with CKKS scheme is used for encryption scheme with 128bit security.  
Parameters are as follows:
```cpp
static const double SCALE = pow(2.0, 30);
static const int POLY_MODULUS_DEGREE = 4096;
static const std::vector<int32_t> MODULUS = {38, 30, 38};
```  


# Training of the models  
We prepared PCA and LR parameters in `idash2021/data/` but you can get parameters by yourself.  

Please run the python codes in `idash2021/train` as follows

```bash
# Prepare numeric dataset
$ python3 train/data_preprocess.py

# after that train the PCA and LR
$ python3 train/train_lr.py
```


# Licences
## Microsoft SEAL
    MIT License

    Copyright (c) Microsoft Corporation. All rights reserved.

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE