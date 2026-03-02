# Laboratorio de Compiladores

Laboratorio de Compiladores - Diseño e implementación de un lenguaje de programación propio y su compilador.

**Curso:** Compiladores  
**Semestre:** 2026-2  
**Institución:** Facultad de Ciencias, UNAM

> Estás viendo la **versión en español** de este documento.  
> También disponible en inglés:
> [![English](https://img.shields.io/badge/language-English-blue)](README.md)

**Trabajo en Progreso** - El README se completará de forma incremental

## Acerca del proyecto

Este repositorio documenta el desarrollo de un compilador completo como parte del curso Compiladores 2026-2. El proyecto evoluciona a través de 11 prácticas enfocadas que abarcan el análisis léxico, sintáctico y semántico hasta la generación de código, culminando en un proyecto integrador final: un lenguaje de programación totalmente funcional diseñado para un dominio específico [por definir] como el control de robots, juegos de estrategia, automatización, etc.

## Estructura del curso

El laboratorio se divide en **11 prácticas enfocadas** y un **proyecto final integrador**:

| Módulo | Cantidad | Enfoque Técnico |
|--------|----------|-----------------|
| **I. Análisis Léxico** | 4 Prácticas | Diseño de Autómatas Finitos (DFA/NFA), expresiones regulares, manejo de buffers y tokenización eficiente. **Construcción del *Scanner***. |
| **II. Análisis Sintáctico** | 4 Prácticas | Gramáticas libres de contexto, algoritmos de análisis (descendente/ascendente), manejo de errores sintácticos y construcción del Árbol de Sintaxis Abstracta (AST). **Construcción del *Parser***. |
| **III. Análisis Semántico** | 2 Prácticas | Validación de tipos, gestión de alcances (scopes), tablas de símbolos y reglas de inferencia lógica. El **"cerebro"** del compilador. |
| **IV. Generación de Código** | 1 Práctica | Traducción del AST validado a una representación intermedia (Código de 3 direcciones o P-Code) lista para ejecución en una máquina virtual simplificada. |

### Progreso de Módulos

| Módulo | Estado | Prácticas Completadas |
|--------|--------|-------------------|
| Análisis Léxico | Iniciado | 1/4 |
| Análisis Sintáctico | No Iniciado | 0/4 |
| Análisis Semántico | No Iniciado | 0/2 |
| Generación de Código | No Iniciado | 0/1 |

## Tecnologías y Herramientas

### Implementación Principal
**Lenguaje:** C 

### Dependencias
- **Bibliotecas Estándar:** 
  - `stdio.h` - Operaciones de E/S
  - `stdlib.h` - Manejo de memoria
  - `string.h` - Manipulación de cadenas
- **Herramientas:** 
  - `flex` - Generador rápido de analizadores léxicos
  - `bison` - Generador de parsers


### Contrato de Interacción (Entrada/Salida)

Todos los programas siguen el **contrato de pipeline de Unix**:

| Stream | Propósito | Uso |
|--------|---------|-------|
| **stdin** | Entrada de código fuente | Lectura desde entrada estándar o redirección de archivo |
| **stdout** | Resultados (tokens, AST, código) | Salida de resultados de compilación |
| **stderr** | Mensajes de error | Reporte de errores y advertencias de compilación |

## Cómo Usar

Cada práctica vive en su propio directorio. Navege a la carpeta correspondiente y siga su README para las instrucciones de compilación y ejecución.

| Practica | Directorio | Descripción |
|----------|-----------|-------------|
| Practica 1 | [`regex_to_nfa/`](regex_to_nfa/README.md) | De Expresiones Regulares a NFA |


## Estrategia de Ramas

Este repositorio implementa un **Git Flow modificado** adaptado específicamente para la metodología incremental del curso de Laboratorio de Compiladores. La estrategia equilibra el rigor de Git Flow con los requisitos académicos de demostrar aprendizaje progresivo a través de once prácticas distintas y un proyecto integrativo culminante.

El modelo de ramificación mantiene dos ramas permanentes: `main` sirve como la rama de lanzamientos estables para entregas oficiales al instructor, mientras que `develop` funciona como la rama de integración para el desarrollo continuo. Todo el trabajo de desarrollo se origina desde develop, que actúa como la única fuente de verdad para el estado actual del proyecto. El modelo se complementa con ramas de práctica efímeras que aíslan las tareas individuales.

### Arquitectura de Ramas de Soporte

El repositorio emplea dos categorías de ramas de soporte para organizar el trabajo a través de diferentes ámbitos temporales y lógicos.

Las ramas de módulo representan ramas de características de larga duración alineadas con los cuatro módulos principales del curso. Estas ramas se crean desde develop al inicio de cada módulo y permanecen activas durante la implementación de todas las prácticas dentro de ese módulo. Las cuatro ramas de módulo (`module-1-lexical`, `module-2-syntactic`, `module-3-semantic` y `module-4-generation`) agregan prácticas relacionadas dentro de cada componente principal del compilador.

Las ramas de práctica constituyen ramas de desarrollo de corta duración dedicadas a tareas individuales. Cada rama de práctica sigue la convención de nomenclatura `practice-[número]-[característica-descriptiva]`, donde el número corresponde a la secuencia de práctica y el descriptor de característica identifica la funcionalidad específica que se está implementando. Estas ramas se crean desde develop, se utilizan para trabajo de implementación enfocado y posteriormente se fusionan en sus respectivas ramas de módulo al completarse. Las ramas de práctica se eliminan después de una integración exitosa para mantener la higiene del repositorio.

### Flujo de Trabajo de Desarrollo

El ciclo de desarrollo sigue un patrón de fusión jerárquico diseñado para acomodar tanto la organización modular de los componentes del compilador como los requisitos de entrega incremental del curso. Cada práctica debe completar su ciclo de integración completo antes de que comience la práctica subsecuente, asegurando que cada nueva tarea se construya sobre todo el trabajo previamente completado.

Al comenzar una nueva práctica, se crea una rama específica de práctica desde el estado actual de develop. Este entorno aislado permite trabajo de implementación enfocado mientras se preserva la estabilidad de la línea de desarrollo principal. Todos los commits siguen la especificación de Conventional Commits para establecer un historial de versión semántico que documenta la evolución de cada componente del compilador. Al completar la implementación y prueba de una práctica, la rama de práctica se fusiona en su rama de módulo correspondiente, consolidando funcionalidad relacionada dentro del componente apropiado del compilador.

Cada práctica completada requiere entrega formal al instructor del curso. Después de la integración de la práctica en la rama de módulo, se crea una rama de release desde la rama de módulo para preparación final. Esta rama de release se somete a actualizaciones finales de documentación y procedimientos de verificación antes de fusionarse en main con una etiqueta de versión acompañante que identifica la entrega específica de la práctica. El release posteriormente se propaga de vuelta a develop para asegurar que todos los refinamientos hechos durante la preparación del release permanezcan sincronizados a través de la línea de desarrollo activa. Este paso de sincronización es crítico porque asegura que cuando la siguiente práctica se ramifique desde develop, contendrá todo el código de prácticas anteriores más cualquier mejora hecha durante la preparación del release.

Cuando todas las prácticas dentro de un módulo están completas, la rama de módulo se fusiona en develop para pruebas de integración comprensivas. Luego se crea un release a nivel de módulo desde develop, fusionándose en main con una etiqueta de completación de módulo que confirma la implementación exitosa de una fase completa del compilador.

```bash
# Inicio e implementación de práctica
git checkout develop
git checkout -b practice-01-dfa
git commit -m "feat(lexer): implement DFA for identifier recognition"

# Completación de práctica e integración al módulo
git checkout module-1-lexical
git merge practice-01-dfa

# Entrega individual de práctica al instructor
git checkout -b release/v1.1-practice-01
git checkout main
git merge --no-ff release/v1.1-practice-01
git tag -a v1.1-practice-01 -m "Practice 1: DFA Implementation - Official Delivery"
git checkout develop
git merge --no-ff release/v1.1-practice-01
# develop ahora contiene practice-01, lista para que practice-02 se ramifique

# Continuar con las prácticas subsecuentes en el mismo módulo
# Cada práctica sigue el mismo ciclo, ramificándose desde develop actualizado

# Completación del módulo después de que todas las prácticas han sido entregadas
git checkout develop
git merge module-1-lexical
git checkout -b release/v1.0-module-lexical
git checkout main
git merge --no-ff release/v1.0-module-lexical
git tag -a v1.0-module-lexical -m "Module I: Lexical Analysis Complete"
git checkout develop
git merge -
```

### Etiquetas de Release

Los entregables principales se etiquetan para rastreo de versiones. Las completaciones de módulos reciben etiquetas siguiendo el patrón v1.0-module-lexical, v2.0-module-syntactic, v3.0-module-semantic y v4.0-final-project. Las entregas de prácticas individuales reciben etiquetas intermedias como v1.1-practice-01, v1.2-practice-02, y así sucesivamente, permitiendo tanto al instructor como al equipo de desarrollo referenciar puntos específicos de entrega a lo largo del semestre.

El proyecto integrativo final será desarrollado en una rama dedicada nombrada `integrated-project`, que será creada desde develop después de que todo el trabajo de módulos esté completo.

## Licencia

Licencia MIT - ver [LICENSE](LICENSE)

<div align="center">

**[Volver al inicio](#laboratorio-de-compiladores)**

</div>