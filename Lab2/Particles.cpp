#include <GL/glew.h>
#include <GL/glut.h>
#include <vector>
#include <cstdlib>
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

using namespace std;

// Enum for different states of a particle in the fireworks
enum ParticleState {
    TRAIL,
    EXPLOSION,
    DEAD
};

// Global variables
GLuint g_textureID;                   // Texture ID for particle texture
bool currentFireworkExploded = false; // Flag to check if the current firework has exploded
const float GRAVITY = -0.0005f;       // Gravity constant for particle movement
const float WIND = 0.0005f;           // Wind effect constant for particle movement
float explosionX = 0.0f;              // X-coordinate for explosion location
float explosionY = 0.0f;              // Y-coordinate for explosion location

// Particle class representing each particle in the fireworks
class Particle {
public:
    // Properties of a particle
    float x, y, z;         // Position
    float vx, vy, vz;      // Velocity
    float life;            // Life span of the particle
    float targetY;         // Target Y-coordinate for the particle
    float alpha;           // Alpha value for transparency
    float size;            // Size of the particle
    ParticleState state;   // Current state of the particle (TRAIL, EXPLOSION, DEAD)
    GLuint textureID;      // Texture ID associated with the particle
    bool colorful;         // Flag to indicate if the particle is colorful
    float r, g, b;         // Color components for the particle

    // Constructor for Particle class
    Particle(float startX, float startY, ParticleState initialState, bool _colorful, float _r, float _g, float _b)
        : state(initialState), textureID(g_textureID), x(startX), y(startY), z(0.0f), colorful(_colorful), r(_r), g(_g), b(_b) {
        // Initialize properties based on the state of the particle
        if (state == TRAIL) {
            // Initial velocity for trailing particles
            vx = (rand() % 5 - 2) / 1000.0f;
            vy = 0.03f + (rand() % 5) / 1000.0f;
            vz = 0.0f;
            life = 2.0f; // Lifespan for trailing particles
            targetY = startY + 0.25f + (rand() % 25) / 40.0f; // Target position for trailing particles
        }
        else {
            // Randomized velocity for explosion particles
            float angle = (rand() % 360) * (3.14159 / 180.0f);
            float verticalAngle = (rand() % 180) * (3.14159 / 180.0f);
            float speedVariation = ((rand() % 30) - 15) / 100.0f;
            float speed = ((rand() % 3) + 2 + speedVariation) / 100.0f;
            vx = speed * sin(verticalAngle) * cos(angle);
            vy = speed * cos(verticalAngle);
            vz = speed * sin(verticalAngle) * sin(angle);
            alpha = 1.0;
            life = 0.5f + ((rand() % 50) - 25) / 100.0f; // Lifespan for explosion particles
        }

        alpha = 1.0f; // Initial alpha for particles
        size = ((rand() % 10) + 1) / 300.0f; // Randomized size

        // If the particle is colorful, assign a random color
        if (colorful) {
            int colorChoice = rand() % 3;
            switch (colorChoice) {
            case 0: r = 1; g = 0.2; b = 0.2; break;
            case 1: r = 1; g = 1; b = 0.2; break;
            case 2: r = 0.2; g = 0.2; b = 1; break;
            }
        }
    }

    // Update function to change particle properties over time
    void update(vector<Particle>& particles) {
        x += vx; // Update position based on velocity
        y += vy;
        z += vz;

        // Apply gravity if the particle is in the EXPLOSION state
        if (state == EXPLOSION) {
            vy += GRAVITY;
        }

        // Apply wind effect
        x += WIND;

        // Update the state of the particle
        if (state == TRAIL && y >= targetY) {
            state = DEAD; // Kill the particle if it reaches its target
            currentFireworkExploded = true; // Trigger explosion
            explosionX = x; // Set explosion coordinates
            explosionY = y;
        }
        else if (state == EXPLOSION) {
            life -= 0.005f; // Decrease life span
            alpha -= 0.03f + (rand() % 2 + 1) / 100; // Fade out
            size -= 0.0005f; // Decrease size
            if (size < 0) size = 0;
            if (life <= 0.0f) {
                state = DEAD; // Kill the particle if its life ends
            }
        }
    }

    // Function to check if the particle is dead
    bool isDead() const {
        return state == DEAD;
    }

    // Draw function to render the particle using OpenGL
    void draw() const {
        glColor4f(r, g, b, alpha); // Set color with alpha
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);
        float halfSize = size / 2.0f; // Calculate half size for rendering

        // Render a textured quad for the particle
        glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0); glVertex3f(x - halfSize, y - halfSize, z + halfSize);
        glTexCoord2f(1.0, 0.0); glVertex3f(x + halfSize, y - halfSize, z + halfSize);
        glTexCoord2f(1.0, 1.0); glVertex3f(x + halfSize, y + halfSize, z - halfSize);
        glTexCoord2f(0.0, 1.0); glVertex3f(x - halfSize, y + halfSize, z - halfSize);
        glEnd();
        glDisable(GL_TEXTURE_2D);
    }
};

