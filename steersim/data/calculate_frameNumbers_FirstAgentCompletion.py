'''
DATE: 	     MARCH 14, 2017
------------------------------
AUTHOR:      Muhammad Usman (usman@cse.yorku.ca)
CONTRIBUTOR: Mubbasir Kapadia
------------------------------
This script helps in collecting simulation stats in a CSV file.

Input: .REC file
Ouput: .CSV with simulation stats

Supported Stats: testcases/recfile name and frame number from the first agent completion
'''

import os;

# path to recfiles directory. For example:
PATH_TO_REC_FILES = 'D:\\Crowds\\SteerSuite\\output_rec\\'

# path to output directory for storing stats.  For example:
PATH_TO_OUTPUT_DIR = 'D:\\Crowds\\SteerSuite\\output_rec\\'

# path to SteerSuite\build\bin\
PATH_TO_BIN='C:\\Crowds\\SteerSuite\\build\\bin\\'

# name to stats file
SIM_STATS = 'simData_Frames.csv'

# extension of rec file
REC_FILE_EXTENSION = '.rec'
			
# iterate over each .rec file in .rec files directory	
for recfile in os.listdir(PATH_TO_REC_FILES):

	if recfile.endswith(REC_FILE_EXTENSION): 
		print("Current Rec File: " + str(recfile))
		print("------------------------------------\n")
		
		print(PATH_TO_BIN + "steersim.exe -module recFilePlayer,recfile="+(PATH_TO_REC_FILES+recfile)+",setDataFile="+(PATH_TO_OUTPUT_DIR+SIM_STATS)+",saveTestcaseName,saveFirstAgentCompletionFrame")
		run = PATH_TO_BIN + "steersim.exe -module recFilePlayer,recfile="+(PATH_TO_REC_FILES+recfile)+",setDataFile="+(PATH_TO_OUTPUT_DIR+SIM_STATS)+",saveTestcaseName,saveFirstAgentCompletionFrame -commandLine"
		os.system(run)