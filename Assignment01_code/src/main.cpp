#include "utils.h"
#include <math.h>

#define DRAW_CUBIC_BEZIER 1 // Use to switch Linear and Cubic bezier curves
#define SAMPLES_PER_BEZIER 100 //Sample each Bezier curve as N=10 segments and draw as connected lines

// GLobal variables
std::vector<float> controlPoints;
std::vector<float> linearBezier;
std::vector<float> cubicBezier;

int numPoints = 0;

int width = 640, height = 640; 
bool controlPointsUpdated = false;

void calculatePiecewiseLinearBezier()
{
    // Since linearBezier is just a polyline, we can just copy the control points and plot.
    // However to show how a piecewise parametric curve needs to be plotted, we sample t and 
    // evaluate all linear bezier curves.
    // linearBezier.assign(controlPoints.begin(), controlPoints.end());

    linearBezier.clear();
    int sz  = controlPoints.size(); // Contains 3 points/vertex. Ignore Z
    if(sz==0){
    	numPoints = 0;
    }
    float x[2], y[2];
    float delta_t = 1.0/(SAMPLES_PER_BEZIER - 1.0);
    float t;
    for(int i=0; i<(sz-3); i+=3) {
        x[0] = controlPoints[i];
        y[0] = controlPoints[i+1];
        x[1] = controlPoints[i+3];
        y[1] = controlPoints[i+4];
        linearBezier.push_back(x[0]);
        linearBezier.push_back(y[0]);
        linearBezier.push_back(0.0);
        t = 0.0;
        for (float j=1; j<(SAMPLES_PER_BEZIER-1); j++)
        {
	        t += delta_t;
	        linearBezier.push_back(x[0] + t*(x[1] - x[0]));
	        linearBezier.push_back(y[0] + t*(y[1] - y[0]));
	        linearBezier.push_back(0.0);
        }
        // No need to add the last point for this segment, since it will be added as first point in next.
    }
    // However, add last point of entire piecewise curve here (i.e, the last control point)
    linearBezier.push_back(x[1]);
    linearBezier.push_back(y[1]);
    linearBezier.push_back(0.0);
}

