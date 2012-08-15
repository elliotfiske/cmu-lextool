#!/usr/bin/python2

import string
import re
import sys

input_file = open(sys.argv[1], 'r')
output_file = open(sys.argv[2], 'r')

total_correct_cap = 0
total_incorrect_cap = 0
total_correct_comma = 0
total_extra_comma = 0
total_missing_comma = 0
total_correct_period = 0
total_extra_period = 0
total_missing_period = 0
line_count = 0
while 1:
    input_text = input_file.readline()
    output_text = output_file.readline()
    if not input_text or not output_text:
        break
    line_count += 1
    i = 0 
    j = 0

    correct_cap = 0
    correct_comma = 0
    correct_period = 0

    incorrect_cap = 0
    subst_comma_period = 0
    bad_add_comma = 0
    missing_comma = 0
    subst_period_comma = 0
    bad_add_period = 0
    missing_period = 0

    input_list = input_text.split()
    output_list = output_text.split()
    output_comma_list = []
    output_period_list = []
    input_comma_list = []
    input_period_list = []

    for word in output_list:
        if word != "<PERIOD>" and word != "<COMMA>" and word != "<period>" and word != "<comma>":
            while input_list[i] == "<COMMA>" or input_list[i] == "<PERIOD>":
                i += 1
            if word == input_list[i]:
                correct_cap += 1
            else:
                incorrect_cap += 1
            if word.lower() != input_list[i].lower():
                print "ERROR: words do not match"
                print "output " + word + " input " + input_list[i] + " " + str(i)
                print line_count
                break
            i += 1
            j += 1
        else:
            if word == "<COMMA>":
                output_comma_list.append(j)
            if word == "<PERIOD>":
                output_period_list.append(j)

    i = 0

    for word in input_list:
        if word != "<PERIOD>" and word != "<COMMA>":
            i += 1
        else:
            if word == "<COMMA>":
                input_comma_list.append(i)
            elif word == "<PERIOD>":
                input_period_list.append(i)

    i = 0

    for comma in output_comma_list:
        try:
            input_comma_list.index(comma)
            correct_comma += 1
        except ValueError:
            try:
                input_period_list.index(comma)
                subst_comma_period += 1
            except ValueError:
                bad_add_comma += 1

    missing_comma = len(input_comma_list) - correct_comma

    for period in output_period_list:
        try:
            input_period_list.index(period)
            correct_period += 1
        except ValueError:
            try:
                input_period_list.index(period)
                subst_period_comma += 1
            except ValueError:
                bad_add_period += 1

    missing_period = len(input_period_list) - correct_period

    total_correct_cap += correct_cap
    total_incorrect_cap += incorrect_cap

    total_correct_comma += correct_comma
    total_extra_comma += bad_add_comma
    total_missing_comma += missing_comma

    total_correct_period += correct_period
    total_extra_period += bad_add_period
    total_missing_period += missing_period

print "CAPITALIZATION: "
print "CORRECT: " + str(total_correct_cap)
print "INCORRECT " + str(total_incorrect_cap)
print ""
print "COMMA:"
print "CORRECT: " + str(total_correct_comma)
print "EXTRA COMMA: " + str(total_extra_comma)
print "MISSING FROM OUTPUT: " + str(total_missing_comma)
print ""
print "PERIOD:"
print "CORRECT: " + str(total_correct_period)
print "EXTRA PERIOD: " + str(total_extra_period)
print "MISSING FROM OUTPUT: " + str(total_missing_period)
