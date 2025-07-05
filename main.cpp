#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLuint texturaTerra = 0;  // ID da textura para a Terra
GLuint texturaAnelSaturno = 0; // ✅ DECLARAÇÃO GLOBAL CORRETA

// Estrutura para planeta/lua
struct Planeta {
    float raio;           // Tamanho relativo
    float distancia;      // Distância média do sol
    float excentricidade; // Excentricidade orbital
    float velocidadeOrbita; // Radianos/frame
    float anguloOrbita;   // Posição atual na órbita
    float inclinacaoOrbital; // Graus, inclinação da órbita
    float rotacaoAxial;   // Graus, rotação do próprio eixo
    float velocidadeRotacao; // Graus/frame
    float inclinacaoAxial; // Graus, inclinação axial
    float cor[3];         // Cor do planeta
    std::string nome;     // Nome do planeta
    std::vector<Planeta> luas; // Luas do planeta
    bool temAnel;        // Indica se o planeta tem anéis
    GLuint texturaID = 0; // ID da textura carregada (opcional)
    std::string caminhoTextura; // Caminho para o arquivo de textura
};

// Estrutura para estrelas
struct Estrela {
    float x, y, z;       // Posição da estrela
    float brilho;        // Brilho da estrela (0.5 a 1.0)
    float incremento;    // Velocidade do brilho
};

// Configurações globais
std::vector<Estrela> estrelas;
int planetaSelecionado = -1; // Índice do planeta selecionado (-1 = nenhum)
bool modoSeguimento = false;
const int NUM_ESTRELAS = 500;
const int FPS = 60;

// Definição do Sol
Planeta sol = {
    1.0f, 0.0f, 0.0f, 0, 0, 0, 0, 0, 0, 
    {1.0f, 1.0f, 0.0f}, "Sol", {}, false
};

GLuint texturaID = 0;
std::string caminhoTextura;

// Planetas do sistema solar
std::vector<Planeta> planetas = {
    {0.12f, 3.8f, 0.205f, 0.017f, 0, 7.0f, 0, 6.1f, 0.01f, 
     {0.6f, 0.6f, 0.6f}, "Mercurio", {}, false},
    {0.3f, 5.8f, 0.007f, 0.012f, 0, 3.4f, 0, -1.48f, 177.4f, 
     {0.9f, 0.7f, 0.2f}, "Venus", {}, false},
    {0.35f, 7.6f, 0.017f, 0.01f, 0, 7.15f, 0, 1.0f, 23.44f, 
     {0.2f, 0.5f, 1.0f}, "Terra", {
        {0.1f, 1.0f, 0.05f, 0.02f, 0, 5.14f, 0, 2.5f, 6.68f, 
         {0.8f, 0.8f, 0.8f}, "Lua", {}, false}
     }, false},
    {0.25f, 10.8f, 0.093f, 0.008f, 0, 1.85f, 0, 0.97f, 25.19f, 
     {0.8f, 0.3f, 0.1f}, "Marte", {}, false},
    {0.9f, 15.2f, 0.048f, 0.005f, 0, 1.3f, 0, 2.4f, 3.13f, 
     {0.9f, 0.7f, 0.5f}, "Jupiter", {}, false},
    {0.8f, 19.0f, 0.056f, 0.004f, 0, 2.5f, 0, 2.3f, 26.7f, 
     {0.9f, 0.8f, 0.6f}, "Saturno", {}, true},
    {0.65f, 23.0f, 0.047f, 0.003f, 0, 0.77f, 0, -1.0f, 97.8f, 
     {0.5f, 0.8f, 0.9f}, "Urano", {}, false},
    {0.6f, 27.0f, 0.009f, 0.002f, 0, 1.77f, 0, 1.1f, 28.3f, 
     {0.2f, 0.3f, 0.7f}, "Netuno", {}, false}
};

