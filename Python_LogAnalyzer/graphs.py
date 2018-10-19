from __future__ import (absolute_import, division,
                        print_function, unicode_literals)


import csv
import sys

import glob
import copy

#import pandas as pd
from datetime import datetime
from decimal import *

import statistics as stat


#import seaborn as sns

import csv


##########################################
#Graphs variables declaration
##########################################
#color =     ['#ffffff', '#cfcecf', '#918c8c', '#6f6d6d']
#edgecolor = ['#cfcecf', '#918c8c', '#6f6d6d', '#504d4d']

#color =     ['#ffffff', '#81BEF7', '#D8D8D8', '#424242', '#FFFFFF', '#6f6d6d']
#edgecolor = ['#cfcecf', '#045FB4', '#120A2A', '#120A2A', '#120A2A', '#504d4d']
#hatch =     [ "/////", " ", " ", "oo", ".....", " "]
marker = ['D', 'v','H', 'v', '*', '>', '<', 'o', 'x']
color = []
edgecolor = []
hatch =     []



Y_min =     [0,0,0,0,0,0,0,0,0]
Y_max =     [9500,9500,100,100,9500,9500,100,100,9500]

Y_min =     [0,0,0,0,0,0,0,0,0]
Y_max_stacked =     [3500*6,3500*6,100*6,100*6,3500*6,3500*6,100*6,100*6,3500*6]

##########################################
#Variables declaration
##########################################

file_names = []
dirs_names = []
legends= []

graph_last_index_values = []
yaxis_array = []
xaxis_array = []

legends_names_array= []
parameters_names_array = []
null_array = []

rb_hits_data = []
rb_miss_data = []
rb_hitrate_data = []
rb_missrate_data = []

drc_hits_data = []
drc_miss_data = []
drc_hitrate_data = []
drc_missrate_data = []

tagCacheHits_data = []
tagCacheMisses_data = []
add_1_data = []

rb_hits_last_index_values = []
rb_miss_last_index_values = []
rb_hitrate_last_index_values = []
rb_missrate_last_index_values = []
drc_hits_last_index_values = []
drc_miss_last_index_values = []
drc_hitrate_last_index_values = []
drc_missrate_last_index_values = []

tagCacheHits_last_index_values = []
tagCacheMisses_last_index_values = []

add_1_last_index_values = []
add_2_last_index_values = []
add_3_last_index_values = []

parameters_checks = []
parameters_range= []
parameters_file_norm= []
x_axis= []

parameters_default = []
parameters_max = []
parameters_min = []
parameters_avg_hm = []
parameters_ref_legend_file = []
parameters_scaled_array = []
parameters_graph = []
parameters_y_axis_mark= []
parameters_x_axis_rot= []

array_temp1 = []
array_temp2 = []
array_temp3 = []
array_temp4 = []
array_temp5 = []
array_temp6 = []
array_temp7 = []
array_temp8 = []
array_temp9 = []
array_temp10 = []
array_temp11 = []



