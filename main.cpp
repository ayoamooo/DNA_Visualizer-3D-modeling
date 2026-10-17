#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <glm/glm.hpp>
#include <vector>
#include <cmath>
#include <ctime>
#include <cstdio>
#include <algorithm>
#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glut.h"

const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;

enum MoleculeType { DNA, RNA, PROTEIN };
enum InteractionMode { NONE, DNA_RNA_HYBRID, PROTEIN_BINDING, TRANSCRIPTION };

struct Nucleotide {
    glm::vec3 position;
    glm::vec3 color;
    float radius;
    float rotation;
    bool paired = false;
};

struct Molecule {
    MoleculeType type;
    std::vector<Nucleotide> nucleotides;
    glm::vec3 position;
};

// Глобальные переменные
std::vector<Molecule> molecules;
bool show_controls = true;
bool auto_rotate = false;
float camera_distance = 26.0f;
float camera_angle_x = 1.2f;
float camera_angle_y = 0.0f;
int last_mouse_x = 0;
int last_mouse_y = 0;
bool mouse_pressed = false;
InteractionMode current_mode = NONE;
float simulation_speed = 1.0f;
bool show_bonds = true;
bool show_interactions = true;

const glm::vec3 DNA_COLORS[4] = {
    {0.0f, 0.5f, 1.0f},  // Adenine
    {1.0f, 0.5f, 0.0f},  // Thymine
    {0.5f, 0.0f, 1.0f},  // Cytosine
    {1.0f, 0.0f, 0.5f}   // Guanine
};

const glm::vec3 RNA_COLORS[4] = {
    {1.0f, 0.3f, 0.3f},  // Adenine
    {0.8f, 0.8f, 0.0f},  // Uracil
    {0.5f, 0.8f, 0.0f},  // Cytosine
    {1.0f, 0.6f, 0.0f}   // Guanine
};

const glm::vec3 PROTEIN_COLOR = {0.0f, 0.8f, 0.0f};
const glm::vec3 BOND_COLOR = {0.7f, 0.7f, 0.7f};
const float BOND_DISTANCE = 2.5f;
const float INTERACTION_RANGE = 5.0f;

const glm::vec3 SELECTED_COLOR = {1.0f, 1.0f, 0.0f};
const float MAX_TWIST = 0.5f;
const float RNA_WAVE_AMPLITUDE = 0.5f;
float dna_twist_factor = 1.0f;
float rna_wave_phase = 0.0f;
int selected_molecule = -1;
bool show_advanced_settings = false;
bool show_light_controls = false;
GLfloat global_ambient[4] = {0.2f, 0.2f, 0.2f, 1.0f};
GLfloat light_pos[4] = {5.0f, 10.0f, 5.0f, 1.0f};

void InitOpenGL();
void CreateMolecules();
void CreateDNA(Molecule& dna);
void CreateRNA(Molecule& rna);
void CreateProtein(Molecule& protein);
void DrawNucleotide(const Nucleotide& n);
void DrawBond(const glm::vec3& start, const glm::vec3& end);
void DrawMolecule(const Molecule& m);
void UpdatePhysics();
void UpdateInteractions();
void Hybridization();
void ProteinBinding();
void TranscriptionProcess();
void DrawInterface();
void DrawAdvancedControls();
void DrawLightControls();
void FocusCameraOnMolecule(int index);
void Display();
void Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Advanced Biomolecules Visualizer");

    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK) {
        fprintf(stderr, "GLEW initialization failed\n");
        return 1;
    }

    InitOpenGL();
    CreateMolecules();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGLUT_Init();
    ImGui_ImplOpenGL3_Init("#version 330");
    
    ImGui::StyleColorsDark();
    ImGui::GetStyle().WindowRounding = 5.0f;

    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutMouseFunc(Mouse);
    glutMotionFunc(Motion);
    glutIdleFunc([]() { glutPostRedisplay(); });

    glutMainLoop();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGLUT_Shutdown();
    ImGui::DestroyContext();

    return 0;
}

void InitOpenGL() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glShadeModel(GL_SMOOTH);

    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, global_ambient);

    GLfloat light_diffuse[] = {0.9f, 0.9f, 0.9f, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}
