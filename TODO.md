valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes  --verbose     
     --log-file=valgrind-out.txt          ./webserv config/test.conf

Check nginx action with directory
Check leak as change to vector
redirection will overwrite index
file->redirect->index->autoindex->file

To add new error page
    configParser::addErrorPages
    configParser::initDefaultErrorPages
    Server::Server

# Parser 
    upload_store

# Handle request
    check upload dir can be open, if not, return 500
    max_body_size
    204 No response
    Fatest method to read file content and return string
    unkonwon request
    when passing route, check for root directory, if have index variable
    when not member function failed, close socket and clear in FdSet
    sent bad request
    check http version

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