# NGINX

A C project to establish connection between a **C server** and **web page client** using **nginx** as a **reverse proxy server** facilitating the connection. The **C server** creates a **dynamic JSON** object containing date and time and sends to client. The **web page** using **javascript** fetch and parse the time and date from the server every **1s.**

## Prerequisites

### Setting up NGINX

Install and verify nginx using the below commands   

sudo apt install nginx   
nginx -v    

Now run nginx using   

sudo systemctl enable nginx      
sudo systemctl start nginx     

Check its status

sudo systemctl status nginx    
curl http://localhost

If the latter command shows an nginx welcome page then nginx is succesfully setup

### Facilitating client server connection

Open **/etc/nginx/sites-available/default**   

Create a server block in it like below   

server {   
        listen 8085 default_server;   
        listen [::]:8085 default_server;   

        root /var/www/html;   
        index indexTime.html;  

        location / {  
                try_files $uri $uri/ =404;  
        }  

        location /time/ {   
                proxy_pass http://127.0.0.1:9079;  
                proxy_set_header Host $host;   
                proxy_buffering off;  
        }  
}   

Copy **indexTime.html, scriptTime.js, styleTime.css** to **/var/www/html/**

Run commands

sudo nginx -t   
sudo systemctl reload nginx  

create and run **jsonTime**

Open webpage http://localhost:8085    

## Features

**Current Date** and **Current Time** are displayed on the webpage in two different boxes   
The **time** box updates every **second** and **Date** box updates every **day**

## Building with Make

### $ make time

Output: jsonTime

### $ make clean

Removes jsonTime
