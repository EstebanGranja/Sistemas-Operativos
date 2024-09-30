
README - myBash Project
Descripción General
Este proyecto implementa un shell simplificado llamado myBash, diseñado para ejecutar comandos básicos y manejar pipelines. La estructura principal del código está dividida en varios módulos, cada uno con una responsabilidad específica. El shell permite la ejecución de comandos externos, así como comandos internos (cd, help, exit), utilizando funcionalidades como redirección de entrada/salida y el manejo de procesos.

Estructura del Proyecto

1. Builtin
El archivo builtin.c define las funciones para manejar comandos internos (builtins). Estos comandos son:

cd: Cambia el directorio actual.
help: Muestra información sobre los comandos disponibles.
exit: Cierra el shell.

2. Command
El archivo command.c define la estructura y operaciones para manejar los comandos simples (scommand) y las pipelines (pipeline).

scommand: Representa un comando simple con sus argumentos y redirecciones de entrada/salida.
pipeline: Conjunto de comandos simples conectados por pipes (|), que se ejecutan en secuencia.

3. Execute
El archivo execute.c contiene las funciones para ejecutar los comandos y pipelines. Aquí se maneja la creación de procesos hijos, la redirección de entrada/salida y la conexión de comandos en una pipeline.

4. Main
El archivo main.c es el punto de entrada del programa. Se encarga de mostrar el prompt, leer la entrada del usuario, y pasar el control a las funciones de parsing y ejecución de comandos.

5. Parsing
El archivo parsing.c maneja el análisis sintáctico de la entrada del usuario, convirtiendo la cadena de entrada en estructuras de datos scommand y pipeline, que luego se ejecutan.

Compilación y Ejecución
Para compilar el proyecto, utiliza un compilador compatible con C. Por ejemplo:

gcc -o mybash main.c builtin.c command.c execute.c parsing.c -lglib-2.0

Para ejecutar el shell:

./mybash

Uso
El shell muestra un prompt indicando el directorio actual. Los usuarios pueden ingresar comandos como lo harían en una consola típica. Se soportan los siguientes comandos:

cd <directorio>: Cambia el directorio de trabajo.
help: Muestra ayuda e información sobre los comandos internos.
exit: Sale del shell.
También se pueden utilizar comandos externos, redirección de entrada/salida (<, >) y pipelines (|).

Autores
Martín Binaghi
Ramiro Álvarez
Esteban Granja
Emilio Lobos

Este shell es un proyecto educativo diseñado para comprender y practicar la creación de un intérprete de comandos básico en C.