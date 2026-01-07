# Notes

## NGINX

NGINX is a reverse proxy server designed to serve as an intermediate between client and backend servers.   
It also functions as a web server for linux devices to host web connections.   

After setting up nginx, linux device can load a web page in a browser after formatting the webpage using a .html file    
The default web page after nginx installation will be located in **/var/www/html/** directory of name **index.nginx-debian.html**   
This file contains the format for a basic webpage design   

The default port for listening to web connections is set to be port 80 inside **/etc/nginx/sites-available/default** inside the default server block    
The priority for .html files is set using **index**   
The default base directory root is set to **/var/www/html/**    
The hostname to be handled is set using **server_name** which on default is **_** . This lets it handle all hostnames that can access the port   
The location directive helps in handling requests. It can be used to reject specific files when establishing web connection. It can also return error messages when particular cases are not met. On default it is configured to return 404 when the requested page is not found   
The **location** directive is used to check and specify further details about the request
These default settings can be changed inside the default file.  

## Web connection

**async functions** - defines an asynchronous function so browser doesn't have to wait until response arrives    
**try {** - tries if the code inside works if not moves to catch   
**catch {** - usually used to display errors   
**await fetch** - usually used besides async. Sends a request to fetch data and wait until response arrives   
**setInterval** - Used to set a time interval between to function calls. It is what facilitates constant polling of time and date. without it the function runs only once   
This is how a communication between client(browser) and server happens   
Browser -> NGINX server(reverse proxy) -> Backend server(C server) -> Gets data -> sends it back as response -> Parse the Data -> give to webpage   

JSON is a file format where a single json object consists of one or more key value pairs   
Dynamic JSON is a type of JSON that changes during run time.   

The server should send **HTTP Response header** before sending data to let the client know the type and length of data send for proper parsing and to avoid chunk errors     
The client (javascript) parses this data to get needed information  

The reverse proxy server block is created inside etc/nginx/sites-available/default   
In my example I run an nginx server on localhost port 8085. the nginx port then looks for the request and then connects to the needed backend server.   
The javascript uses fetch api to continuously get response from the server to update date and time on the webpage   

Check if the server is responding using "curl http://127.0.0.1:8085/time/"   

There are different ways to request a connection with the backend server.
These are collectively  termed HTTP request methods
The main five HTTP request methods are

GET - GET request is used to access resource from the server
POST - POST request creates a resource in the server. It is the primary method of giving data to the server from the web page
PUT- PUT is used to modify the data or resource. It will replace the specified data with the modified data completely
PATCH - PATCH is used in cases where only partial modification of data is needed
DELETE - DELETE is used to delete a resource from the server

