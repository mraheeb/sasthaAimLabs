#include <GL/glut.h>
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>

// Constants
const float ballRadius = 0.05f;
const int maxTime = 20 * 1000; // 20 seconds in milliseconds

// Variables
struct Ball {
    float x, y;
    bool isHit;
    float r, g, b; // Color components
};

Ball currentBall;
int score = 0;
int screenWidth, screenHeight;
int windowWidth = 800, windowHeight = 600; // Initial window size
int windowPosX = 100, windowPosY = 100; // Initial window position
bool isFullscreen = false;
int startTime; // Timer start time
bool gameActive = true; // Whether the game is currently active

// Function declarations
void display();
void reshape(int w, int h);
void mouseClick(int button, int state, int x, int y);
void keyboard(unsigned char key, int x, int y);
void drawBall();
void generateBall();
bool checkCollision(float mouseX, float mouseY, Ball &ball);
void drawShadedBall(float x, float y, float radius, float r, float g, float b);
void toggleFullscreen();
void updateTimer(int value); // Timer callback function
void handleClickToContinue(float mouseX, float mouseY);
void resetGame(); // Reset game state

// Main function
int main(int argc, char** argv) {
    srand(time(0)); // Initialize random seed

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(windowPosX, windowPosY);
    glutCreateWindow("Ball Clicker Game");

    // Get screen resolution
    screenWidth = glutGet(GLUT_SCREEN_WIDTH);
    screenHeight = glutGet(GLUT_SCREEN_HEIGHT);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouseClick);
    glutKeyboardFunc(keyboard);

    resetGame(); // Initialize game state

    glutMainLoop();
    return 0;
}

// Display function
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    if (gameActive) {
        drawBall();

        // Display score
        glColor3f(1.0f, 1.0f, 1.0f); // Set color to white
        glRasterPos2f(-0.95f, 0.9f);
        std::string scoreText = "Score: " + std::to_string(score);
        for (char c : scoreText) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }

        // Display time remaining
        int elapsedTime = glutGet(GLUT_ELAPSED_TIME) - startTime;
        int remainingTime = maxTime - elapsedTime;
        if (remainingTime < 0) remainingTime = 0;
        glRasterPos2f(-0.95f, 0.8f);
        std::string timeText = "Time: " + std::to_string(remainingTime / 1000) + "s";
        for (char c : timeText) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }
    } else {
        // Display game over message
        glColor3f(1.0f, 1.0f, 1.0f); // Set color to white
        glRasterPos2f(-0.1f, 0.1f);
        std::string gameOverText = "Game Over! Score: " + std::to_string(score);
        for (char c : gameOverText) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }

        // Display click to continue message
        glRasterPos2f(-0.2f, 0.0f);
        std::string continueText = "CLICK TO CONTINUE";
        for (char c : continueText) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }
    }

    glutSwapBuffers();
}

// Reshape function
void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

// Mouse click function
void mouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        float mouseX = (float(x) / glutGet(GLUT_WINDOW_WIDTH)) * 2.0f - 1.0f;
        float mouseY = 1.0f - (float(y) / glutGet(GLUT_WINDOW_HEIGHT)) * 2.0f;

        if (gameActive) {
            if (checkCollision(mouseX, mouseY, currentBall)) {
                score++;
                generateBall(); // Generate a new ball when the current one is hit
                glutPostRedisplay();
            }
        } else {
            // Handle click to continue
            handleClickToContinue(mouseX, mouseY);
        }
    }
}

// Keyboard function
void keyboard(unsigned char key, int x, int y) {
    if (key == 'f' || key == 'F') {
        toggleFullscreen();
    }
}

// Draw ball function
void drawBall() {
    if (!currentBall.isHit) {
        drawShadedBall(currentBall.x, currentBall.y, ballRadius, currentBall.r, currentBall.g, currentBall.b);
    }
}

// Draw shaded ball function
void drawShadedBall(float x, float y, float radius, float r, float g, float b) {
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(r + 0.3f, g + 0.3f, b + 0.3f); // Center color (lighter)
    glVertex2f(x, y);

    for (int i = 0; i <= 360; i += 1) {
        float theta = i * M_PI / 180.0f;
        float dx = radius * cos(theta);
        float dy = radius * sin(theta);

        float shade = 0.5f * (cos(theta) + 1.0f);
        glColor3f(r * shade, g * shade, b * shade); // Edge color (darker)
        glVertex2f(x + dx, y + dy);
    }
    glEnd();
}

// Generate new ball function
void generateBall() {
    currentBall.x = (rand() % 200 - 100) / 100.0f;
    currentBall.y = (rand() % 200 - 100) / 100.0f;
    currentBall.isHit = false;

    // Generate random color
    currentBall.r = static_cast<float>(rand()) / RAND_MAX;
    currentBall.g = static_cast<float>(rand()) / RAND_MAX;
    currentBall.b = static_cast<float>(rand()) / RAND_MAX;
}

// Check collision function
bool checkCollision(float mouseX, float mouseY, Ball &ball) {
    return std::sqrt((mouseX - ball.x) * (mouseX - ball.x) + (mouseY - ball.y) * (mouseY - ball.y)) < ballRadius;
}

// Toggle fullscreen function
void toggleFullscreen() {
    if (isFullscreen) {
        glutReshapeWindow(windowWidth, windowHeight);
        glutPositionWindow(windowPosX, windowPosY);
    } else {
        windowWidth = glutGet(GLUT_WINDOW_WIDTH);
        windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
        windowPosX = glutGet(GLUT_WINDOW_X);
        windowPosY = glutGet(GLUT_WINDOW_Y);
        glutFullScreen();
    }
    isFullscreen = !isFullscreen;
}

// Timer callback function
void updateTimer(int value) {
    if (gameActive) {
        // Check if time is up
        int elapsedTime = glutGet(GLUT_ELAPSED_TIME) - startTime;
        if (elapsedTime >= maxTime) {
            gameActive = false;
        }

        // Redraw the display
        glutPostRedisplay();

        // Call this function again after 100 milliseconds
        glutTimerFunc(100, updateTimer, 0);
    }
}

// Handle click to continue function
void handleClickToContinue(float mouseX, float mouseY) {
    // If the game is over, clicking anywhere will restart the game
    resetGame();
}

// Reset game state function
void resetGame() {
    gameActive = true;
    score = 0;
    startTime = glutGet(GLUT_ELAPSED_TIME);
    generateBall(); // Generate a new ball
    glutPostRedisplay();

    // Restart the timer
    updateTimer(0);
}
