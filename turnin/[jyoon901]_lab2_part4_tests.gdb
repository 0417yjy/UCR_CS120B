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
test "PINA: 30, PINB: 40 PINC: 50 => PORTD: 0x78"
setPINA 30
setPINB 40
setPINC 50
continue 5
expectPORTD 0x78
checkResult

test "PINA: 55, PINB: 40 PINC: 50 => PORTD: 0x91"
setPINA 55
setPINB 40
setPINC 50
continue 5
expectPORTD 0x91
checkResult

test "PINA: 10, PINB: 20 PINC: 100 => PORTD: 0x82"
setPINA 10
setPINB 20
setPINC 100
continue 5
expectPORTD 0x82
checkResult

test "PINA: 10, PINB: 40 PINC: 100 => PORTD: 0x97"
setPINA 10
setPINB 40
setPINC 100
continue 5
expectPORTD 0x97
checkResult

# Report on how many tests passed/tests ran
set $passed=$tests-$failed
eval "shell echo Passed %d/%d tests.\n",$passed,$tests
echo ======================================================\n
