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
#weight is 3
test "PIND: 0x00, PINB: 0x01 => PORTB: 0x01"
setPIND 0x00
setPINB 0x01
continue 5
expectPORTB 0x01
checkResult

#weight is 2
test "PIND: 0x01, PINB: 0x00 => PORTB: 0x00"
setPIND 0x01
setPINB 0x00
continue 5
expectPORTB 0x00
checkResult

#weight is 30
test "PIND: 0x0F, PINB: 0x00 => PORTB: 0x04"
setPIND 0x0F
setPINB 0x00
continue 5
expectPORTB 0x04
checkResult

#weight is 51
test "PIND: 0x19, PINB: 0x01 => PORTB: 0x05"
setPIND 0x19
setPINB 0x01
continue 5
expectPORTB 0x05
checkResult

#weight is 500
test "PIND:0xFC, PINB: 0x00 => PORTB: 0x02"
setPIND 0xFC
setPINB 0x00
continue 5
expectPORTB 0x02
checkResult

#weight is 75
test "PIND: 0x25, PINB: 0x01 => PORTB: 0x03"
setPIND 0x25
setPINB 0x01
continue 5
expectPORTB 0x03
checkResult

# Report on how many tests passed/tests ran
set $passed=$tests-$failed
eval "shell echo Passed %d/%d tests.\n",$passed,$tests
echo ======================================================\n