#------------------------------------------------------------------------------------------------------------------#
##########################################
# Data Array Operation
##########################################
def array_manupulation(rb_hits_data,rb_miss_data,rb_hitrate_data,rb_missrate_data,drc_hits_data,drc_miss_data,drc_hitrate_data,drc_missrate_data,tagCacheHits_data,tagCacheMisses_data,add_1_data, add_2_data, add_3_data):

	
	#print (rb_hits_data)
	a = len(rb_hits_data)
	b = len(rb_miss_data)
	c = len(rb_hitrate_data)
	d = len(rb_missrate_data)
	e = len(drc_hits_data)
	f = len(drc_miss_data)
	g = len(drc_hitrate_data)
	h = len(drc_missrate_data)
	i = len(tagCacheHits_data)
	j = len(tagCacheMisses_data)
	k = len(add_1_data)
	l = len(add_2_data)
	m = len(add_3_data)
	
	
	"""
	for r in range(c): 
		rb_missrate_data[r] = 100 - rb_hitrate_data[r]
	for p in range(g-1): 
		drc_missrate_data[p] = 100 - drc_hitrate_data[p]
	print rb_hitrate_data
	print rb_missrate_data
	#print drc_missrate_data
	"""
	list = [a, b, c, d, e, f, g, h, i, j, k, l, m]
	#print list
	
	if a > 0:
		data = rb_hits_data[a-1]
		rb_hits_last_index_values.append(data)
	else:
		rb_hits_last_index_values.append('0')
	if b > 0:
		data = rb_miss_data[b-1]
		rb_miss_last_index_values.append(data)
	else:
		rb_miss_last_index_values.append('0')
	if c > 0:
		data = rb_hitrate_data[c-1]
		rb_hitrate_last_index_values.append(data)
	else:
		rb_hitrate_last_index_values.append('0')
	if d > 0:
		data = rb_missrate_data[d-1]
		rb_missrate_last_index_values.append(data)
	else:
		rb_missrate_last_index_values.append('0')
	if e > 0:
		data = drc_hits_data[e-1]
		drc_hits_last_index_values.append(data)
	else:
		drc_hits_last_index_values.append('0')
	if f > 0:
		data = drc_miss_data[f-1]
		drc_miss_last_index_values.append(data)
	else:
		drc_miss_last_index_values.append('0')
	if g > 0:
		data = drc_hitrate_data[g-1]
		drc_hitrate_last_index_values.append(data)
	else:
		drc_hitrate_last_index_values.append('0')
	if h > 0:
		data = drc_missrate_data[h-1]
		drc_missrate_last_index_values.append(data)
	else:
		drc_missrate_last_index_values.append('0')
	if i > 0:
		data = tagCacheHits_data[i-1]
		tagCacheHits_last_index_values.append(data)
	else:
		tagCacheHits_last_index_values.append('0')
	if j > 0:
		data = tagCacheMisses_data[j-1]
		tagCacheMisses_last_index_values.append(data)
	else:
		tagCacheMisses_last_index_values.append('0')
	if k > 0:
		data = add_1_data[j-1]
		add_1_last_index_values.append(data)
	else:
		add_1_last_index_values.append('0')
	if l > 0:
		data = add_2_data[k-1]
		add_2_last_index_values.append(data)
	else:
		add_2_last_index_values.append('0')
	if m > 0:
		data = add_3_data[l-1]
		add_3_last_index_values.append(data)
	else:
		add_3_last_index_values.append('0')


	#print tagCacheMisses_data
	#print tagCacheMisses_last_index_values

	temp = max(list)

	x = '-'
	for n in range(temp): 
		null_array.append(x)
		#null_array.append([])

	rb_hits_data.extend(null_array)
	rb_miss_data.extend(null_array)
	rb_hitrate_data.extend(null_array)
	rb_missrate_data.extend(null_array)
	drc_hits_data.extend(null_array)
	drc_miss_data.extend(null_array)
	drc_hitrate_data.extend(null_array)
	drc_missrate_data.extend(null_array)
	tagCacheHits_data.extend(null_array)
	tagCacheMisses_data.extend(null_array)
	add_1_data.extend(null_array)
	add_2_data.extend(null_array)
	add_3_data.extend(null_array)
	return temp

##########################################


#######################################
#Reading parameters file 
#------------------------------------------------------------------------------------------------------------------#
#
#
#
#------------------------------------------------------------------------------------------------------------------#
def read_parameters_file():

	parameters_file = open("parameters.txt", "r")


	for line in parameters_file:
		if not line.startswith("-") and not line.startswith("'") and not line.startswith("!") and not line.startswith("="):
			str = line

			data = str.split( )[0]
			parameters_names_array.append(data)

			data = str.split( )[1]
			parameters_graph.append(int(data))

			data = str.split( )[2]
			parameters_scaled_array.append(data)

			data = str.split( )[3]
			parameters_ref_legend_file.append(int(data))

			data = str.split( )[4]
			parameters_avg_hm.append(data)

			data = str.split( )[5]
			parameters_min.append(int(data))

			data = str.split( )[6]
			parameters_max.append(int(data))

			data = str.split( )[7]
			parameters_default.append(data)

			data = str.split( )[8]
			parameters_y_axis_mark.append(data)

			data = str.split( )[9]
			parameters_x_axis_rot.append(int(data))

		elif line.startswith("-"):
			str = line
			#print str.split( )[1]

			data = str.split( )[1]
			parameters_checks.append(data)

		elif line.startswith("'"):
			str = line
			#print str.split( )[1]

			data = str.split( )[1]
			parameters_range.append(data)

		elif line.startswith("!"):
			str = line
			#print str.split( )[1]

			data = str.split( )[1]
			parameters_file_norm.append(data)


