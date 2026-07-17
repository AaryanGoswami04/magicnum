FROM alpine:latest

# Install the compiler and make
RUN apk add --no-cache g++ make curl

WORKDIR /app

# Download the single-header web framework
RUN curl -O https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h

# Copy our source code
COPY main.cpp .

# Compile the server with threading enabled
RUN g++ -O3 -pthread main.cpp -o server

# Expose the port and run the executable
EXPOSE 8080
CMD ["./server"]