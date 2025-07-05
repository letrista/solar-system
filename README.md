<h1 align="center">Solar System 3D | Planetary Simulation</h1>
<p align="center">
  <b>OpenGL Project</b> • C++ 🧠 • Space Simulation 🌌 • Stylized View ✨
</p>

---

## 🖼️ Preview

<p align="center">
  <img src="https://github.com/letrista/solar-system/blob/main/preview.gif" alt="Preview da Simulação" width="700"/>
</p>

---

## 🧠 Sobre o Projeto

Este é um projeto em **C++ com OpenGL clássico (GLUT)** que simula o nosso sistema solar em 3D, com foco em visual, realismo estilizado e interações.

A simulação inclui:
- Planetas orbitando com **excentricidade e inclinação orbital**,
- **Rotação axial** e inclinação de cada planeta,
- Lua com rotação e órbita própria,
- Anel de Saturno com **textura translúcida**,
- Efeitos visuais como **estrelas pulsantes**, emissão de luz solar, partículas na corona, câmera de perseguição e sistema de seleção por clique.

---

## ⚙️ Tecnologias Utilizadas

- 🧠 **C++**  
- 🎨 **OpenGL (GLUT)**  
- 🔃 **GLU + GLUQuadric**  
- 🖼️ **Texturas com stb_image**  
- 🛰️ **Transformações 3D com inclinações reais**

---

## 🗂️ Estrutura do Projeto

```
solar-system/
├── main.cpp              # Código principal da simulação
├── stb_image.h           # Biblioteca para carregar texturas
├── CMakeLists.txt        # Arquivo de configuração para compilação via CMake
├── texturas/             # Pasta com as texturas usadas
│   ├── sol.png
│   ├── mercurio.png
│   ├── venus.png
│   ├── terra.png
│   ├── marte.png
│   ├── jupiter.png
│   ├── saturno.png
│   ├── urano.png
│   ├── netuno.png
│   ├── lua.png
│   └── aneldesaturno.png
└── preview.gif           # Preview do projeto
```

---

## 💡 Funcionalidades

- 🌌 Câmera com rotação via mouse e zoom com `+` e `-`
- 🪐 Planetas com órbitas inclinadas e elípticas
- 🌗 Luas com rotação e inclinação próprias
- 🔭 Modo de **câmera de perseguição** (`F`) focada em um planeta
- 🖱️ Seleção por clique (exibe dados do planeta)
- 🌟 Estrelas de fundo com brilho pulsante
- ☀️ Sol com textura e emissão de luz
- 💫 Anéis de Saturno texturizados com **alpha blending**
- 🖥️ HUD com informações detalhadas do corpo celeste
- ✨ Design visual limpo, interativo e educativo

---

## 🛠️ Como Rodar Localmente

1. Instale uma ferramenta com suporte a OpenGL/GLUT. Exemplos:
   - Windows: FreeGLUT + MinGW ou Visual Studio
   - Linux: `sudo apt install freeglut3-dev`
   - Mac: Xcode + GLUT headers

2. Compile com:

```bash
g++ main.cpp -o solar-system -lGL -lGLU -lglut
```

3. Execute:

```bash
./solar-system
```

### 🛠️ Usando CMake

```bash
mkdir build && cd build
cmake ..
make
./solar-system
```
---

## 🧠 Autor

<h3 align="center">Jackson de Oliveira ☕</h3>
<p align="center">
  <b>AKA Letrista</b> • Front-End Developer 💻 • Campinas, SP 🇧🇷
</p>

<p align="center">
  <a href="https://github.com/letrista">GitHub</a> • 
  <a href="mailto:jacksonndeoliveira@gmail.com">Email</a> • 
  <a href="https://letristadev.web.app">Portfolio</a>
</p>

---

<p align="center">
  ⌨️ Crafted with focus, caffeine and <code>C++</code>
</p>
