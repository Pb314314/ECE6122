# this is the cmakefile on mac
set(CMAKE_CXX_STANDARD 11) # Set the C++ standard to C++11
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRCROOT ${PROJECT_SOURCE_DIR}/examples/sockets)

# Server source files
set(SERVER_SRC 
    ${SRCROOT}/server.cpp
    # Add other source files for the server if necessary
)

# Client source files
set(CLIENT_SRC 
    ${SRCROOT}/client.cpp
    # Add other source files for the client if necessary
)

# Define the server target
add_executable(server ${SERVER_SRC})
target_link_libraries(server sfml-network)
# Add other dependencies for the server if necessary

# Define the client target
add_executable(client ${CLIENT_SRC})
target_link_libraries(client sfml-network)
# Add other dependencies for the client if necessary
