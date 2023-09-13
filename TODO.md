valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes  --verbose     
     --log-file=valgrind-out.txt          ./webserv config/test.conf

To add new error page
    configParser::addErrorPages
    configParser::initDefaultErrorPages
    Server::Server

# NGINX Check
location /dir {
    index Youpie
}
when access /dir/nop will return 404 or not

Post/Put to /some_path, check will save it.
upload store and path, which will be precednece

# Test
Check leak
## Get
    direct file
    index
    autoindex
        nesting directory with autoindex
        failed
    redirection

## Post
    Duplicate upload of same file

# To Do
    Check file_path creation
    When nested autoindex, the file path does not add up
    Add error handling, when receive failed FD_CLR read_fd
    Add error handling, when send failed FD_CLR write_fd
    Add error handling, when select faied

    Handle request and send response not at same for loop

    Check receive
    Check send
    Fix cgi interpretor
    fix when error no comeout
    Do env



# Parser 
    Rework cgi parser
    When no root and cgi bin specified, use current directory as root

# Handle request
    Improve path finding
    new cgi request struct
    fix leak when invalid cgi_bin path
    fix when recv failed
    fix when select bad file descriptor(google serach :continue when select() failed)
    check is cgi request
    execute cgi
    when select failed, handle properly,dont quit
    when not member function failed, close socket and clear in FdSet
    gprod
    flush
    
Location block before server block
Mulitiple root present
Check leak
Handle address other than localhost
parser->data structure
    map-><path, actual path>
location->data structure
Add map to store request and response
separate header and body
nginx config


Test listen to all port if not specified
Test address 0.0.0.0
Test connect to a port but used wrong port for client
Test connect to a specific host with a client
multiple port.
multiple server
Accept should not quit
if too long not receive header, send error
/directory/ should have different file


sin_port = 0 bind to all port
server_addr.sin_family = AF_INET;
    server_addr.sin_port = 0;  // Bind to all ports
    server_addr.sin_addr.s_addr = INADDR_ANY;  // Bind to all available interfaces

# Information
host cat etc/hosts

# Future Improvement
Add ipv6 support
Multiple worker thread
Handle multiple request type


# CGI env
GATEWAY_INTERFACE=CGI/1.1
SERVER_NAME=Get from config
SERVER_SOFTWARE=webserv
SERVER_PROTOCOL=HTTP/1.1
SERVER_PORT= get from requet
REQUEST_METHOD=pass down
PATH_INFO=path to cgi handler
PATH_TRANSLATED=actual path to cgi handler
SCRIPT_NAME=path to cgi handler
DOCUMENT_ROOT= get from config
QUERY_STRING=empty
REMOTE_HOST= get from request(HOST)
REMOTE_ADDR= get from request
CONTENT_TYPE= */*
CONTENT_LENGTH= get from request

# CGI script
print.cpp -> print env -> compile as print.cgi
https://www.youtube.com/watch?v=cP1fN6xf3nI&ab_channel=nptelhrdv

https://w3.cs.jmu.edu/kirkpams/OpenCSF/Books/csf/html/Extended4CGI.html

https://www.oreilly.com/openbook/cgi/ch02_02.html

write->query string to some file

# CGI 
Add handler "cgi-dir" extension
better method to detect CGI script
Check method
    Get->get query string
    Post->in body

<HTML>
<HEAD><TITLE>Simple Form!</TITLE></HEAD>
<BODY>
<H1>Simple Form!</H1>
<HR>
<FORM ACTION="/cgi-bin/unix.pl" METHOD="GET">
Command: <INPUT TYPE="text" NAME="command" SIZE=40>
<P>
<INPUT TYPE="submit" VALUE="Submit Form!">
<INPUT TYPE="reset"  VALUE="Clear Form">
</FORM>
<HR>
</BODY>
</HTML>

http://some.machine/cgi-bin/display.pl/cgi/cgi_doc.txt
Since the server knows that display.pl is the name of the program, the string "/cgi/cgi_doc.txt" is stored in the environment variable PATH_INFO. Meanwhile, the variable PATH_TRANSLATED is also set, which maps the information stored in PATH_INFO to the document root directory (e.g., /usr/local/etc/httpd/ public/cgi/cgi-doc.txt).


change handle method to class, handler inherit a base class which contain handleError function


Path finding
    -Check for particular server, through port(new map to map socket_fd to server)
    -Check for cgi path
    -Check for direct access
    -Check for location path
        -Check for max file size




https://github.com/cclaude42/webserv/blob/master/srcs/cgi/CgiHandler.cpp