void calculatePiecewiseCubicBezier()
{
    cubicBezier.clear();
    int sz  = controlPoints.size(); // Contains 3 points/vertex. Ignore Z
    if(sz==0){
    	numPoints = 0;
    }
    float x[4], y[4];
    float delta_t = 1.0/(SAMPLES_PER_BEZIER - 1.0);
    float t;
    
    //Getting 4 points for a piecewise bezier curve

    for(int i=0; i<(sz-9); i+=9) {

        x[0] = controlPoints[i];
        y[0] = controlPoints[i+1];
        x[1] = controlPoints[i+3];
        y[1] = controlPoints[i+4];
        x[2] = controlPoints[i+6];
        y[2] = controlPoints[i+7];
        x[3] = controlPoints[i+9];
        y[3] = controlPoints[i+10];
        
        cubicBezier.push_back(x[0]);
        cubicBezier.push_back(y[0]);
        cubicBezier.push_back(0.0);

        //Initializing the 3 linear bezier curves
        
        float linearBezier1X[SAMPLES_PER_BEZIER];
		float linearBezier2X[SAMPLES_PER_BEZIER];
        float linearBezier3X[SAMPLES_PER_BEZIER];

        float linearBezier1Y[SAMPLES_PER_BEZIER];
		float linearBezier2Y[SAMPLES_PER_BEZIER];
        float linearBezier3Y[SAMPLES_PER_BEZIER];


        t = 0.0;

        //Making linear curve for 1st and 2nd point
	    for (int j=0; j<(SAMPLES_PER_BEZIER); j++){
	        t += delta_t;
	        linearBezier1X[j] = x[0] + t*(x[1] - x[0]);
	        linearBezier1Y[j] = y[0] + t*(y[1] - y[0]);
	    }

	    t = 0.0;

	    //Making linear curve for 2nd and 3rd point
		for (int j=0; j<(SAMPLES_PER_BEZIER); j++){
	        t += delta_t;
	        linearBezier2X[j] = x[1] + t*(x[2] - x[1]);
	        linearBezier2Y[j] = y[1] + t*(y[2] - y[1]);
	    }

	    t = 0.0;

	    //Making linear curve for 3rd and 4th point
	    for (int j=0; j<(SAMPLES_PER_BEZIER); j++){
	        t += delta_t;
	        linearBezier3X[j] = x[2] + t*(x[3] - x[2]);
	        linearBezier3Y[j] = y[2] + t*(y[3] - y[2]);
	    }

	    //Initializing points for 2 quadratic bezier curves

	    float quadraticBezier1X[SAMPLES_PER_BEZIER-2];
		float quadraticBezier2X[SAMPLES_PER_BEZIER-2];

		float quadraticBezier1Y[SAMPLES_PER_BEZIER-2];
		float quadraticBezier2Y[SAMPLES_PER_BEZIER-2];

		t = 0.0;


        //Making quadratic curve from 1st and 2nd linear curve and then making cubic from the obtained curves
	    for (int j=1; j<(SAMPLES_PER_BEZIER-1); j++){
	        //t += delta_t;
	        quadraticBezier1X[j] = linearBezier1X[j] + delta_t*j*(linearBezier2X[j] - linearBezier1X[j]);
	        quadraticBezier1Y[j] = linearBezier1Y[j] + delta_t*j*(linearBezier2Y[j] - linearBezier1Y[j]);
	        quadraticBezier2X[j] = linearBezier2X[j] + delta_t*j*(linearBezier3X[j] - linearBezier2X[j]);
	        quadraticBezier2Y[j] = linearBezier2Y[j] + delta_t*j*(linearBezier3Y[j] - linearBezier2Y[j]);

	        cubicBezier.push_back(quadraticBezier1X[j] + delta_t*j*(quadraticBezier2X[j] - quadraticBezier1X[j]));
	        cubicBezier.push_back(quadraticBezier1Y[j] + delta_t*j*(quadraticBezier2Y[j] - quadraticBezier1Y[j]));
	        cubicBezier.push_back(0.0);
	    }
    


        // No need to add the last point for this segment, since it will be added as first point in next.
    }

    // However, add last point of entire piecewise curve here (i.e, the last control point)
    cubicBezier.push_back(x[3]);
    cubicBezier.push_back(y[3]);
    cubicBezier.push_back(0.0);


}

float rescaleX(int x){
	float rescaled_x = -1.0 + ((1.0*x - 0) / (width - 0)) * (1.0 - (-1.0));
	return rescaled_x;
}


float rescaleY(int y){
    float rescaled_y = -1.0 + ((1.0*(height - y) - 0) / (height - 0)) * (1.0 - (-1.0));
	return rescaled_y;
}


int main(int, char* argv[])

