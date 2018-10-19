# Matplotlib-Python-Graphs
A library to generate different types of graphs (bar, stack and line) in PDF format (single and multi pages) and formatting them using Matplotlib in Python.


Description:
--------------------------------------------------------
The purpose behind this work is to read different types of parameters values (rb_hits, rb_miss etc.) from different benchmarks dirs (B1, B2 etc.) for particular DRAM files (DRAM-Base, STT-Base etc.) generated through NVMain Simulator to fill a CSV file and calculate HM/Average values of respective parameters. From that CSV file different types of graphs for those parameters are being generated.


Example:
--------------------------------------------------------
Some example plots are presnet in Graphs directory.


Usage:
--------------------------------------------------------
To generate stack graphs from different log files of different benchmarks (in Script) use this command:<br />
python3 graphs.py stack log

To generate line graphs from different log files of different benchmarks (in Script) use this command:<br />
python3 graphs.py line log

To generate bar graphs from different log files of different benchmarks (in Script) use this command:<br />
python3 graphs.py bar log


To generate stack graphs from a csv file use this command:<br />
python3 graphs.py stack direct

To generate line graphs from a csv file use this command:<br />
python3 graphs.py line direct

To generate bar graphs from a csv file use this command:<br />
python3 graphs.py bar direct



