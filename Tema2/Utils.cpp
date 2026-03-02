// #include <iostream>
// #include <vector>
// #include "core/engine.h"
// #include "utils/gl_utils.h"
// #include "lab_m1/Tema2/Tema2.h"
// using namespace std;
// using namespace m1;

// void Create_Rectangle_2X1(){
//     vector<VertexFormat> vertices{
// 			VertexFormat(glm::vec3(0, 0, 1), glm::vec3(0, 1, 1),
// 						 glm::vec3(0.2, 0.8, 0.6)),
// 			VertexFormat(glm::vec3(1, 0, 1), glm::vec3(0, 0, 1),
// 						 glm::vec3(0.2, 0.7, 0.1)),
// 			VertexFormat(glm::vec3(0, 1, 1), glm::vec3(1, 0, 1),
// 						 glm::vec3(0.2, 0.6, 0.2)),
// 			VertexFormat(glm::vec3(1, 1, 1), glm::vec3(0, 1, 1),
// 						 glm::vec3(0.2, 0.5, 0.3)),
// 			VertexFormat(glm::vec3(0, 0, -1), glm::vec3(0, 0, 1),
// 						 glm::vec3(0.2, 0.4, 0.4)),
// 			VertexFormat(glm::vec3(1, 0, -1), glm::vec3(1, 0, 1),
// 						 glm::vec3(0.2, 0.3, 0.5)),
// 			VertexFormat(glm::vec3(0, 1, -1), glm::vec3(0, 1, 1),
// 						 glm::vec3(0.2, 0.2, 0.6)),
// 			VertexFormat(glm::vec3(1, 1, -1), glm::vec3(0, 0, 1),
// 						 glm::vec3(0.2, 0.1, 0.7)),

// 		};

//         vector<unsigned int> indices = {
// 			0, 1, 2, // indices for first triangle
// 			1, 3, 2, // indices for second triangle
// 					 // TODO(student): Complete indices data for the cube mesh
// 			2, 3, 7, 2, 7, 6, 1, 7, 3, 1, 5, 7, 6, 7, 4, 7, 5, 4, 0, 4, 1, 1, 4,
// 			5, 2, 6, 4, 0, 2, 4

// 		};

//         CreateMesh("Rectangle", vertices, indices);
// }

// void CreateMesh(const char *name,
// 					  const std::vector<VertexFormat> &vertices,
// 					  const std::vector<unsigned int> &indices) {
// 	unsigned int VAO = 0;
// 	// TODO(student): Create the VAO and bind it
// 	glGenVertexArrays(1, &VAO);
// 	glBindVertexArray(VAO);

// 	unsigned int VBO = 0;
// 	// TODO(student): Create the VBO and bind it
// 	glGenBuffers(1, &VBO);
// 	// TODO(student): Send vertices data into the VBO buffer
// 	glBindBuffer(GL_ARRAY_BUFFER, VBO);
// 	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(),
// 				 &vertices[0], GL_STATIC_DRAW);
// 	unsigned int IBO = 0;
// 	// TODO(student): Create the IBO and bind it
// 	glGenBuffers(1, &IBO);
// 	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
// 	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(),
// 				 &indices[0], GL_STATIC_DRAW);
// 	// TODO(student): Send indices data into the IBO buffer

// 	// ========================================================================
// 	// This section demonstrates how the GPU vertex shader program
// 	// receives data. It will be learned later, when GLSL shaders will be
// 	// introduced. For the moment, just think that each property value from
// 	// our vertex format needs to be sent to a certain channel, in order to
// 	// know how to receive it in the GLSL vertex shader.

// 	// Set vertex position attribute
// 	glEnableVertexAttribArray(0);
// 	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

// 	// Set vertex normal attribute
// 	glEnableVertexAttribArray(1);
// 	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat),
// 						  (void *)(sizeof(glm::vec3)));

// 	// Set texture coordinate attribute
// 	glEnableVertexAttribArray(2);
// 	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat),
// 						  (void *)(2 * sizeof(glm::vec3)));

// 	// Set vertex color attribute
// 	glEnableVertexAttribArray(3);
// 	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat),
// 						  (void *)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));
// 	// ========================================================================

// 	// TODO(student): Unbind the VAO
// 	glBindVertexArray(0);

// 	// Check for OpenGL errors
// 	if (GetOpenGLError() == GL_INVALID_OPERATION) {
// 		cout << "\t[NOTE] : For students : DON'T PANIC! This error should go "
// 				"away when completing the tasks."
// 			 << std::endl;
// 		cout << "\t[NOTE] : For developers : This happens because OpenGL core "
// 				"spec >=3.1 forbids null VAOs."
// 			 << std::endl;
// 	}

// 	// Mesh information is saved into a Mesh object
// 	meshes[name] = new Mesh(name);
// 	meshes[name]->InitFromBuffer(VAO,
// 								 static_cast<unsigned int>(indices.size()));
// }