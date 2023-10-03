valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes  --verbose     
     --log-file=valgrind-out.txt          ./webserv config/test.conf

To add new error page
    configParser::addErrorPages
    configParser::initDefaultErrorPages
    Server::Server

Fix client max body size
upper cgi


## server
    Multiple server work differently
    different port can be connect
    duplicate port


## Get
    direct file/not found
    index
    autoindex
        nesting directory with autoindex
        failed
    redirection
    default index
    Precendence->have both

## Post
    normal and multipart save
    Duplicate upload of same file
    upload large file

## Delete

## CGI
    Hello
    print env,with query
    interpretor not found

# To Do
    Check leak


    



# CGI script
print.cpp -> print env -> compile as print.cgi
https://www.youtube.com/watch?v=cP1fN6xf3nI&ab_channel=nptelhrdv

https://w3.cs.jmu.edu/kirkpams/OpenCSF/Books/csf/html/Extended4CGI.html

https://www.oreilly.com/openbook/cgi/ch02_02.html

write->query string to some file


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