#print parameters_checks
##########################################
#------------------------------------------------------------------------------------------------------------------#
#
#
#
#------------------------------------------------------------------------------------------------------------------#
def read_legends_file():

	legends_file = open("legends.txt", "r")

	#Reading legends file 

	for line in legends_file:
		str = line
		data = str.split('\n')[0]
		legends_names_array.append(data)
 
########################################
#------------------------------------------------------------------------------------------------------------------#
#
#
#
#------------------------------------------------------------------------------------------------------------------#
def read_format_file():

	format_file = open("format.txt", "r")

	#Reading format file 

	for line in format_file:
		if not line.startswith("'"):
			str = line

			data = str.split('\n')[0]
			data = data.split(',')[0]
			color.append(data)

			data = str.split(',')[1]
			if data != '\n':
				edgecolor.append(data)

			data = str.split(',')[2]
			if data != '\n':
				hatch.append(data)

	for k in range(len(hatch)):
		hatch[k] = hatch[k].rstrip('\n')
########################################
#------------------------------------------------------------------------------------------------------------------#
#
#
#
#------------------------------------------------------------------------------------------------------------------#
#Reading axis file 
def read_axis_file():

	axis_file = open("axis.txt", "r")

	for line in axis_file:
		if not line.startswith("'"):
			str = line
			#print str.split( )[1]

			data = str.split('\n')[0]
			data = data.split(',')[0]
			yaxis_array.append(data)

			data = str.split(',')[1]
			if data != '\n':
				x_axis.append(data)

	for k in range(len(x_axis)):
		x_axis[k] = x_axis[k].rstrip('\n')