// Controles de câmera
float cameraAzim = 45.0f;
float cameraElev = 20.0f;
float cameraDist = 45.0f;
int lastX = -1, lastY = -1;
bool mouseDown = false;

// --- Funções auxiliares ---

void getPosicaoPlaneta(const Planeta& p, float& x, float& y, float& z) {
    x = p.distancia * cos(p.anguloOrbita) - p.distancia * p.excentricidade;
    y = 0.0f;
    z = p.distancia * sqrt(1 - p.excentricidade * p.excentricidade) * sin(p.anguloOrbita);
}

void desenhaTexto(const std::string &texto, float x, float y, float z) {
    glRasterPos3f(x, y, z);
    for (char c : texto) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
}

void desenhaOrbita(float a, float e, float inclinacao) {
    glPushMatrix();
    glRotatef(inclinacao, 0, 0, 1);
    glColor3f(0.6f, 0.6f, 0.6f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 100; ++i) {
        float ang = 2 * M_PI * i / 100;
        float x = a * cos(ang) - a * e;
        float z = a * sqrt(1 - e * e) * sin(ang);
        glVertex3f(x, 0, z);
    }
    glEnd();
    glPopMatrix();
}

void desenhaAnelSaturno(float raioInterno, float raioExterno) {
    glPushMatrix();
    glRotatef(90, 1, 0, 0);

    if (texturaAnelSaturno != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texturaAnelSaturno);
    } else {
        glColor4f(0.9f, 0.9f, 0.8f, 0.5f);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int segmentos = 100;
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segmentos; ++i) {
        float ang = 2.0f * M_PI * i / segmentos;
        float x = cos(ang);
        float y = sin(ang);
        float tx = (float)i / segmentos;

        glTexCoord2f(tx, 0.0f); glVertex3f(raioInterno * x, raioInterno * y, 0);
        glTexCoord2f(tx, 1.0f); glVertex3f(raioExterno * x, raioExterno * y, 0);
    }
    glEnd();

    if (texturaAnelSaturno != 0) glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glPopMatrix();
}

void desenhaEsfera(const Planeta& p) {
    if (p.texturaID != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, p.texturaID);

        GLUquadric* quad = gluNewQuadric();
        gluQuadricTexture(quad, GL_TRUE);
        gluQuadricNormals(quad, GLU_SMOOTH);
        gluSphere(quad, p.raio, 50, 50);
        gluDeleteQuadric(quad);

        glDisable(GL_TEXTURE_2D);
    } else {
        glColor3fv(p.cor);
        glutSolidSphere(p.raio, 50, 50);
    }
}

void desenhaPlaneta(const Planeta& p) {
    glPushMatrix();
    glRotatef(p.inclinacaoOrbital, 0, 0, 1);

    float x = p.distancia * cos(p.anguloOrbita) - p.distancia * p.excentricidade;
    float z = p.distancia * sqrt(1 - p.excentricidade * p.excentricidade) * sin(p.anguloOrbita);
    glTranslatef(x, 0, z);

    glRotatef(p.inclinacaoAxial, 0, 0, 1);
    glRotatef(p.rotacaoAxial, 0, 1, 0);

    desenhaEsfera(p);

    if (p.temAnel) {
        desenhaAnelSaturno(p.raio * 1.1f, p.raio * 2.5f);
    }

    desenhaTexto(p.nome, 0, p.raio + 0.1f, 0);

    for (const Planeta& lua : p.luas) {
        glPushMatrix();
        glRotatef(lua.inclinacaoOrbital, 0, 0, 1);
        float xLua = lua.distancia * cos(lua.anguloOrbita) - lua.distancia * lua.excentricidade;
        float zLua = lua.distancia * sqrt(1 - lua.excentricidade * lua.excentricidade) * sin(lua.anguloOrbita);
        glTranslatef(xLua, 0, zLua);
        glRotatef(lua.inclinacaoAxial, 0, 0, 1);
        glRotatef(lua.rotacaoAxial, 0, 1, 0);
        desenhaEsfera(lua);
        desenhaTexto(lua.nome, 0, lua.raio + 0.05f, 0);
        glPopMatrix();
    }
    glPopMatrix();
}

