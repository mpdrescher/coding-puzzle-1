#!/usr/bin/env python

"""Tests.

You need Python 2.7 or 3 in your PATH to execute this file.
"""

from __future__ import print_function
import subprocess
from collections import OrderedDict
import os.path
import sys

class bcolors:
    FAIL = '\033[91m'
    NORMAL = '\033[0m'

def runtests(executable, testcases):
    run = 1
    for testcase in testcases:
        print('Running {} with testcase {}:'.format(executable, run))
        run = run + 1
        proc = subprocess.Popen([executable], stdin=subprocess.PIPE,
            stdout=subprocess.PIPE)

        out, err = proc.communicate(input=bytearray('\n'.join(testcase.keys()),
            encoding='utf-8'))
        results = out.decode('utf-8')[:-1].split('\n')

        # First, check length of output
        actual_len = len(results)
        expected_len = len([ x for x in testcase.values() if x ])
        if actual_len != expected_len:
            print(bcolors.FAIL + '[F]' + bcolors.NORMAL +
                ' got {} lines, expected: {}'.format(actual_len, expected_len))
            continue

        # Then check individual lines
        i = 0
        for k in testcase.keys():
            # Skip lines with no. of test cases and no. of strings
            if testcase[k] is not None:
                actual = results[i]
                expected = testcase[k]
                if actual != expected:
                    print('\'{}\' --> {} '.format(k, actual) + bcolors.FAIL +
                            '[F]' + bcolors.NORMAL + ' expected: {}'.format(expected))
                else:
                    print('\'{}\' --> {}'.format(k, actual))
                i = i + 1

testcase1 = OrderedDict([
        ('1', None),
        ('4', None),
        ('()', 'True'),
        ('([)]', 'False'),
        ('{}', 'False'),
        ('()()', 'True'),
    ])

testcase2 = OrderedDict([
        ('1', None),
        ('4', None),
        ('', 'True'), # The empty string is treated as True
        ('foo', 'False'),
        ('(bar)', 'False'),
        ('())', 'False'), # Odd
        ('()', None), # This last line should not be processed
    ])

testcase3 = OrderedDict([
        ('1', None),
        ('2', None),
        ('({})', 'True'),
        ('({[]})', 'True'),
        #('3', None),
        #('({[()][{}][[]]})', 'True'),
        #('[]', 'False'),
        #('([])', 'False'),
    ])

testcase4 = OrderedDict([
        ('1', None),
        ('8', None),
        ('()', '1:true'),
        ('([)]', '2:false'),
        ('{}', '3:false'),
        ('()()', '4:true'),
        ('({[[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]})', '5:true'),
        ('({[()][{}][[]]})', '6:true'),
        ('[]', '7:false'),
        ('([])', '8:false'),
    ])

if not os.path.exists('partone') or not os.path.exists('parttwo'):
    print('Please invoke make first')
    sys.exit()

runtests('./partone', [testcase1, testcase2, testcase3])
runtests('./parttwo', [testcase4])