void CreateMolecules() {
    Molecule dna = {DNA, {}, {0.0f, 0.0f, -10.0f}};
    Molecule rna = {RNA, {}, {0.0f, 0.0f, 0.0f}};
    Molecule protein = {PROTEIN, {}, {0.0f, 0.0f, 10.0f}};

    CreateDNA(dna);
    CreateRNA(rna);
    CreateProtein(protein);

    molecules = {dna, rna, protein};
}

void CreateDNA(Molecule& dna) {
    const int steps = 50;
    const float radius = 2.0f;
    dna.nucleotides.clear();

    for(int i = 0; i < steps; ++i) {
        float angle = i * (2 * M_PI / steps);
        float twist = MAX_TWIST * dna_twist_factor;
        
        glm::vec3 pos1 = {
            radius * cos(angle + twist * i), 
            0.0f, 
            radius * sin(angle + twist * i)
        };
        
        glm::vec3 pos2 = {
            radius * cos(angle + M_PI + twist * i), 
            0.0f, 
            radius * sin(angle + M_PI + twist * i)
        };

        dna.nucleotides.push_back({pos1, DNA_COLORS[i%4], 0.4f, 0.0f});
        dna.nucleotides.push_back({pos2, DNA_COLORS[(i+2)%4], 0.4f, 0.0f});
    }
}

void CreateRNA(Molecule& rna) {
    const int steps = 30;
    const float radius = 1.5f;
    rna.nucleotides.clear();

    for(int i = 0; i < steps; ++i) {
        float angle = i * (2 * M_PI / steps);
        float wave = RNA_WAVE_AMPLITUDE * sin(rna_wave_phase + angle);
        
        rna.nucleotides.push_back({
            {radius * cos(angle), wave, radius * sin(angle)},
            RNA_COLORS[i%4], 0.3f, 0.0f
        });
    }
}

void CreateProtein(Molecule& protein) {
    const int layers = 5;
    const float radius = 2.5f;

    for(int l = 0; l < layers; ++l) {
        float y = l - layers/2.0f;
        int points = 8 + l*2;
        for(int i = 0; i < points; ++i) {
            float angle = i * (2 * M_PI / points);
            float scale = 1.0f - l*0.1f;
            protein.nucleotides.push_back({
                {radius * cos(angle) * scale, y, radius * sin(angle) * scale},
                glm::mix(PROTEIN_COLOR, {0.2f, 0.9f, 0.2f}, l/5.0f),
                0.35f, 0.0f
            });
        }
    }
}

void DrawNucleotide(const Nucleotide& n) {
    glPushMatrix();
    glTranslatef(n.position.x, n.position.y, n.position.z);
    glRotatef(n.rotation, 0.0f, 1.0f, 0.0f);
    
    GLfloat mat_ambient[] = {n.color.r*0.3f, n.color.g*0.3f, n.color.b*0.3f, 1.0f};
    GLfloat mat_diffuse[] = {n.color.r, n.color.g, n.color.b, 1.0f};
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    
    glutSolidSphere(n.radius, 32, 32);
    glPopMatrix();
}

void DrawBond(const glm::vec3& start, const glm::vec3& end) {
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glColor3fv(&BOND_COLOR[0]);
    glVertex3fv(&start[0]);
    glVertex3fv(&end[0]);
    glEnd();
}

void DrawMolecule(const Molecule& m) {
    glPushMatrix();
    glTranslatef(m.position.x, m.position.y, m.position.z);
    
    if(&m == &molecules[selected_molecule]) {
        GLfloat emission[4] = {1.0f, 1.0f, 0.0f, 1.0f};
        glMaterialfv(GL_FRONT, GL_EMISSION, emission);
    }
    
    if(show_bonds) {
        static std::vector<bool> connected;
        connected.resize(m.nucleotides.size(), false);
        
        for(size_t i = 0; i < m.nucleotides.size(); ++i) {
            if(!connected[i]) {
                float min_dist = INFINITY;
                size_t closest = i;
                
                for(size_t j = i+1; j < m.nucleotides.size(); ++j) {
                    float dist = glm::distance(m.nucleotides[i].position, 
                                             m.nucleotides[j].position);
                    if(dist < BOND_DISTANCE && dist < min_dist) {
                        min_dist = dist;
                        closest = j;
                    }
                }
                
                if(min_dist < BOND_DISTANCE) {
                    DrawBond(m.nucleotides[i].position, 
                            m.nucleotides[closest].position);
                    connected[i] = connected[closest] = true;
                }
            }
        }
    }
    
    for(const auto& n : m.nucleotides) {
        DrawNucleotide(n);
    }
    
    if(&m == &molecules[selected_molecule]) {
        GLfloat emission[4] = {0.0f, 0.0f, 0.0f, 1.0f};
        glMaterialfv(GL_FRONT, GL_EMISSION, emission);
    }
    
    glPopMatrix();
}