void inicializaEstrelas() {
    estrelas.clear();
    srand(static_cast<unsigned int>(time(0)));
    for (int i = 0; i < NUM_ESTRELAS; ++i) {
        Estrela e;
        e.x = (rand() % 2000 - 1000) / 20.0f;
        e.y = (rand() % 2000 - 1000) / 20.0f;
        e.z = (rand() % 2000 - 1000) / 20.0f;
        e.brilho = ((rand() % 100) / 100.0f) * 0.5f + 0.5f;
        e.incremento = ((rand() % 100) / 100.0f) * 0.01f;
        if (rand() % 2) e.incremento = -e.incremento;
        estrelas.push_back(e);
    }
}

void desenhaFundoEstrelas() {
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for (Estrela& e : estrelas) {
        e.brilho += e.incremento;
        if (e.brilho > 1.0f) {
            e.brilho = 1.0f;
            e.incremento = -e.incremento;
        } else if (e.brilho < 0.5f) {
            e.brilho = 0.5f;
            e.incremento = -e.incremento;
        }
        glColor3f(e.brilho, e.brilho, e.brilho);
        glVertex3f(e.x, e.y, e.z);
    }
    glEnd();
}

void desenhaSol() {
    glPushMatrix();

    GLfloat mat_emissao[] = {1.0f, 1.0f, 0.6f, 1.0f};
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emissao);

    if (sol.texturaID != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, sol.texturaID);

        GLUquadric* quad = gluNewQuadric();
        gluQuadricTexture(quad, GL_TRUE);
        gluQuadricNormals(quad, GLU_SMOOTH);
        gluSphere(quad, sol.raio, 60, 60);
        gluDeleteQuadric(quad);

        glDisable(GL_TEXTURE_2D);
    } else {
        glColor3f(1.0f, 1.0f, 0.6f);
        glutSolidSphere(sol.raio, 60, 60);
    }

    GLfloat zero_emissao[4] = {0, 0, 0, 1};
    glMaterialfv(GL_FRONT, GL_EMISSION, zero_emissao);

    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Configuração da câmera
    if (modoSeguimento && planetaSelecionado >= 0 && planetaSelecionado < planetas.size()) {
        const Planeta& p = planetas[planetaSelecionado];
        float x = p.distancia * cos(p.anguloOrbita) - p.distancia * p.excentricidade;
        float z = p.distancia * sqrt(1 - p.excentricidade * p.excentricidade) * sin(p.anguloOrbita);
        float px = x, py = 0.0f, pz = z;

        float camX = px + cameraDist * cosf(cameraAzim * M_PI / 180.0f);
        float camY = cameraDist * sinf(cameraElev * M_PI / 180.0f);
        float camZ = pz + cameraDist * sinf(cameraAzim * M_PI / 180.0f);

        gluLookAt(camX, camY, camZ, px, py, pz, 0, 1, 0);
    } else {
        float camX = cameraDist * cosf(cameraAzim * M_PI / 180.0f) * cosf(cameraElev * M_PI / 180.0f);
        float camY = cameraDist * sinf(cameraElev * M_PI / 180.0f);
        float camZ = cameraDist * sinf(cameraAzim * M_PI / 180.0f) * cosf(cameraElev * M_PI / 180.0f);
        gluLookAt(camX, camY, camZ, 0, 0, 0, 0, 1, 0);
    }

    // Luz
    GLfloat posLuz[] = {0, 0, 0, 1};
    GLfloat luzDifusa[] = {1.0f, 1.0f, 0.9f, 1.0f};
    GLfloat luzAmbiente[] = {0.1f, 0.1f, 0.1f, 1.0f};
    GLfloat luzEspecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, posLuz);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa);
    glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
    glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular);

    desenhaFundoEstrelas();
    desenhaSol();

    for (const Planeta& p : planetas) {
        desenhaOrbita(p.distancia, p.excentricidade, p.inclinacaoOrbital);
        desenhaPlaneta(p);
    }

    // HUD 2D
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1200, 0, 800);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glColor3f(1, 1, 1);

    // Mensagem superior
    if (modoSeguimento && planetaSelecionado >= 0 && planetaSelecionado < planetas.size()) {
        std::string info = "Seguindo " + planetas[planetaSelecionado].nome + ". Pressione 'F' para sair do modo de perseguicao.";
        glRasterPos2i(10, 770);
        for (char c : info) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    } else {
        std::string instrucao = "Pressione 'F' para ativar o modo de perseguicao.";
        std::string escolha = "Escolha o planeta com as teclas [1 a 8]: 1 - Mercurio  2 - Venus  3 - Terra  4 - Marte  5 - Jupiter  6 - Saturno  7 - Urano  8 - Netuno";
        glRasterPos2i(10, 770);
        for (char c : instrucao) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        glRasterPos2i(10, 740);
        for (char c : escolha) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }

    // Painel de dados
    const Planeta& p = (planetaSelecionado >= 0 && planetaSelecionado < planetas.size()) ? planetas[planetaSelecionado] : sol;
