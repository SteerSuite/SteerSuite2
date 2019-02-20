'''
DATE: 	     MARCH 11, 2017
------------------------------
AUTHOR:      Muhammad Usman (usman@cse.yorku.ca)
CONTRIBUTOR: Mubbasir Kapadia
------------------------------
This script helps in generating .REC files for the given XML testcases

Input: .XML testcase file
Ouput: .REC simulation recording file

'''

import os;	

# path to testcases
## SSCALE = Small Scale | LSCALE = Large Scale
PATH_TO_SSCALE_TESTCASES = 'D:\\Study\\YorkU\\Crowds\\Misc\\SteerBench-DataSet\\testcases\\small_scale\\'
PATH_TO_LSCALE_TESTCASES = 'D:\\Study\\YorkU\\Crowds\\Misc\\SteerBench-DataSet\\testcases\\large_scale\\'

# path to output files (.rec files)
PATH_TO_OUTPUT='D:\\Study\\YorkU\\Crowds\\Misc\\SteerBench-DataSet\\testcases\\output_rec\\'

# path to SteerSuite\build\bin\
PATH_TO_BIN='C:\\Crowds\\SteerSuite\\build\\bin\\'

# name to simulation stats file

SIM_STATS = 'simData.csv'

# testcase file extension

TESTCASE_EXTENSION = '.xml'

# record file extension

RECORDING_EXTENSION = '.rec'

# flags to tell for which testcases to generate .rec files

GENERATE_SSCALE_RECORDINGS = False
GENERATE_LSCALE_RECORDINGS = True

# set initial random seed
INITIAL_SEED = 2891

# set number of scenarios
SCENARIOS_START = 1
SCENARIOS_END = 100
SCENARIOS_END += 1

# max number of simulation frames
MAX_FRAMES = 20000

# set benchmark technique to be used
BENCHMARK_TECH = 'compositePLE'

# iterate over each testcase directory
for testcase_type in ['SSCALE', 'LSCALE' ]: #['SSCALE', 'LSCALE' ]
	
	testcase_dir = ''
	
	if testcase_type == 'SSCALE':
		testcase_dir = PATH_TO_SSCALE_TESTCASES
		if GENERATE_SSCALE_RECORDINGS is not True:
			continue
	
	if testcase_type == 'LSCALE':
		testcase_dir = PATH_TO_LSCALE_TESTCASES
		if GENERATE_LSCALE_RECORDINGS is not True:
			continue
	
	if testcase_dir == '' or testcase_dir is None:
		continue;

	# iterate over each steering algorithm: social forces (sf), orca (rvo2d) and ppr
	for ai in ['sfAI']: # ['sfAI', 'rvo2dAI', 'pprAI']

		# create ai directory
		ai_out_dir = PATH_TO_OUTPUT + testcase_type + "\\" + ai + "\\"	
		
		if not os.path.exists(ai_out_dir):
			os.makedirs(ai_out_dir)
			
		# iterate over each testcase file in testcase_dir	
		for testcase in os.listdir(testcase_dir):
		
			if testcase.endswith(".xml"): 
				print("Current Testcase: " + str(testcase))
				print("------------------------------------\n")
				
				# create testcase directory
				testcase_out_dir = ai_out_dir + testcase + "\\"			
				
				if not os.path.exists(testcase_out_dir):
					os.makedirs(testcase_out_dir)
				
				random_seed = INITIAL_SEED
				
				# run the simulation for given number of scenarios if Large Scale
				if testcase_type == 'LSCALE':
				
					for scenario in range(SCENARIOS_START, SCENARIOS_END):
						
						#print("nice -18 ./steersim -ai "+ai+" -testcase "+(testcase_dir+testcase)+" -randomSeed "+str(random_seed)+" -numFrames "+str(MAX_FRAMES)+" -storesimulation "+(testcase_out_dir+scenario+"-"+testcase+"-"+RECORDING_EXTENSION)+" -dataFileName "+ (PATH_TO_OUTPUT + testcase_type + "\\" + SCENARIOS_START + "-" + SCENARIOS_END + "-" + ai + "-" + SIM_STATS) + " -commandline")
						run = "nice -18 ./steersim -ai "+ai+" -testcase "+(testcase_dir+testcase)+" -randomSeed "+str(random_seed)+" -numFrames "+str(MAX_FRAMES)+" -storesimulation "+(testcase_out_dir+str(scenario)+"-"+testcase+"-"+RECORDING_EXTENSION)+" -dataFileName "+ (PATH_TO_OUTPUT + testcase_type + "\\" + str(SCENARIOS_START) + "-" + str(SCENARIOS_END) + "-" + ai + "-" + SIM_STATS) + " -commandline"
						os.system(run)
						
						random_seed += 100
				else:
					#print("nice -18 ./steersim -ai "+ai+" -testcase "+(testcase_dir+testcase)+" -randomSeed "+str(random_seed)+" -numFrames "+str(MAX_FRAMES)+" -storesimulation "+(testcase_out_dir+testcase+"-"+RECORDING_EXTENSION)+" -dataFileName "+ (PATH_TO_OUTPUT + testcase_type + "\\" + ai + "-" + SIM_STATS) + " -commandline")
					run = "nice -18 ./steersim -ai "+ai+" -testcase "+(testcase_dir+testcase)+" -randomSeed "+str(random_seed)+" -numFrames "+str(MAX_FRAMES)+" -storesimulation "+(testcase_out_dir+testcase+"-"+RECORDING_EXTENSION)+" -dataFileName "+ (PATH_TO_OUTPUT + testcase_type + "\\" + ai + "-" + SIM_STATS) + " -commandline"
					
					os.system(run)