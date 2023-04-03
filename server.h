#ifndef SERVER_H
#define SERVER_H

/* A server listens to a port and handles multiple connections */
class Server {
public:
	/* Creates a server that listens to a port */
	explicit Server(int port);
	/* Removes all registered connections */
	virtual ~Server();
	/* Returns true if the server has been initialized correctly */
	bool isReady() const;
	/* Waits for activity on the port. Returns a previously registered
	connection object if an existing client wishes to communicate,
	nullptr if a new client wishes to communicate */
	std::shared_ptr<Connection> waitForActivity() const;
	/* Registers a new connection */
	void registerConnection(const std::shared_ptr<Connection>& conn);
	/* Deregisters a connection */
	void deregisterConnection(const std::shared_ptr<Connection>& conn);
	/* Servers cannot be copied */
	Server(const Server&) = delete;
	Server& operator=(const Server&) = delete;
	Server& operator=(Server&&) = delete;
	Server(Server&&);
};

#endif