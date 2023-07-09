Redo receive
Allow multiple client to connect to server(select)
Test listen to all port if not specified
Test address 0.0.0.0
Test connect to a port but used wrong port for client
Test connect to a specific host with a client
multiple port.
multiple server
Accept should not quit
if too long not receive header, send error


return index.html

sin_port = 0 bind to all port
 server_addr.sin_family = AF_INET;
    server_addr.sin_port = 0;  // Bind to all ports
    server_addr.sin_addr.s_addr = INADDR_ANY;  // Bind to all available interfaces

# Information
host cat etc/hostss

# Future Improvement
Add ipv6 support
A server can listen to multiple port
Multiple worker thread