import sys
from urllib.parse import unquote

variables = dict()

try:
    for arg in sys.argv[1].split("&"):
        name, value = arg.split("=")
        variables[name] = unquote(value)
except:
    print("ERROR: Contenido incorrecto", end='\r\n')

if "temperatura" not in variables:
    print("ERROR: 'temperatura' no definido", end='\r\n')
else:
    temp = float(variables['temperatura'])
    fahrenheit = temp*1.8 + 32
    print(fahrenheit, end='\r\n')

