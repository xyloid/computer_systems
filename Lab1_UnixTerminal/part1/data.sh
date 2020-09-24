#!/bin/bash

# Use this script to scrape data from the web using the provided shell scripts provided.
wget -O - http://www.mshah.io/comp/Fall18/Systems/Lab/2/baseballdata.csv | cut -d ',' -f 5 | tail -n +2 > data.txt
