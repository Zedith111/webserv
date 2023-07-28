use valgrind to check memory leak without deleting serverconf
when error, need successfully free location map

Check path is absolute or relative
# Complete parser
    Check if use relative path, will add "." to the beginning
    Test location block
        missing root(need automatic inherit from server block)
    Check inproper "{" and "}" 
    Check root has "/" at the end, if yes, delete it
    add "./" to the beginning of root/aboslute or relative path
    location block
        limit except
    If no limit except, set method to all
# Handle request
    check if route ha multiple "/"
    when passing route, check for root directory, if have index variable
    Handle /fav.ico
    Check auto index and index which will be dominant
    when not member function failed, close socket and clear in FdSet

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