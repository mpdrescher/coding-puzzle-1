#!/usr/bin/env python

"""Tests.

You need Python 2.7 or 3 in your PATH to execute this file.
"""

from __future__ import print_function
import subprocess
from collections import OrderedDict

# valgrind="valgrind --tool=memcheck --track-origins=yes --leak-check=full"
#
# echo "\
# ()
# ([)]
# {}
# ()()" | $valgrind ./partone
#
# # 500 parentheses, 1000 chars --> approx. 500 heap allocations
# echo "\

class bcolors:
    FAIL = '\033[91m'
    NORMAL = '\033[0m'

def runtests(executable, testcases):
    run = 1
    for testcase in testcases:
        print('Running {} with testcase{}:'.format(executable, run))
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
                    print('\'{}\' --> {} ' + bcolors.FAIL + '[F]' +
                            bcolors.NORMAL + ' expected: {}'.format(k, actual,
                                expected))
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

runtests('./partone', [testcase1, testcase2, testcase3])