#######################################
def data_extraction_logfiles():
	#print (parameters_names_array)
	#Extracting different parameters values from log file
	path = "Script/"
	global dirs_names
	dirs_names = sorted(glob.glob(path + '*')) #key=numericalSort
	#######################################
	#------------------------------------------------------------------------------------------------------------------#
	#
	#
	#
	#------------------------------------------------------------------------------------------------------------------#

	#---------------------------------------#
	for k in range(len(dirs_names)):
	#---------------------------------------#
		global file_names
		file_names = sorted(glob.glob(dirs_names[k] + "/*.log"))
		#print(file_names)
		#---------------------------------------#
		for j in range(len(file_names)):
		#---------------------------------------#
			rb_hits_data = []
			rb_miss_data = []
			rb_hitrate_data = []
			rb_missrate_data = []

			drc_hits_data = []
			drc_miss_data = []
			drc_hitrate_data = []
			drc_missrate_data = []

			tagCacheHits_data = []
			tagCacheMisses_data = []
			add_1_data = []
			add_2_data = []
			add_3_data = []
		#---------------------------------------#
			searchfile = open(file_names[j], "r")
			for line in searchfile:
			    if len(parameters_names_array) >= 1 and parameters_names_array[0] in line: 
			       str = line

			       data = str.split( )[1]
			       rb_hits_data.append(data)
		#---------------------------------------#
			    if len(parameters_names_array) >= 2 and parameters_names_array[1] in line: 
			       str = line

			       data = str.split( )[1]
			       rb_miss_data.append(data)
		#---------------------------------------#
			    if len(parameters_names_array) >= 3 and parameters_names_array[2] in line: 
			       str = line

			       data = str.split( )[1]
			       #data = round(Decimal(data)*100)

			       rb_hitrate_data.append(data)
		#---------------------------------------#
			    if len(parameters_names_array) >= 4 and parameters_names_array[3] in line: 
			       str = line

			       data = str.split( )[1]
			       rb_missrate_data.append(data)
		#---------------------------------------#
			    if len(parameters_names_array) >= 5 and parameters_names_array[4] in line: 
			       str = line

			       data = str.split( )[1]
			       drc_hits_data.append(data)
		#---------------------------------------#
			    if len(parameters_names_array) >= 6 and parameters_names_array[5] in line: 
			       str = line

			       data = str.split( )[1]
			       #data = round(Decimal(data)*100)
			       drc_miss_data.append(data)
		#---------------------------------------#
			    if len(parameters_names_array) >= 7 and parameters_names_array[6] in line: 
			       str = line

			       data = str.split( )[1]
			       #data = round(Decimal(data)*100)
			       #print data
			       drc_hitrate_data.append(data)
		#---------------------------------------#
			    if len(parameters_names_array) >= 8 and parameters_names_array[7] in line: 
			       str = line

			       data = str.split( )[1]
			       drc_missrate_data.append(data)
		#---------------------------------------#
			    if len(parameters_names_array) >= 9 and parameters_names_array[8] in line: 
			       str = line

			       data = str.split( )[1]
			       tagCacheHits_data.append(data)
		#---------------------------------------#
			    if len(parameters_names_array) >= 10 and parameters_names_array[9] in line: 
			       str = line
			    
			       data = str.split( )[1]
			       #data = round(Decimal(data)*100)
			       tagCacheMisses_data.append(data)
		#---------------------------------------#
			    if len(parameters_names_array) >= 11 and parameters_names_array[10] in line: 
			       str = line
			    
			       data = str.split( )[1]
			       add_1_data.append(data)
		#---------------------------------------#
		#---------------------------------------#
			    if len(parameters_names_array) >= 11 and parameters_names_array[11] in line: 
			       str = line
			    
			       data = str.split( )[1]
			       add_2_data.append(data)
		#---------------------------------------#
		#---------------------------------------#
			    if len(parameters_names_array) >= 11 and parameters_names_array[12] in line: 
			       str = line
			       data = str.split( )[1]
			       #data = round(Decimal(data)*100)
			       add_3_data.append(data)
		#---------------------------------------#
			temp = array_manupulation(rb_hits_data, rb_miss_data, rb_hitrate_data, rb_missrate_data, drc_hits_data, drc_miss_data, drc_hitrate_data, drc_missrate_data, tagCacheHits_data, tagCacheMisses_data, add_1_data, add_2_data, add_3_data)
			#print (rb_hits_last_index_values)

#------------------------------------------------------------------------------------------------------------------#
#------------------------------------------------------------------------------------------------------------------#
#	This module reads the data from CSV file to plot difeerent graphs.
#	reading_csv()
#	Tanveer Ahmad
#------------------------------------------------------------------------------------------------------------------#
def reading_csv():
	temp_buffer = []
	temp_buffer2D = []
	splitted_list2D= []
	rb_hits_last_index_values = []
	rb_miss_last_index_values = []
	rb_hitrate_last_index_values = []
	rb_missrate_last_index_values = []
	drc_hits_last_index_values = []
	drc_miss_last_index_values = []
	drc_hitrate_last_index_values = []
	drc_missrate_last_index_values = []

	tagCacheHits_last_index_values = []
	tagCacheMisses_last_index_values = []

	add_1_last_index_values = []
	add_2_last_index_values = []
	add_3_last_index_values = []

	dir_no = 0
	row_check = True
	with open("Graphs/overall_csv.csv",'r') as csvFile:
		reader=csv.reader(csvFile,delimiter=',')
		next(reader)
		for row in reader:
		    if row_check == True: 
		        temp_row = row
		        row_buffer_offset = int (((len(row)/2) - 2))
		        global file_names
		        file_names = [elem for elem in range(row_buffer_offset)]
		        row_check = False
		    if len(row) > 1:
		        temp_buffer.extend(row[int (((len(row)/2) + 2)):])
		        dir_no += 1
		    else:
		        temp_buffer = [float(i) for i in temp_buffer[row_buffer_offset:]]
		        #print(temp_buffer)
		        temp_buffer2D.append(temp_buffer)
		        global dirs_names
		        dirs_names = [elem for elem in range(dir_no-2)]
		        dir_no = 0
		        temp_buffer= []

		temp_buffer = [float(i) for i in temp_buffer[row_buffer_offset:]]
		temp_buffer2D.append(temp_buffer)

		for j in range(sum(1 for x in parameters_graph if x == 1)):    
		    
		    splitted_list = split_list_simple(temp_buffer2D[j], len(dirs_names)+1)
		    splitted_list2D.append(splitted_list)
	return splitted_list2D