std::string composicao = (p.nome == "Sol") ? "Plasma" :
                         (p.nome == "Terra") ? "Rochoso" :
                         (p.nome == "Jupiter" || p.nome == "Saturno") ? "Gasoso" :
                         (p.nome == "Urano" || p.nome == "Netuno") ? "Gelo/Gasoso" :
                         "Rochoso";

std::vector<std::string> info = {
    "Informacoes do corpo celeste:",
    "Nome: " + p.nome,
    "Raio (relativo): " + std::to_string(p.raio),
    "Distancia ao Sol: " + std::to_string(p.distancia) + " UA",
    "Incl. Orbital: " + std::to_string(p.inclinacaoOrbital) + " graus",
    "Incl. Axial: " + std::to_string(p.inclinacaoAxial) + " graus",
    "Vel. Orbital: " + std::to_string(p.velocidadeOrbita * 149.6f * 100.0f) + " km/s",
    "Composicao: " + composicao
};

    int yBase = 180;
    for (const auto& linha : info) {
        glRasterPos2i(20, yBase);
        for (char c : linha) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
        yBase -= 20;
    }

    glEnable(GL_LIGHTING);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glutSwapBuffers();
}

void update(int value) {
    for (Planeta& p : planetas) {
        p.anguloOrbita += p.velocidadeOrbita;
        if (p.anguloOrbita > 2 * M_PI) p.anguloOrbita -= 2 * M_PI;

        p.rotacaoAxial += p.velocidadeRotacao;
        if (p.rotacaoAxial > 360) p.rotacaoAxial -= 360;

        for (Planeta& lua : p.luas) {
            lua.anguloOrbita += lua.velocidadeOrbita;
            if (lua.anguloOrbita > 2 * M_PI) lua.anguloOrbita -= 2 * M_PI;

            lua.rotacaoAxial += lua.velocidadeRotacao;
            if (lua.rotacaoAxial > 360) lua.rotacaoAxial -= 360;
        }
    }
    glutPostRedisplay();
    glutTimerFunc(1000/FPS, update, 0);
}

void mouseMotion(int x, int y) {
    if (mouseDown) {
        int dx = x - lastX;
        int dy = y - lastY;
        cameraAzim += dx * 0.5f;
        cameraElev += dy * 0.5f;
        cameraElev = std::max(-10.0f, std::min(89.0f, cameraElev));
        lastX = x;
        lastY = y;
    }
}