void UpdatePhysics() {
    static float rotation = 0.0f;
    rotation += 0.5f * simulation_speed;
    
    if(dna_twist_factor < 1.0f) {
        dna_twist_factor += 0.01f * simulation_speed;
        CreateDNA(molecules[0]);
    }
    
    rna_wave_phase += 0.02f * simulation_speed;
    CreateRNA(molecules[1]);

    for(auto& m : molecules) {
        for(auto& n : m.nucleotides) {
            n.rotation = rotation * 0.1f;
        }
    }
    
    if(current_mode != NONE) UpdateInteractions();
}

void UpdateInteractions() {
    switch(current_mode) {
        case DNA_RNA_HYBRID: Hybridization(); break;
        case PROTEIN_BINDING: ProteinBinding(); break;
        case TRANSCRIPTION: TranscriptionProcess(); break;
        default: break;
    }
}

void Hybridization() {
    auto& dna = molecules[0];
    auto& rna = molecules[1];

    for(auto& d : dna.nucleotides) {
        for(auto& r : rna.nucleotides) {
            float dist = glm::distance(d.position, r.position);
            if(dist < INTERACTION_RANGE) {
                d.color = glm::mix(d.color, RNA_COLORS[1], 0.05f);
                r.color = glm::mix(r.color, DNA_COLORS[0], 0.05f);
                
                if(dist < BOND_DISTANCE && !d.paired && !r.paired) {
                    DrawBond(d.position, r.position);
                    d.paired = true;
                    r.paired = true;
                }
            }
        }
    }
}

void ProteinBinding() {
    auto& dna = molecules[0];
    auto& protein = molecules[2];

    for(auto& p : protein.nucleotides) {
        glm::vec3 closest;
        float min_dist = INFINITY;
        
        for(auto& d : dna.nucleotides) {
            float dist = glm::distance(p.position, d.position);
            if(dist < min_dist) {
                min_dist = dist;
                closest = d.position;
            }
        }
        
        if(min_dist < INTERACTION_RANGE) {
            p.position = glm::mix(p.position, closest, 0.01f * simulation_speed);
        }
    }
}

void TranscriptionProcess() {
    static size_t transcription_pos = 0;
    auto& dna = molecules[0];
    auto& rna = molecules[1];
    
    if(transcription_pos < rna.nucleotides.size()) {
        auto& r = rna.nucleotides[transcription_pos];
        auto& d = dna.nucleotides[transcription_pos % dna.nucleotides.size()];
        
        r.position = glm::mix(r.position, d.position, 0.05f * simulation_speed);
        r.color = glm::mix(r.color, d.color, 0.1f);
        transcription_pos++;
    }
}

void DrawInterface() {
    ImGui::Begin("Control Panel", &show_controls, ImGuiWindowFlags_AlwaysAutoResize);
    
    ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "Molecular Interaction System");
    ImGui::Separator();
    
    ImGui::Checkbox("Auto Rotation", &auto_rotate);
    ImGui::SameLine();
    if(ImGui::Button("Pause All")) {
        simulation_speed = 0.0f;
    }
    ImGui::SameLine();
    if(ImGui::Button("Reset Speed")) {
        simulation_speed = 1.0f;
    }
    
    ImGui::SliderFloat("Global Speed", &simulation_speed, 0.0f, 5.0f);
    
    ImGui::Separator();
    ImGui::Text("Molecule Focus:");
    const char* items[] = {"DNA", "RNA", "Protein"};
    if(ImGui::Combo("Select", &selected_molecule, items, 3)) {
        FocusCameraOnMolecule(selected_molecule);
    }
    
    ImGui::Separator();
    if(ImGui::CollapsingHeader("Advanced Settings")) {
        DrawAdvancedControls();
    }
    
    if(ImGui::CollapsingHeader("Light Settings")) {
        DrawLightControls();
    }
    
    ImGui::Separator();
    if(ImGui::Button("Reset System")) {
        CreateMolecules();
        current_mode = NONE;
        selected_molecule = -1;
    }
    
    ImGui::End();
}

