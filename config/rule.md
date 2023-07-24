Listen :
	Can used both domain name (localhost) or ip address (127.0.0.1)
	Port number must be specified in any case.
	Can present multiple times. However,
		1) They must both point to same address, just changing port number
		2) They must both be in same type. If the fist one used localhost, the second one must also use localhost, and not 127.0.0.1
	Starting from second one, only the port number need to be specified

server_name:
	Default to localhost if not specified
	Will modified the "/etc/hosts" file
	If the address is "127.0.0.1", will add the server name to address after localhost
	Do nothing if other address present

autoindex
	only available in location block