void mouseButton(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        mouseDown = (state == GLUT_DOWN);

        if (mouseDown) {
            lastX = x;
            lastY = y;
        }

        // Se já estiver no modo de perseguição, não tentar selecionar planeta
        if (!modoSeguimento && state == GLUT_DOWN) {
            GLbyte corPixel[4];
            GLint viewport[4];
            glGetIntegerv(GL_VIEWPORT, viewport);

            int mx = x;
            int my = viewport[3] - y;

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glLoadIdentity();
            gluLookAt(0, 0, cameraDist, 0, 0, 0, 0, 1, 0); // Câmera fixa p/ seleção

            for (size_t i = 0; i < planetas.size(); ++i) {
                glPushMatrix();
                const Planeta& p = planetas[i];
                float x = p.distancia * cos(p.anguloOrbita) - p.distancia * p.excentricidade;
                float z = p.distancia * sqrt(1 - p.excentricidade * p.excentricidade) * sin(p.anguloOrbita);
                glTranslatef(x, 0, z);
                glColor3ub(i + 1, 0, 0);
                glutSolidSphere(p.raio, 10, 10);
                glPopMatrix();
            }

            glFlush();
            glReadPixels(mx, my, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, corPixel);

            if (corPixel[0] > 0) {
                planetaSelecionado = corPixel[0] - 1;
                std::cout << "Planeta selecionado: " << planetas[planetaSelecionado].nome << std::endl;
            } else {
                planetaSelecionado = -1;
            }

            glutPostRedisplay();
        }
    }
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case '+':
            cameraDist = std::max(10.0f, cameraDist - 1.0f);
            break;
        case '-':
            cameraDist = std::min(80.0f, cameraDist + 1.0f);
            break;
        case 'f':
            modoSeguimento = !modoSeguimento;
            break;
        case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8':
            planetaSelecionado = key - '1'; // '1' = índice 0
            break;
    }
    glutPostRedisplay();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, (float)w / (float)h, 1.0, 200.0);
    glMatrixMode(GL_MODELVIEW);
}

GLuint carregaTextura(const char* arquivo) {
    int largura, altura, canais;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* dados = stbi_load(arquivo, &largura, &altura, &canais, 3);
    if (!dados) {
        std::cerr << "Erro ao carregar textura: " << arquivo << std::endl;
        return 0;
    }

    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Substituir glTexImage2D + glGenerateMipmap por:
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, largura, altura, GL_RGB, GL_UNSIGNED_BYTE, dados);

    stbi_image_free(dados);
    return texID;
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1200, 800);
    glutCreateWindow("Sistema Solar 3D - Simulação Planetária");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_TEXTURE_2D);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);

    inicializaEstrelas();

// Caminhos das texturas (ajuste conforme os arquivos que você baixou)
planetas[0].caminhoTextura = "../texturas/mercurio.png";
planetas[1].caminhoTextura = "../texturas/venus.png";
planetas[2].caminhoTextura = "../texturas/terra.png";   // PNG também funciona
planetas[3].caminhoTextura = "../texturas/marte.png";
planetas[4].caminhoTextura = "../texturas/jupiter.png";
planetas[5].caminhoTextura = "../texturas/saturno.png";
planetas[6].caminhoTextura = "../texturas/urano.png";
planetas[7].caminhoTextura = "../texturas/netuno.png";

// Carregar texturas
for (Planeta& p : planetas) {
    p.texturaID = carregaTextura(p.caminhoTextura.c_str());
    if (p.texturaID == 0) {
        std::cerr << "Falha ao carregar textura de " << p.nome << "!\n";
    }
}

// Texturas adicionais
planetas[2].luas[0].caminhoTextura = "../texturas/lua.png";
planetas[2].luas[0].texturaID = carregaTextura(planetas[2].luas[0].caminhoTextura.c_str());

sol.caminhoTextura = "../texturas/sol.png";
sol.texturaID = carregaTextura(sol.caminhoTextura.c_str());

texturaAnelSaturno = carregaTextura("../texturas/aneldesaturno.png");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(1000 / FPS, update, 0);
    glutMotionFunc(mouseMotion);
    glutMouseFunc(mouseButton);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}