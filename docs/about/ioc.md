## What is Inversion of Control (IoC)
> In software engineering, inversion of control (IoC) is a design pattern in which custom-written portions of a computer program receive the flow of control from a generic framework. The term "inversion" is historical: a software architecture with this design "inverts" control as compared to procedural programming. In procedural programming, a program's custom code calls reusable libraries to take care of generic tasks, but with inversion of control, it is the framework that calls the custom code.
> 
> -- <cite><a href="https://en.wikipedia.org/wiki/Inversion_of_control">Wikipedia</a></cite>

**TL;DR**: The lib control's the flow based on modular, extensible and composable implementations. Default implementations are provided on OS, API and Feature levels. IoC allows the Application to add, replace and disable implementations.

## Why do we use it Anthragon  
The use of IoC was motivated by the following topics:

- Uniform platform independent Interfaces: There shall always be only one Interface/Class responsible for one things (One Filesystem Interface, One Image Interface, and so on)
- Limitless implementations: There shall be multiple ways of implementations. While `ant.os.filesystem` should be served by the platform independent implementations there should also be options for dynamic image implementations like `ant.image.png`, `ant.image.jpg` and so on.
- Modularity: Every set of implementations shall be implemented in its own module. This shall allow the application to decide wich modules shall be included in the lib and thus wich dependencies are required.  
- Independent core: The core shall be completely decoupled from actual implementations and be provided in a completely standard only way,
- Head-less core: The core shall be completely headless and allow all kinds of IO operations no matter how absurd (Like displaying the UI by encoding and streaming a video signal over the network). 

## How do we use it in Anthragon
In Anthragon we use IoC for literally everything. We have one big header-only library that defines all interfaces that are standardized (However the IoC-Container is NOT limited to these. You can abuse it like you want). The Implementations are defined in modules that can be conditionally compiled as required. The modules are defined on OS, API and Feature base. The implementations are loaded with a module loader lib. The module loader can load modules from static and dynamic libs. 
