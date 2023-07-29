use valgrind to check memory leak without deleting serverconf
# Complete parser
    Check inproper "{" and "}" 
# Handle request
    First check if matching route, if yes, search for index, if no, check autoindex
    Then check if directory route
    First checki if is specific file, if yes then send the specific file, 
    then check for index, after that autoindex
    Default search for index first. If not found, then handle other
    Split the route, check mataching location block(if has "/" behind, then is a directory route)
        if is directory route, split the route, and find respective file
    if got index, use index.html, if no index, 
    if no limit_except present, dont check
    in no index present, search for file
    check if route ha multiple "/"
    when passing route, check for root directory, if have index variable
    Handle /fav.ico
    Check auto index and index which will be dominant
    when not member function failed, close socket and clear in FdSet

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
A server can listen to multiple port
Multiple worker thread