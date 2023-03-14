import sys
from urllib.parse import unquote

print("Parseo de variables: ")
variables = dict()

try:
    for arg in sys.argv[1].split("&"):
        name, value = arg.split("=")
        variables[name] = unquote(value)
except:
    print("ERROR: Contenido incorrecto", end='\r\n')
    
print(variables, end='\r\n')

print("Unreachable by server")