#include "glad/glad.h"
#include "SDL.h"
#include <iostream>
#include "vao.h"
#include "vbo.h"
#include "ebo.h"
#include "shaderProgram.h"
#include "mat4.h"
#include "vec3.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include "cube.h"
#include "camera.h"
#include "input.h"
#include <map>

#include "btBulletDynamicsCommon.h"

extern std::map<SDL_Keycode, bool> keyPressedMap;
extern mouse_click_state_t mouse_click_state;
extern mouse_state_t mouse_state;

// TODO: create func to go from cube world coords to screen coords,
// keep track of z values, and use this info to see what hit
// * might have to learn collision detection

// motionstates are the way bullet physics tells you the world transform of the objects

int width = 800, height = 800;

int main(int argc, char* args[]) {

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		std::cout << "sdl gave error" << std::endl;
		return -1;
	}

	std::cout << "sdl works" << std::endl;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);


	SDL_Window* window = SDL_CreateWindow("window",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		width,
		height,
		SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

	SDL_GLContext context = SDL_GL_CreateContext(window);

	SDL_GL_MakeCurrent(window, context);
	gladLoadGLLoader(SDL_GL_GetProcAddress);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();

	ImFont* robotoFont = io.Fonts->AddFontFromFileTTF("ext\\imgui\\fonts\\Roboto-Medium.ttf", 16.0f);

	ImGui_ImplSDL2_InitForOpenGL(window, context);
	const char* glsl_version = "#version 330";
	ImGui_ImplOpenGL3_Init(glsl_version);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glDepthFunc(GL_LESS);
	// glEnable(GL_CULL_FACE);
	// glCullFace(GL_BACK);
	// glFrontFace(GL_CCW);

	bool running = true;

    vec3 clearColor(0.0f, 0.0f, 0.0f);

    /*
	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
    */

	Cube cube;

    /*
	const char* vertexFilePath = "C:\\Sarthak\\voxel_editor\\VoxelEditor\\vertexShader.vert";
	const char* fragmentFilePath = "C:\\Sarthak\\voxel_editor\\VoxelEditor\\fragmentShader.frag";
	ShaderProgram shaderProgram(vertexFilePath, fragmentFilePath);

	shaderProgram.setFloat("windowHeight", (float)height);

	const char* outlineVert = "C:\\Sarthak\\voxel_editor\\VoxelEditor\\outline.vert";
	const char* outlineFrag = "C:\\Sarthak\\voxel_editor\\VoxelEditor\\outline.frag";
	ShaderProgram outlineProgram(outlineVert, outlineFrag);

	vec3 pos;
	vec3 scale(1.0f, 1.0f, 1.0f);
	vec3 outlineScale(1.05f, 1.05f, 1.05f);
	vec3 rot;
    */

	float posDelta = 0.01f;
	float scaleDelta = 0.1f;
	float rotDelta = 1.0f;

	uint32_t start = SDL_GetTicks();

	bool checked = false;
	bool show_demo_window = true;

	float val = 0.0f;

	// vec3 color(1.0f, 0.0f, 1.0f);

	mat4 projection = getProjectionMat(45.0f, 0.1f, 100.0f, ((float)width) / height);

	Camera cam(0.0f, 0.0f, 5.0f);

	int stencilBits;
	glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_STENCIL, GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE, &stencilBits);
	std::cout << "stencilBits: " << stencilBits << std::endl;

	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	// bool outline = false;

	while (running) {

		uint32_t cur = SDL_GetTicks();
		uint32_t diff = cur - start;
		start = cur;

		SDL_Event event;

		mat4 view = cam.getViewMat();

		handle_input(event);

		if (keyPressedMap[SDLK_o]) {
			cube.outline = !cube.outline;
		}

		if (keyPressedMap[SDL_QUIT] || keyPressedMap[SDLK_ESCAPE]) {
			running = false;
		}

		if (mouse_click_state.left) {
			std::cout << mouse_state.x << ", " << mouse_state.y << std::endl;
		}
		if (mouse_click_state.right) {
			std::cout << "right clicked" << std::endl;
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		glClearColor(clearColor.coords.x, clearColor.coords.y, clearColor.coords.z, 1.0f);
		glClearStencil(0);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);

        cube.render(projection, view);

        /*
		glStencilFunc(GL_ALWAYS, 1, 0xFF);

		{
			shaderProgram.bind();

			mat4 translationMat = getTranslationMatrix(pos.coords.x, pos.coords.y, pos.coords.z);
			shaderProgram.setMat4("translate", GL_TRUE, mat4_get_ptr(translationMat));

			mat4 rotMat = getRotMatrix(rot.coords.x, rot.coords.y, rot.coords.z);
			shaderProgram.setMat4("rot", GL_TRUE, mat4_get_ptr(rotMat));

			mat4 scaleMat = getScaleMatrix(scale.coords.x, scale.coords.y, scale.coords.z);
			shaderProgram.setMat4("scale", GL_TRUE, mat4_get_ptr(scaleMat));

			shaderProgram.setMat4("projection", GL_TRUE, mat4_get_ptr(projection));

			shaderProgram.setMat4("view", GL_TRUE, mat4_get_ptr(view));
			shaderProgram.setVec3("inColor", vec3_get_ptr(color));

			cube.render();
			shaderProgram.unbind();

		}

		if (outline) {
			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			{
				outlineProgram.bind();

				mat4 translationMat = getTranslationMatrix(pos.coords.x, pos.coords.y, pos.coords.z);
				outlineProgram.setMat4("translate", GL_TRUE, mat4_get_ptr(translationMat));

				mat4 rotMat = getRotMatrix(rot.coords.x, rot.coords.y, rot.coords.z);
				outlineProgram.setMat4("rot", GL_TRUE, mat4_get_ptr(rotMat));

				mat4 scaleMat = getScaleMatrix(outlineScale.coords.x, outlineScale.coords.y, outlineScale.coords.z);
				outlineProgram.setMat4("scale", GL_TRUE, mat4_get_ptr(scaleMat));

				outlineProgram.setMat4("projection", GL_TRUE, mat4_get_ptr(projection));

				outlineProgram.setMat4("view", GL_TRUE, mat4_get_ptr(view));

				cube.render();
				outlineProgram.unbind();

			}
		}
        */

		ImGui::PushFont(robotoFont);
		{
			ImGui::Begin("Triangle Info");
			if (ImGui::CollapsingHeader("transform")) {
				if (ImGui::TreeNode("position")) {
					ImGui::SliderFloat("x", &cube.pos.coords.x, -3.0f, 3.0f);
					ImGui::SliderFloat("y", &cube.pos.coords.y, -3.0f, 3.0f);
					ImGui::SliderFloat("z", &cube.pos.coords.z, -3.0f, 3.0f);

					if (ImGui::Button("reset")) {
						cube.pos = vec3();
					}

					ImGui::TreePop();
				}

				if (ImGui::TreeNode("scale")) {
					ImGui::SliderFloat("x", &cube.scale.coords.x, -5.0f, 5.0f);
					ImGui::SliderFloat("y", &cube.scale.coords.y, -5.0f, 5.0f);
					ImGui::SliderFloat("z", &cube.scale.coords.z, -5.0f, 5.0f);

					if (ImGui::Button("reset")) {
						cube.scale = vec3(1.0f, 1.0f, 1.0f);
					}

					ImGui::TreePop();
				}

				if (ImGui::TreeNode("rotation")) {
					ImGui::SliderFloat("x", &cube.rot.coords.x, -180.0f, 180.0f);
					ImGui::SliderFloat("y", &cube.rot.coords.y, -180.0f, 180.0f);
					ImGui::SliderFloat("z", &cube.rot.coords.z, -180.0f, 180.0f);

					if (ImGui::Button("reset")) {
						cube.rot = vec3();
					}

					ImGui::TreePop();
				}
			}
			if (ImGui::CollapsingHeader("color")) {
				ImGui::ColorEdit3("Triangle color", &cube.color.vals[0]);
			}

			if (ImGui::Button("toggle outline")) {
				cube.outline = !cube.outline;
			}

			ImGui::End();
		}

		{
			ImGui::Begin("Camera Info");

			if (ImGui::CollapsingHeader("transform")) {
				if (ImGui::TreeNode("position")) {
					ImGui::SliderFloat("x", &cam.pos.coords.x, -10.0f, 10.0f);
					ImGui::SliderFloat("y", &cam.pos.coords.y, -10.0f, 10.0f);
					ImGui::SliderFloat("z", &cam.pos.coords.z, -10.0f, 10.0f);

					cam.target = vec3_add(cam.pos, cam.offset);
					if (ImGui::Button("reset")) {
						cam.pos = vec3(0.0f, 0.0f, 5.0f);
					}
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("rotation")) {
					ImGui::SliderFloat("x", &cam.rot.coords.x, -180.0f, 180.0f);
					ImGui::SliderFloat("y", &cam.rot.coords.y, -180.0f, 180.0f);
					ImGui::SliderFloat("z", &cam.rot.coords.z, -180.0f, 180.0f);
					if (ImGui::Button("reset")) {
						cam.rot = vec3();
					}
					ImGui::TreePop();
				}
			}

			ImGui::End();
		}

		ImGui::PopFont();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		SDL_GL_SwapWindow(window);

	}


	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
