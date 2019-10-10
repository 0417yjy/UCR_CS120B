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

#unlock the door
test "PINA: 0x04, 0x00, 0x02, 0x00 => PORTB: 0x01, PORTC: 0x00, state: wait"
set state = wait
setPINA 0x04
continue 5
setPINA 0x00
continue 5
setPINA 0x02
continue 5
setPINA 0x00
continue 5
expectPORTB 0x01
expectPORTC 0x00
expect state wait
checkResult

#unlock the door and lock it (hold)
test "PINA: 0x04, 0x00, 0x02, 0x00, 0x80 => PORTB: 0x00, PORTC: 0x01, state: lock"
set state = wait
setPINA 0x04
continue 5
setPINA 0x00
continue 5
setPINA 0x02
continue 5
setPINA 0x00
continue 5
setPINA 0x80
continue 5
expectPORTB 0x00
expectPORTC 0x01
expect state lock
checkResult

#unlock failure (press # X)
test "PINA: 0x04, 0x00, 0x01, 0x00 => PORTB: 0x00, PORTC: 0x00, state: wait"
set state = wait
setPINA 0x04
continue 5
setPINA 0x00
continue 5
setPINA 0x01
continue 5
setPINA 0x00
continue 5
expectPORTB 0x00
expectPORTC 0x00
expect state wait
checkResult

#wait for the 'Y' button to unlock
test "PINA: 0x04, 0x00 => PORTB: 0x00, PORTC: 0x03, state: unlockSeq2"
set state = wait
setPINA 0x04
continue 5
setPINA 0x00
continue 5
expectPORTB 0x00
expectPORTC 0x03
expect state unlockSeq2
checkResult

#lock by press the code when unlocked
test "PINA: 0x04, 0x00, 0x02, 0x00, 0x04, 0x00, 0x02, 0x00 => PORTB: 0x00, PORTC: 0x00, state: wait"
set state = wait
setPINA 0x04
continue 5
setPINA 0x00
continue 5
setPINA 0x02
continue 5
setPINA 0x00
continue 5
setPINA 0x04
continue 5
setPINA 0x00
continue 5
setPINA 0x02
continue 5
setPINA 0x00
continue 5
expectPORTB 0x00
expectPORTC 0x00
expect state wait
checkResult

# Report on how many tests passed/tests ran
set $passed=$tests-$failed
eval "shell echo Passed %d/%d tests.\n",$passed,$tests
echo ======================================================\n