// Firework class representing a collection of particles
class Firework {
public:
    vector<Particle> particles; // Container for particles
    float startX;               // Starting X-coordinate for the firework
    bool colorful;              // Flag to indicate if the firework is colorful
    float r, g, b;              // Color components for the entire firework

    // Constructor for the Firework class
    Firework(float _startX, bool _colorful) : startX(_startX), colorful(_colorful) {
        // Set the color of the firework based on the color flag
        if (!colorful) {
            int colorChoice = rand() % 3;
            switch (colorChoice) {
            case 0: r = 0.5; g = 0; b = 0.5; break;  // Purple
            case 1: r = 0; g = 1; b = 0; break;     // Green
            case 2: r = 1; g = 1; b = 0; break;     // Yellow
            }
        }
        generateParticles(); // Generate the particles for the firework
    }

    // Function to generate particles for the firework
    void generateParticles() {
        int numParticles = 150 + rand() % 300; // Random number of particles
        for (int i = 0; i < numParticles; i++) {
            particles.emplace_back(startX, 0.0f, TRAIL, colorful, r, g, b); // Create trailing particles
        }
    }

    // Update function for the firework
    void update() {
        for (Particle& particle : particles) {
            particle.update(particles); // Update each particle
        }
        // Remove dead particles
        particles.erase(remove_if(particles.begin(), particles.end(), [](const Particle& p) { return p.isDead(); }), particles.end());

        // Create explosion particles if the firework has exploded
        if (currentFireworkExploded) {
            for (int i = 0; i < 100; i++) {
                particles.emplace_back(explosionX, explosionY, EXPLOSION, colorful, r, g, b); // Create explosion particles
            }
            currentFireworkExploded = false; // Reset the explosion flag
        }
    }

    // Draw function for the firework
    void draw() {
        for (Particle& particle : particles) {
            particle.draw(); // Draw each particle
        }
    }
};

vector<Firework> fireworks; // Global container for all fireworks

// Function to generate a new firework
void generateFirework() {
    float startX = (rand() % 100) / 100.0f; // Random starting X-coordinate for the firework
    bool colorful = rand() % 2; // Randomly decide if the firework is colorful
    fireworks.emplace_back(startX, colorful); // Create and add the firework to the global container
}

// Update function for all the fireworks
void updateFireworks() {
    for (Firework& firework : fireworks) {
        firework.update(); // Update each firework
    }
    // Remove fireworks that have no particles left
    fireworks.erase(remove_if(fireworks.begin(), fireworks.end(), [](const Firework& f) { return f.particles.empty(); }), fireworks.end());

    // Periodically generate new fireworks
    static int frameCounter = 0;
    const int fireworkGenerationInterval = 200; // Fixed interval for the next firework
    if (++frameCounter >= fireworkGenerationInterval) {
        generateFirework(); // Generate a new firework
        frameCounter = 0; // Reset the frame counter
    }
}

// Display function called by GLUT for rendering
void display() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Set background color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the color and depth buffers
    glLoadIdentity(); // Reset the modelview matrix

    // Set up the camera
    gluLookAt(1, 1, 2.0, 0.5, 0.5, 0.0, 0.0, 1.0, 0.0);

    // Update and draw all fireworks
    updateFireworks();
    for (Firework& firework : fireworks) {
        firework.draw(); // Draw each firework
    }

    // Swap the front and back buffers
    glutSwapBuffers();
}

// Timer function for GLUT to control the rendering speed
void timer(int value) {
    glutPostRedisplay(); // Post a re-paint request to update the window
    glutTimerFunc(20, timer, 0); // Set the timer to call this function again in 20 milliseconds
}

// Initialization function
void init() {
    ilInit(); // Initialize DevIL
    iluInit(); // Initialize DevIL Utilities
    ilutRenderer(ILUT_OPENGL); // Set the renderer to be used by ILUT with OpenGL

    // Enable depth test for 3D rendering
    glEnable(GL_DEPTH_TEST);

    // Set up alpha blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    // Set the shading model
    glShadeModel(GL_FLAT);

    // Set up the projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.0, 0.1, 10.0);

    // Set up the modelview matrix
    glMatrixMode(GL_MODELVIEW);

    // Load the texture for the particles
    const wchar_t* constString = L"C:/Users/doric/Desktop/RACAN/RA-labosi/RA-labosi/Primjeri/Lab2/explosion.bmp";
    wchar_t* nonConstString = const_cast<wchar_t*>(constString);
    g_textureID = ilutGLLoadImage(nonConstString);
    if (!g_textureID) {
        exit(1); // Exit if the texture loading fails
    }

    // Generate the first firework
    generateFirework();
}

// Main function
int main(int argc, char** argv) {
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE); // Enable double buffering
    glutInitWindowSize(800, 800); // Set window size
    glutInitWindowPosition(100, 100); // Set window position
    glutCreateWindow("Particles"); // Create window with title

    // Register callbacks
    glutDisplayFunc(display); // Display callback
    glutTimerFunc(0, timer, 0); // Timer callback

    // Initialize GLEW
    glewInit();

    // Initialize the program
    init();

    // Start the GLUT main loop
    glutMainLoop();

    return 0;
}
