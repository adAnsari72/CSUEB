'''
Program by: Amir Daud
Machine Learning

Using Scikit Learn to predict stock data and decide whether to buy or not to buy.
'''

import csv
import numpy
import sklearn as sk
import scipy as sp
import pandas as pd 	#used for reading the csv

from sklearn.model_selection import train_test_split
from sklearn.tree import DecisionTreeClassifier
from sklearn.metrics import accuracy_score


#Features used for predicting values
featureSet = ['Date', 'Open', 'High', 'Low', 'Close', 'Adj Close', 'Volume', 
			  'High Low Change', 'Daily Average', 'Price Variance', 'Keep']
			  
#Stock files used for each company
stockFiles = ['AAPL.csv', 'AMD.csv', 'AMZN.csv', 'ATVI.csv', 'CMCSA.csv', 
			  'EA.csv', 'EBAY.csv', 'FB.csv', 'FIT.csv', 'GIS.csv', 'GME.csv', 
			  'GOLD.csv', 'GOOG.csv', 'INTC.csv', 'K.csv', 'MSFT.csv', 'NTDOY.csv',
			  'NVDA.csv', 'SBUX.csv', 'SNE.csv', 'STX.csv', 'TGT.csv', 'TWTR.csv',
			  'WDC.csv', 'WMT.csv']

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

	#defines the tree using a Gini model of classification
	clf_gini = DecisionTreeClassifier(criterion="gini", random_state=100)

	#by multiplying by 100 and casting into an int we do not lose the cent values
	#Some of the numbers were too large for this function as floats, so I convert them to int
	#Y_train = [int(i*100) for i in Y_train]

	clf_gini.fit(X_train, Y_train)

	y_pred = clf_gini.predict(X_test)

	#Reduces the predicted value so that it matches the range of the test values
	#y_pred = [int(i/100) for i in y_pred]
	#print("Accuracy of Prediciton : ", numpy.mean([y_pred[i]/Y_test[i]*100 for i in range(len(Y_test))]))

	print("Prediction: ", y_pred[-1])
	print("Score: ", accuracy_score(Y_test, y_pred))

#Main Function begins here
for i in range(len(stockFiles)):
	print("\nFor ", stockFiles[i], ":")
	df = getData(stockFiles[i])
	dataManip(df)

print("\nEnd of Program")