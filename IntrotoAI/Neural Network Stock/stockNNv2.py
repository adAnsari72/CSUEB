'''
Program by: Amir Daud
Neural Networks

Using Scikit Learn to predict stock data and decide whether to buy or not to buy.
Version 2:
	consolidated the 25 csv files into 1 massive file

Libraries to install for program to run:
	scipy
	scikit learn
	pandas
'''

import csv
import numpy
import sklearn as sk
import scipy as sp
import pandas as pd

from sklearn.model_selection import train_test_split
from sklearn.tree import DecisionTreeClassifier
from sklearn.metrics import accuracy_score
from sklearn.neural_network import MLPClassifier


#Features used for predicting values
featureSet = ['Date', 'Open', 'High', 'Low', 'Close', 'Adj Close', 'Volume', 
			  'High Low Change', 'Daily Average', 'Price Variance', 'Keep']
			  
#Stock files used for each company
stockFiles = ['AAPL.csv', 'AMD.csv', 'AMZN.csv', 'ATVI.csv', 'CMCSA.csv', 
			  'EA.csv', 'EBAY.csv', 'FB.csv', 'FIT.csv', 'GIS.csv', 'GME.csv', 
			  'GOLD.csv', 'GOOG.csv', 'INTC.csv', 'K.csv', 'MSFT.csv', 'NTDOY.csv',
			  'NVDA.csv', 'SBUX.csv', 'SNE.csv', 'STX.csv', 'TGT.csv', 'TWTR.csv',
			  'WDC.csv', 'WMT.csv']

#The consolidated csv data
big_frame = pd.DataFrame()

#this function is purely for returning a Buy or Dont Buy into the pandas data structure
def BuyDontBuy(row):
	if row['Close'] - row['Open'] >= 0:
		return "Buy"
	else :
		return "Dont Buy"

#pull csv data
def getData(filename):
	newData = pd.read_csv(filename, index_col='Date')

	# Adding 4 additional attributes based on the 7 in the csv file
	newData['High Low Change'] = newData['High'] - newData['Low']
	newData['Daily Average'] = (newData['Close']+newData['Open'])/2
	newData['Price Variance'] = (newData['Daily Average'] - newData['High Low Change']) * newData['Volume']
	newData['Keep'] = newData.apply(BuyDontBuy, axis=1)

	#Drops rows with incomplete data
	newData = newData.dropna()
	return newData

#Does the heavy lifting for the code
#Does all the data manipulation
def dataManip(df):
	#splitting the data where:
	# X is the predicting data
	# Y is the Buy or Dont Buy functions output for those predicting data
	X = df.drop('Keep', axis=1)
	Y = df['Keep']

	#Declare and define 4 variables for training and testing
	#splits the data for 80% train 20% test
	X_train, X_test, Y_train, Y_test = train_test_split(X, Y, test_size=0.2, random_state=100)


	'''
	*********************************************************************
	In the following lines, uncomment the NN lines for different settings
	*********************************************************************
	'''
	#This constructs a 2 layer with 4 nodes each
	NN = MLPClassifier(hidden_layer_sizes=(4, 4))

	#Construction with 4 layers with 4 nodes
	#NN = MLPClassifier(hidden_layer_sizes=(4, 4, 4, 4))

	#Construction with 4 layers with 8 nodes
	#NN = MLPClassifier(hidden_layer_sizes=(8, 8, 8, 8))

	#Construction with 10 layers with 20 nodes
	#NN = MLPClassifier(hidden_layer_sizes=(20, 20, 20, 20, 20, 20, 20, 20, 20, 20))

	'''
	*********************************************************************
	*********************************************************************
	'''

	NN.fit(X_train, Y_train)

	y_pred = NN.predict(X_test)

	print("Prediction: ", y_pred[-1])
	print("Score: ", accuracy_score(Y_test, y_pred))

	'''
	*********************************************************************
								WEIGHTS OUTPUT
	*********************************************************************
	''' 

	'''
	This is commented out to not produce more files when test running
	when done, it will generate a file in the weights folder, 
	the file will be named weightsXLYN where :
		X = number of layers
		Y = number of nodes per layer
	you have to manually change the path name below if you want it to match
	I have ran the data 4 times with each set of NN sizes as listed in the code above
	they are located in the folder: 
		weights compare
	'''

	#expDF = pd.DataFrame(data=NN.coefs_)
	#expDF.to_csv(path_or_buf=("weights\weights4L4N.csv"))



#Main Function begins here

#for use in consolidation of csv files
tempDF = []

for i in range(len(stockFiles)):
	df = getData(stockFiles[i])
	tempDF.append(df)

#consolidate all the csv into one frame
big_frame = pd.concat(tempDF)
dataManip(big_frame)


print("\nEnd of Program")