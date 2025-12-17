**TEST RESULTS**

Multiple client Test : 				        Success : Server accepts all connections and allow for file transfer  
Multiple Server Test : 				        Failure : A socket can only be binded to one server at a time  
File from same directory transfer : 	    Success : File name and content is transmitted as is     
File from a different directory transfer :  Success : Path is removed from file name before transferring   
Running client without running server :     Failure : Client side fails to connect   
Small file transfer : 				        Success : File of 50 bytes was transferred   
Large file transfer :				        Success : File of 19.5 MBs was transferred   
Wireshark test : 				            Success : Data of Packet size 1KB(Excluding ACK headers) was transferred    



**INFERENCES**

A socket is used to facilitate two way communication between programs running on a network.  
A socket address is made up of the system's IP and a specific Port number.  
A port is assigned to a specific application so that whenever data is to be transferred during socket communication, this port is set as the endpoint for that application  
A port can be assigned in the range 0 - 65535   
Among these ports upto 1023 are considered generic ports assigned to specific services like  
FTP - 20  
SSH - 22  
DNS - 53  
HTTP - 80  
HTTPS - 443  
Ports from 1024 to 49151 are assigned for custom applications to run   
Ports from 49152 to 65535 are usually for temporary connections  
There are two major socket communication protocols, TCP and UDP  
Transmission control protocol is slow, reliable and connection oriented  
It establishes three way handshake between client and server through SYN->SYN-ACK->ACK before data transmission  
It features data reordering, flow control, segmentation and retransmission so data arrive without packet loss  
It is used in File transfer, web browsing, etc  
User Datagram protocol is fast, unreliable and connectionless  
There is no error checks, acknowledgments and retransmissions  
Some packets can get lost  
Used in Video streaming, gaming, etc  

In TCP client needs to connect with the server for Data transfer, this is achieved through connect()   
On the server side, The server first binds to a socket address through bind() before listening for connections using listen() where these connections are queued   
The server then accepts a connection through accept() after which data transmission can happen.   


TCP communication was verified using Wireshark and the generated capture was filtered   
Packet size and count were found, packet contents were verified and three way handshake was confirmed   
ACK packet was found to be 68 bytes while PSH-ACK(data packet) was 1092 bytes   

