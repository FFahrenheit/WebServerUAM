import sys
from urllib.parse import unquote

variables = dict()

try:
    for arg in sys.argv[1].split("&"):
        name, value = arg.split("=")
        variables[name] = unquote(value)
except:
    print("ERROR: Contenido incorrecto", end='\r\n')

if "nombre" not in variables:
    print("ERROR: 'nombre' no definido", end='\r\n')
else:
    print(f"Hola {variables['nombre']}!", end='\r\n')