void DrawAdvancedControls() {
    ImGui::Checkbox("Show Bonds", &show_bonds);
    ImGui::SameLine();
    ImGui::Checkbox("Show Interactions", &show_interactions);
    
    ImGui::Separator();
    ImGui::Text("DNA Parameters:");
    if(ImGui::SliderFloat("Twist Factor", &dna_twist_factor, 0.1f, 2.0f)) {
        CreateDNA(molecules[0]);
    }
    
    ImGui::Separator();
    ImGui::Text("RNA Parameters:");
    ImGui::SliderFloat("Wave Phase", &rna_wave_phase, 0.0f, 2*M_PI);
    
    ImGui::Separator();
    ImGui::Text("Interaction Modes:");
    ImGui::RadioButton("None", (int*)&current_mode, NONE); ImGui::SameLine();
    ImGui::RadioButton("DNA-RNA", (int*)&current_mode, DNA_RNA_HYBRID); ImGui::SameLine();
    ImGui::RadioButton("Protein", (int*)&current_mode, PROTEIN_BINDING);
    ImGui::RadioButton("Transcription", (int*)&current_mode, TRANSCRIPTION);
}

void DrawLightControls() {
    ImGui::ColorEdit3("Ambient Light", global_ambient);
    ImGui::SliderFloat3("Light Position", light_pos, -20.0f, 20.0f);
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, global_ambient);
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
}

void FocusCameraOnMolecule(int index) {
    if(index >= 0 && index < 3) {
        const auto& pos = molecules[index].position;
        camera_angle_x = atan2f(pos.z, pos.x);
        camera_angle_y = asinf(pos.y / camera_distance);
    }
}

void Display() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGLUT_NewFrame();
    ImGui::NewFrame();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)WINDOW_WIDTH/WINDOW_HEIGHT, 0.1, 100.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float eyeX = camera_distance * sin(camera_angle_x) * cos(camera_angle_y);
    float eyeY = camera_distance * sin(camera_angle_y);
    float eyeZ = camera_distance * cos(camera_angle_x) * cos(camera_angle_y);
    
    gluLookAt(eyeX, eyeY, eyeZ, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

    UpdatePhysics();
    for(const auto& m : molecules) DrawMolecule(m);

    DrawInterface();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    glutSwapBuffers();
    if(auto_rotate) camera_angle_x += 0.003f;
}

void Reshape(int w, int h) {
    glViewport(0, 0, w, h);
    ImGui::GetIO().DisplaySize = ImVec2((float)w, (float)h);
}

void Keyboard(unsigned char key, int x, int y) {
    ImGui_ImplGLUT_KeyboardFunc(key, x, y);
    if(key == 27) exit(0);
}

void Mouse(int button, int state, int x, int y) {
    ImGui_ImplGLUT_MouseFunc(button, state, x, y);
    
    if(!ImGui::GetIO().WantCaptureMouse && button == GLUT_LEFT_BUTTON) {
        mouse_pressed = (state == GLUT_DOWN);
        last_mouse_x = x;
        last_mouse_y = y;
    }
}

void Motion(int x, int y) {
    ImGui_ImplGLUT_MotionFunc(x, y);
    
    if(mouse_pressed && !ImGui::GetIO().WantCaptureMouse) {
        camera_angle_x += (x - last_mouse_x) * 0.01f;
        camera_angle_y = glm::clamp(camera_angle_y + (y - last_mouse_y) * 0.01f, -1.5f, 1.5f);
        last_mouse_x = x;
        last_mouse_y = y;
    }
}