#------------------------------------------------------------------------------------------------------------------#
#
#
#
#------------------------------------------------------------------------------------------------------------------#
def percentage(x, pos):
    'The two args are the value and tick position'
    return '%1.1f' % (x) + '%'
#------------------------------------------------------------------------------------------------------------------#
#
#
#
#------------------------------------------------------------------------------------------------------------------#
def simple(x, pos):
    'The two args are the value and tick position'
    return '%1.1f' % (x)

#------------------------------------------------------------------------------------------------------------------#
#
#
#
#------------------------------------------------------------------------------------------------------------------#
#------------------------------------------------------------------------------------------------------------------#
def split_list(alist, wanted_parts, scaled, ref_file_index, csv_writing, writer, parameter):

    extended = []
    extended2d = []
    transposed = []
    length = len(alist)

    splitted_list = [ alist[i*length // wanted_parts: (i+1)*length // wanted_parts] for i in range(wanted_parts) ]

   
    #print (extended2d)
    if csv_writing == True:
	    for file in range(len(dirs_names)):
		    rwotowrite = [' '] + [parameters_names_array[parameter]] + [elem for elem in splitted_list[file]]
		    writer.writerow(rwotowrite)

    for i in range(len(file_names)):
	    transposed.append([row[i] for row in splitted_list])
    #print (transposed)
    return transposed

		
#------------------------------------------------------------------------------------------------------------------#
#
#
#
#------------------------------------------------------------------------------------------------------------------#
def split_list_simple(alist, wanted_parts):

    transposed = []
    length = len(alist)

    splitted_list = [ alist[i*length // wanted_parts: (i+1)*length // wanted_parts] for i in range(wanted_parts) ]

    for i in range(len(file_names)):
        transposed.append([row[i] for row in splitted_list])
    
    return transposed
	
#------------------------------------------------------------------------------------------------------------------#
#
#
#
#------------------------------------------------------------------------------------------------------------------#
def split(arr, size):
     arrs = []
     while len(arr) > size:
         pice = arr[:size]
         arrs.append(pice)
         arr   = arr[size:]
     arrs.append(arr)
     return arrs
#------------------------------------------------------------------------------------------------------------------#
#
#
#
#------------------------------------------------------------------------------------------------------------------#
my_objects = []
class MyClass(object):
    def __init__(self, obj):
        self.x = obj

#------------------------------------------------------------------------------------------------------------------#
def graphs_generation():
	if sys.argv[2] == 'direct':
		all_data_values_2D = reading_csv()
#------------------------------------------------------------------------------------------------------------------#
	if sys.argv[1] == 'bar':
	#------------------------------------------------------------------------------------------------------------------#
		#with matplotlib.backends.backend_pdf.PdfPages('Graphs/graphs_dram_caches_bars.pdf') as pdf:
		#------------------------------------------------------------------------------------------------------------------#	
			if not sys.argv[2] == 'direct':
				csv_writing = True
				f = open("Graphs/overall_csv.csv", 'wt')
				writer = csv.writer(f)
				legends = [nmes.split('/')[2] for nmes in file_names]
				#legends = [nmes.split('/')[0] for nmes in file_names]
				#print(legends)
		#------------------------------------------------------------------------------------------------------------------#
			for x in range(len(parameters_graph)):
				if x == 0:
					graph_last_index_values = copy.deepcopy(rb_hits_last_index_values)
					header = [' '] + [' '] + [leg for leg in legends]
					writer.writerow( header )

				elif x == 1:
					graph_last_index_values = copy.deepcopy(rb_miss_last_index_values)
				elif x == 2:
					graph_last_index_values = copy.deepcopy(rb_hitrate_last_index_values)
				elif x == 3:
					graph_last_index_values = copy.deepcopy(rb_missrate_last_index_values)
				elif x == 4:
					graph_last_index_values = copy.deepcopy(drc_hits_last_index_values)
				elif x == 5:
					graph_last_index_values = copy.deepcopy(drc_miss_last_index_values)
				elif x == 6:
					graph_last_index_values = copy.deepcopy(drc_hitrate_last_index_values)
				elif x == 7:
					graph_last_index_values = copy.deepcopy(drc_missrate_last_index_values)
				elif x == 8:
					graph_last_index_values = copy.deepcopy(tagCacheHits_last_index_values)
				elif x == 9:
					graph_last_index_values = copy.deepcopy(tagCacheMisses_last_index_values)
				elif x == 10:
					graph_last_index_values = copy.deepcopy(add_1_last_index_values)
				elif x == 11:
					graph_last_index_values = copy.deepcopy(add_2_last_index_values)
				elif x == 12:
					graph_last_index_values = copy.deepcopy(add_3_last_index_values)
				#print (rb_hitrate_last_index_values)
				if parameters_graph[x] == 1:
					if not sys.argv[2] == 'direct':
						avg_hm = []
						#header = [' '] + [parameters_names_array[x]] + [leg for leg in legends_names_array]
						#print (header)
						#writer.writerow([' '])
						#writer.writerow( header )

					ymax = parameters_max[x]
					ymin = parameters_min[x]

					if parameters_avg_hm[x] == 'Y':
						N = len(dirs_names) + 1 
						x_axis[N-1] = 'Average' 
					elif parameters_avg_hm[x] == 'N':
						N = len(dirs_names) + 1 
						x_axis[N-1] = 'H.Mean' 
					else:
						N = len(dirs_names) #3
					

		
					
					if not sys.argv[2] == 'direct':
						if parameters_scaled_array[x] == 'Y':
							splitted_list = split_list(graph_last_index_values, len(dirs_names), True, parameters_ref_legend_file[x], csv_writing, writer, x)
						else:
							splitted_list = split_list(graph_last_index_values, len(dirs_names), False, parameters_ref_legend_file[x], csv_writing, writer, x)



	#------------------------------------------------------------------------------------------------------------------#
	#------------------------------------------------------------------------------------------------------------------#



	#------------------------------------------------------------------------------------------------------------------#
	#------------------------------------------------------------------------------------------------------------------#
	
	#------------------------------------------------------------------------------------------------------------------#

	#------------------------------------------------------------------------------------------------------------------#
	#------------------------------------------------------------------------------------------------------------------#
	



#------------------------------------------------------------------------------------------------------------------#/net/home/tahmad/Documents/Python/Script/
#------------------------------------------------------------------------------------------------------------------#

#------------------------------------------------------------------------------------------------------------------#
#
#
#
#------------------------------------------------------------------------------------------------------------------#
def main(argv):
    # My main code here
    pass
    
    #Reading parameters files
    read_parameters_file()
    read_legends_file()
    read_format_file()
    read_axis_file()

    #if in case reading data from log files
    if not sys.argv[2] == 'direct':
	    #Data extraction from log files
	    data_extraction_logfiles()

    #Graphs generation
    graphs_generation()

if __name__ == "__main__":
    main(sys.argv)
#------------------------------------------------------------------------------------------------------------------#









