# webserv

## class
### Signal
> Wrapper around system signal handling

- methods

| Prototype                               | Description                                                                                                                                                             |
| --------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `void set(int sig, void (*func)(int));` | Associates a signal `sig` with a handler function `func`. Internally calls the system `signal()` function to define how the program reacts when the signal is received. |

### Config

> populate UnitConf_t (throw exception if error)

- attributes

| Name    | Type                      | Description                               |
| ------- | ------------------------- | ----------------------------------------- |
| n       | `int`                     | Max server count                          |
| configs | `std::vector<UnitConf_t>` | Vector of configuration data (UnitConf_t) |

- methods

| Prototype                               | Description                                                                             |
| --------------------------------------- | --------------------------------------------------------------------------------------- |
| `std::vector<UnitConf_t> getConfigs();` | Returns a vector containing all `UnitConf_t` configuration objects stored in the class. |
| `int getN();`                           | Returns an integer value `n`, number of servers.                                        |
| `void printInfo();`                     | Prints information in the configs.                                                      |



### SocketOperation
> all socket operations (throw exception if error)

- methods

| Prototype                                                | Description                                                                                                                               |
| -------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------- |
| `static int createSocket();`                             | Creates a new socket and returns its file descriptor.                                                                                     |
| `static void setsockopt_(int fd);`                       | Configures socket options (e.g., reuse address/port) on the given file descriptor `fd`.                                                   |
| `static void set_non_blocking(int fd);`                  | Sets the socket identified by `fd` to non-blocking mode.                                                                                  |
| `static void connect_(int fd, struct sockaddr_in addr);` | Initiates a connection from the socket `fd` to the specified address `addr`.                                                              |
| `static void bind_(int fd, struct sockaddr_in addr);`    | Binds the socket `fd` to the given local address `addr`.                                                                                  |
| `static void listen_(int fd, int backlog);`              | Marks the socket `fd` as a passive (listening) socket with a specified `backlog` queue size.                                              |
| `static clientData_t accept_(int fd);`                   | Accepts an incoming connection on socket `fd` and returns a `clientData_t` structure containing the client’s file descriptor and address. |

### Address
> all address operations (throw exception if error)

- attributes

| Name | Type                 | Description                                                |
| ---- | -------------------- | ---------------------------------------------------------- |
| addr | `struct sockaddr_in` | Stores the IPv4 address information (IP address and port). |

- methods

| Prototype                                      | Description                                                                                                |
| ---------------------------------------------- | ---------------------------------------------------------------------------------------------------------- |
| `Address(std::string host, std::string port);` | Constructs and initializes the address using the given `host` (IP or hostname) and `port` (string format). |
| `struct sockaddr_in& getAddr();`               | Returns a reference to the internal `sockaddr_in` structure for direct use in socket operations.           |

### SelectMultiplexer
> all I/O multiplexing operations (throw exception if error)

- attributes

| Name       | Type             | Description                                              |
| ---------- | ---------------- | -------------------------------------------------------- |
| master_set | `fd_set`         | Stores all registered file descriptors.                  |
| read_set   | `fd_set`         | Subset of descriptors to monitor for read readiness.     |
| write_set  | `fd_set`         | Subset of descriptors to monitor for write readiness.    |
| fd_max     | `int`            | Highest-numbered file descriptor currently tracked.      |
| timeout    | `struct timeval` | Timeout value used by `select()` to limit blocking time. |

- methods

| Prototype                       | Description                                                                                     |
| ------------------------------- | ----------------------------------------------------------------------------------------------- |
| `SelectMultiplexer(int fd);`    | Initializes the multiplexer and registers an initial file descriptor `fd`.                      |
| `void wait();`                  | Calls `select()` to monitor registered file descriptors for activity (read/write readiness).    |
| `void registerFd(int fd);`      | Adds a file descriptor `fd` to the monitored set (`master_set`) and updates `fd_max` if needed. |
| `void unregisterFd(int fd);`    | Removes a file descriptor `fd` from the monitored set.                                          |
| `int getFdMax();`               | Returns the current maximum file descriptor value being tracked.                                |
| `void setFdMax(int value);`     | Manually sets the maximum file descriptor value.                                                |
| `bool isReadyForRead(int fd);`  | Checks if the file descriptor `fd` is ready for reading after `select()` has been called.       |
| `bool isReadyForWrite(int fd);` | Checks if the file descriptor `fd` is ready for writing after `select()` has been called.       |


### ExceptionHandler
> all http related exceptions
### File
> all file operations (throw exception if error)

