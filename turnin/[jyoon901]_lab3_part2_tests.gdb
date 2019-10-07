# Test file for Lab2_introToAVR


# commands.gdb provides the following functions for ease:
#   test "<message>"
#       Where <message> is the message to print. Must call this at the beginning of every test
#       Example: test "PINA: 0x00 => expect PORTC: 0x01"
#   checkResult
#       Verify if the test passed or failed. Prints "passed." or "failed." accordingly, 
#       Must call this at the end of every test.
#   expectPORTx <val>
#       With x as the port (A,B,C,D)
#       The value the port is epected to have. If not it will print the erroneous actual value
#   setPINx <val>
#       With x as the port or pin (A,B,C,D)
#       The value to set the pin to (can be decimal or hexidecimal
#       Example: setPINA 0x01
#   printPORTx f OR printPINx f 
#       With x as the port or pin (A,B,C,D)
#       With f as a format option which can be: [d] decimal, [x] hexadecmial (default), [t] binary 
#       Example: printPORTC d
#   printDDRx
#       With x as the DDR (A,B,C,D)
#       Example: printDDRB

echo ======================================================\n
echo Running all tests..."\n\n

# Add tests below
test "PINA: 0 => PORTC: 0x40"
setPINA 0
continue 5
expectPORTC 0x40
checkResult

test "PINA: 3 => PORTC: 0x70"
setPINA 3
continue 5
expectPORTC 0x70
checkResult

test "PINA: 6 => PORTC: 0x38"
setPINA 6
continue 5
expectPORTC 0x38
checkResult

test "PINA: 10 => PORTC: 0x3E"
setPINA 10
continue 5
expectPORTC 0x3E
checkResult

test "PINA: 15 => PORTC: 0x3F"
setPINA 15
continue 5
expectPORTC 0x3F
checkResult

# Report on how many tests passed/tests ran
set $passed=$tests-$failed
eval "shell echo Passed %d/%d tests.\n",$passed,$tests
echo ======================================================\n
