# NGINX

A C project to establish connection between a **C server** and **web page client** using **nginx** as a **reverse proxy server** facilitating the connection. The **Web page** makes **GET**, **POST** or **PUT** request to the server and the server sends back current count on receiving **GET** or updates count to value specified in **POST** or **PUT** request.  

## Prerequisites

Open **/etc/nginx/sites-available/default**   

Create a server block in it like below   

server  
{   
        listen 8083 default_server;   
        listen [::]:8083 default_server;   

        root /var/www/html;   
        index indexCount.html;   

        location / {   
                try_files $uri $uri/ =404;   
        }  

        location /Count/ {   
                proxy_pass http://127.0.0.1:9080;  
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

**Count** will be displayed on the webpage updated every **second**   
The user can change **count** by entering it inside either the **POST** or **PUT** box  

## Building with Make

### $ make count

Output: jsonCount

### $ make clean

Removes jsonCount
Removes the created JSON files
