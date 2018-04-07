
Francisco Huelsz Prince
A01019512

Most of the changes are around bank_server.c
I moved a couple things to server_utils.h and server_definitions.h
to unclutter the file a little bit.

I took the freedom of editing bank_client.c so that it polls both 
STDIN and the socket so that it can be closed if and when the server is 
closed with a ^C.

test.txt and test2.txt are made to test concurency.
test does 6885 $500 deposits and test2 does 6885 $500 whidthrawals.
by launching:
./bank_client address port < test.txt

followed by:
./bank_client address port < test2.txt
a couple seconds later

we can see how the server behaves.
After the last widthrawal the account #0 balance should go back to 0.

I also launched several combinations of deposit/withraw instances and it all seems to add up nicely