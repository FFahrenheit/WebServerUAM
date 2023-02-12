import socket
import timeit

setup = "import socket"

main = """
servername = '192.168.11.129'
port = 8080
body = b"Hola desde aqui"


for i in range(100):
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect((servername,port))

    client_socket.send(body)
    modifiedSentence = client_socket.recv(1024)

    client_socket.close()
"""

if __name__ == '__main__':
    print(
        timeit.timeit(setup=setup,
        stmt = main,
        number=100)
    )
    #main()