{	
	
	int prev_x;
	int prev_y;

    GLFWwindow* window = setupWindow(width, height);
    ImGuiIO& io = ImGui::GetIO(); // Create IO object

    ImVec4 clear_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    unsigned int shaderProgram = createProgram("./shaders/vshader.vs", "./shaders/fshader.fs");
	glUseProgram(shaderProgram);

    // Create VBOs, VAOs
    unsigned int VBO_controlPoints, VBO_linearBezier, VBO_cubicBezier;
    unsigned int VAO_controlPoints, VAO_linearBezier, VAO_cubicBezier;
    glGenBuffers(1, &VBO_controlPoints);
    glGenVertexArrays(1, &VAO_controlPoints);
    glGenBuffers(1, &VBO_linearBezier);
    glGenVertexArrays(1, &VAO_linearBezier);
    glGenBuffers(1, &VBO_cubicBezier);
    glGenVertexArrays(1, &VAO_cubicBezier);
    
    //TODO:

    int button_status = 0;
    bool point_picked = false;
    int pointInd = -1;

    //Display loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Rendering
        showOptionsDialog(controlPoints, io);
        ImGui::Render();

        // Add a new point on mouse click
        float x,y ;
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        if(io.MouseClicked[0] && !ImGui::IsAnyItemActive()){
            x = io.MousePos.x;
            y = io.MousePos.y;
            int size = controlPoints.size();
            bool check = true;

            float rescaled_x = rescaleX(x);
            float rescaled_y = rescaleY(y);

            if(point_picked){
                controlPoints[pointInd] = rescaled_x;
                controlPoints[pointInd+1] = rescaled_y;
                point_picked = false;
                controlPointsUpdated = true;
            }

            else{

                for(int i = 0; i<size; i=i+3){
                	if(fabs(rescaled_x-controlPoints[i])<=0.01){
                		if(fabs(rescaled_y-controlPoints[i+1])<=0.01){
                			//std::cout<<"Point already clicked " << fabs(rescaled_x - controlPoints[i]) << " " << fabs(rescaled_y - controlPoints[i+1]) << "\n";
                			check = false;
                            point_picked = true;
                            pointInd = i;
                            std::cout<<"point picked"<<std::endl;
                		}
                	}
                }

                if(check){
                	addPoints(controlPoints, x, y, width, height);
                	numPoints++;
                
    	            if(numPoints==4){
    	            	addPoints(controlPoints, x + x - prev_x, y + y - prev_y, width, height);
    	            	numPoints = 2;
    	            }

    	            //std::cout<<"numPoints is "<<numPoints<<"\n";

    	            prev_x = x;
    	            prev_y = y;
    	            controlPointsUpdated = true;
    	        }
            }
	            
        }

        if(controlPointsUpdated) {
            // Update VAO/VBO for control points (since we added a new point)
            glBindVertexArray(VAO_controlPoints);
            glBindBuffer(GL_ARRAY_BUFFER, VBO_controlPoints);
            glBufferData(GL_ARRAY_BUFFER, controlPoints.size()*sizeof(GLfloat), &controlPoints[0], GL_DYNAMIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0); //Enable first attribute buffer (vertices)

            // Update VAO/VBO for piecewise linear Bezier curve (since we added a new point)
            calculatePiecewiseLinearBezier();
            glBindVertexArray(VAO_linearBezier);
            glBindBuffer(GL_ARRAY_BUFFER, VBO_linearBezier);
            glBufferData(GL_ARRAY_BUFFER, linearBezier.size()*sizeof(GLfloat), &linearBezier[0], GL_DYNAMIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0); //Enable first attribute buffer (vertices)

            // Update VAO/VBO for piecewise cubic Bezier curve
            calculatePiecewiseCubicBezier();
            glBindVertexArray(VAO_cubicBezier);
            glBindBuffer(GL_ARRAY_BUFFER, VBO_cubicBezier);
            glBufferData(GL_ARRAY_BUFFER, cubicBezier.size()*sizeof(GLfloat), &cubicBezier[0], GL_DYNAMIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0); //Enable first attribute buffer (vertices)
            // TODO:
        
            controlPointsUpdated = false; // Finish all VAO/VBO updates before setting this to false.
        }

        glUseProgram(shaderProgram);

        // Draw control points
        glBindVertexArray(VAO_controlPoints);
		glDrawArrays(GL_POINTS, 0, controlPoints.size()/3); // Draw points

#if DRAW_CUBIC_BEZIER
        glBindVertexArray(VAO_cubicBezier);
        glDrawArrays(GL_LINE_STRIP, 0, cubicBezier.size()/3); //Draw lines

#else
        // Draw linear Bezier
        glBindVertexArray(VAO_linearBezier);
        glDrawArrays(GL_LINE_STRIP, 0, linearBezier.size()/3); //Draw lines
#endif

        glUseProgram(0);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);

    }

    // Delete VBO buffers
    glDeleteBuffers(1, &VBO_controlPoints);
    glDeleteBuffers(1, &VBO_linearBezier);
    glDeleteBuffers(1, &VBO_cubicBezier);
    numPoints = 0;
    
    //TODO:

    // Cleanup
    cleanup(window);
    return 0;
}
