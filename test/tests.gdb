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
#fasten seatbelt on
test "PINA: 0x60 => PORTC: 0xC0"
setPINA 0x60
continue 5
expectPORTC 0xC0
checkResult

test "PINA: 0x23 => PORTC: 0x70"
setPINA 0x23
continue 5
expectPORTC 0x70
checkResult

#fasten seatbelt on
test "PINA: 0x66 => PORTC: 0B8"
setPINA 0x66
continue 5
expectPORTC 0xB8
checkResult

test "PINA: 0x4A => PORTC: 0x3E"
setPINA 0x4A
continue 5
expectPORTC 0x3E
checkResult

#fasten seatbelt on
test "PINA: 0x6F => PORTC: 0xBF"
setPINA 0x6F
continue 5
expectPORTC 0xBF
checkResult

# Report on how many tests passed/tests ran
set $passed=$tests-$failed
eval "shell echo Passed %d/%d tests.\n",$passed,$tests
echo ======================================================\n
