# Dependencias de Terceros

Esta biblioteca requiere las siguientes dependencias header-only:

## 1. nlohmann/json

**Versión recomendada:** 3.11.2 o superior

**Descripción:** Biblioteca JSON moderna para C++

**Instalación:**

### Opción A: Descarga directa
```bash
cd third_party
curl -O https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp
mkdir -p nlohmann
mv json.hpp nlohmann/
```

### Opción B: Git submodule
```bash
cd /Users/miguel/Projects/qhato/biz-map-system-movil/cpp-lib
git submodule add https://github.com/nlohmann/json.git third_party/nlohmann-json
```

### Opción C: Package manager (vcpkg, conan)
```bash
# vcpkg
vcpkg install nlohmann-json

# conan
conan install nlohmann-json/3.11.2@
```

**Repositorio:** https://github.com/nlohmann/json

---

## 2. cpp-httplib

**Versión recomendada:** 0.14.0 o superior

**Descripción:** Biblioteca HTTP/HTTPS cliente-servidor header-only

**Instalación:**

### Opción A: Descarga directa
```bash
cd third_party
curl -O https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h
```

### Opción B: Git submodule
```bash
cd /Users/miguel/Projects/qhato/biz-map-system-movil/cpp-lib
git submodule add https://github.com/yhirose/cpp-httplib.git third_party/cpp-httplib
```

**Repositorio:** https://github.com/yhirose/cpp-httplib

---

## Estructura esperada

```
third_party/
├── README.md                    # Este archivo
├── nlohmann/
│   └── json.hpp                 # JSON library
└── httplib.h                    # HTTP library
```

O con submodules:

```
third_party/
├── README.md
├── nlohmann-json/               # Submodule
│   └── single_include/nlohmann/json.hpp
└── cpp-httplib/                 # Submodule
    └── httplib.h
```

---

## Alternativa: Usar gestor de paquetes

Si prefieres usar un gestor de paquetes moderno, puedes configurar CMake para que use `FetchContent`:

```cmake
include(FetchContent)

FetchContent_Declare(
  json
  URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)

FetchContent_Declare(
  httplib
  URL https://github.com/yhirose/cpp-httplib/archive/v0.14.0.tar.gz
)

FetchContent_MakeAvailable(json httplib)
```

---

## Notas

- **nlohmann/json** es MIT licensed
- **cpp-httplib** es MIT licensed
- Ambas son header-only, no requieren compilación separada
- Compatible con C++17 y superior