- methods

| Prototype                                                  | Description                                                                              |
| ---------------------------------------------------------- | ---------------------------------------------------------------------------------------- |
| `static std::string getFileContent(std::string fileName);` | Reads the content of the file specified by `fileName` and returns it as a `std::string`. |

### Validator
> all validation operations (throw exception if error)

- methods

| Prototype                                                                  | Description                                                                               |
| -------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------- |
| `static bool isPositiveInt(const std::string& s);`                         | Checks whether the string `s` represents a valid positive integer.                        |
| `static bool isIpV4(const std::string& s);`                                | Validates whether `s` is a correctly formatted IPv4 address.                              |
| `static bool isFile(const std::string& path);`                             | Checks if the given `path` exists and is a file.                                          |
| `static bool isDirectory(const std::string& path);`                        | Checks if the given `path` exists and is a directory.                                     |
| `static bool isFile(const std::string& path, std::string& errorMsg);`      | Same as `isFile`, but also sets `errorMsg` with details if validation fails.              |
| `static bool isDirectory(const std::string& path, std::string& errorMsg);` | Same as `isDirectory`, but also sets `errorMsg` with details if validation fails.         |
| `static bool validateHost(std::string host);`                              | Validates whether `host` is a valid IPv4 address.                             |
| `static bool validatePort(std::string port);`                              | Checks if `port` is a valid port number (numeric and within valid range).                 |
| `static bool validateMethod(std::string method);`                          | Validates whether `method` is an allowed HTTP method (e.g., GET, POST, DELETE).           |
| `static bool validateDirectoryListing(std::string directoryListing);`      | Validates a directory listing option (e.g., ON/OFF or on/off).                        |
| `static bool validateRoot(std::string root);`                              | Validates whether `root` is a valid directory path.                                       |
| `static bool validateUploads(std::string root, std::string uploads);`      | Validates upload configuration, typically ensuring `uploads` is valid relative to `root`. |
| `static bool validateIndex(std::string index);`                            | Validates whether `index` refers to a valid file (e.g., index.html).                      |
| `static bool validateMaxBodySize(std::string maxBodySize);`                | Validates that `maxBodySize` is a valid size value (numeric and within limits).           |
| `static bool isInVect(int integer, std::vector<int> vect);`                | Checks whether `integer` exists within the given vector `vect`.                           |


### RequestHandler
> handle incomming request data and checking if it is full (throw exception if error)
### HttpError
> error response string to match exception from ExceptionHandler (throw exception if error)
### HttpSuccess
> success response string to match exception from ExceptionHandler (throw exception if error)
### HttpRedirection
> redirection response string to match exception from ExceptionHandler (throw exception if error)
### StaticFile
> send static file response if success (throw exception if error) 
### Upload
> stream upload to file (throw exception if error) 
### DirectoryListing
> send directory listing response if success (throw exception if error)  
### CGI
> send CGI response if success (throw exception if error)
### ResponseHandler
> choose response type according to configuration (send response success/error)
```text
.
├── Config
│   ├── Config.cpp
│   └── Config.hpp
├── File
│   ├── File.cpp
│   └── File.hpp
├── main.cpp
├── Makefile
├── Multiplexing
│   ├── Multiplexer.cpp
│   └── Multiplexer.hpp
├── Network
│   ├── Address.cpp
│   ├── Address.hpp
│   ├── SocketOperation.cpp
│   └── SocketOperation.hpp
├── README.md
├── Request
│   ├── RequestHandler.cpp
│   └── RequestHandler.hpp
├── Response
│   ├── CGI.cpp
│   ├── CGI.hpp
│   ├── DirectoryListing.cpp
│   ├── DirectoryListing.hpp
│   ├── ExceptionHandler.cpp
│   ├── ExceptionHandler.hpp
│   ├── HttpError.cpp
│   ├── HttpError.hpp
│   ├── HttpRedirection.cpp
│   ├── HttpRedirection.hpp
│   ├── HttpSuccess.cpp
│   ├── HttpSuccess.hpp
│   ├── ReponseHandler.cpp
│   ├── ReponseHandler.hpp
│   ├── StaticFile.cpp
│   ├── StaticFile.hpp
│   ├── Upload.cpp
│   └── Upload.hpp
├── Signal
│   ├── Signal.cpp
│   └── Signal.hpp
├── Validator
│   ├── Validator.cpp
│   └── Validator.hpp
├── Webserv.cpp
└── Webserv.hpp

```