# CGI Parser


A C project to establish connection between a **C backend server** and **web page client** using **nginx** and **CGI** facilitating the connection. The **Web page** makes **GET**, **POST** or **PUT** request to the server. The **CGI process** parses the headers and send only relevant part of request to **backend**. The **backend** after processing the request sends back response to the **browser**.
On **GET** request , the server sends back current **count**
A **POST** or **PUT** request is made when web page want to update the count with a custom value


## Prerequisites

### Install and setup nginx  

sudo apt install nginx  
nginx -v  
sudo systemctl enable nginx  
sudo systemctl start nginx  

### Install fcgiwrap extension

sudo apt install fcgiwrap  

Open **/etc/nginx/sites-available/default**   

### Create a server block in it like below   

server {  
	listen 8084 default_server;  
	listen [::]:8084 default_server;  

	root /var/www/html;  
	index indexCount.html;  

	location / {  
		try_files $uri $uri/ =404;  
	}  

	location /Count/ {  
		fastcgi_buffering off;  

		include fastcgi_params;  
		fastcgi_pass unix:/var/run/fcgiwrap.socket;  

		fastcgi_param SCRIPT_FILENAME /var/www/cgi-bin/parser.cgi;  

		fastcgi_param QUERY_STRING     $query_string;  
		fastcgi_param REQUEST_METHOD   $request_method;  
		fastcgi_param CONTENT_TYPE     $content_type;  
		fastcgi_param CONTENT_LENGTH   $content_length;  
	}  
}  

Copy **indexCount.html, scriptGetCount.js, scriptGiveCount.js, styleTime.css** to **/var/www/html/**  

### Setup CGI binary

cd /var/www/  
mkdir cgi-bin  
sudo chmod 775 /var/www/cgi-bin  
sudo chown root:www-data /var/www/cgi-bin  

**Copy** parse.cgi to mkdir  

sudo chmod 755 /var/www/cgi-bin/cgi-bin/parser.cgi  
sudo chown www-data:www-data /var/www/cgi-bin/parser.cgi  

### Run commands

sudo nginx -t   
sudo systemctl reload nginx  


create and run **jsonTime**


Open webpage http://localhost:8085    


## Features


**Count** will be displayed on the webpage updated every **second**   
The user can change **count** by entering it inside either the **POST** or **PUT** box  


## Building with Make

### CGI

#### make parse

Output: parse.cgi

#### make clean

removes parse.cgi

### Backend 

#### $ make count

Output: jsonCount

#### make clean

removes jsonCount
removes count.json
