#include "Mesh.h"

Mesh::Mesh() {

    /*
    vertices = v;
    normals = n;
    uvs = uv;
    indices = i;
    */

    model = glm::mat4(1.0f);

    vertices = {
        glm::vec3(-2.5, 2.5, 2.5),
        glm::vec3(-2.5, -2.5, 2.5),
        glm::vec3(2.5, -2.5, 2.5),
        glm::vec3(2.5, 2.5, 2.5),
        glm::vec3(-2.5, 2.5, -2.5),
        glm::vec3(-2.5, -2.5, -2.5),
        glm::vec3(2.5, -2.5, -2.5),
        glm::vec3(2.5, 2.5, -2.5)
    };

    indices = 
    {
        // Front face.
        glm::uvec3(0, 1, 2),
        glm::uvec3(2, 3, 0),
        // Back face.
        glm::uvec3(7, 6, 5),
        glm::uvec3(5, 4, 7),
        // Right face.
        glm::uvec3(3, 2, 6),
        glm::uvec3(6, 7, 3),
        // Left face.
        glm::uvec3(4, 5, 1),
        glm::uvec3(1, 0, 4),
        // Top face.
        glm::uvec3(4, 0, 3),
        glm::uvec3(3, 7, 4),
        // Bottom face.
        glm::uvec3(1, 5, 6),
        glm::uvec3(6, 2, 1),
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // vertex positions
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    
    // vertex normals
    glGenBuffers(1, &VBO_N);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_N);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * this->normals.size(), this->normals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    // vertex texture coords
    glGenBuffers(1, &VBO_UV);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_UV);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * this->uvs.size(), this->uvs.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
    

    // indices
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::uvec3) * this->indices.size(),
        this->indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Mesh::Mesh(std::vector<glm::vec3> v, std::vector<glm::vec3> n,
	std::vector<glm::vec2> uv, std::vector<glm::uvec3> i, std::vector<Texture> t) {

	vertices = v;
	normals = n;
	uvs = uv;
    indices = i;
    textures = t;

    model = glm::mat4(1.0f);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // vertex positions
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * this->vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    // vertex normals
    glGenBuffers(1, &VBO_N);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_N);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * this->normals.size(), this->normals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    // vertex texture coords
    glGenBuffers(1, &VBO_UV);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_UV);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * this->uvs.size(), this->uvs.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);

    // indices
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::uvec3) * this->indices.size(),
        this->indices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Mesh::~Mesh() {
    // Delete the VBO/EBO and the VAO.

    /*
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &VBO_N);
    glDeleteBuffers(1, &VBO_UV);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
    */
}

void Mesh::rotate(glm::mat4 rot) {
    std::cout << "n Draw: " << glm::to_string(model) << std::endl;

    model = rot * model;

    std::cout << "In Draw: " << glm::to_string(model) << std::endl;
}

void Mesh::draw(glm::mat4 view, glm::mat4 projection, GLuint shaderProgram) {
    // Actiavte the shader program 
    glUseProgram(shaderProgram);

    glm::vec3 color = glm::vec3(0.5f, 0.5f, 0.5f);

    // Get textures from texture vector
            // bind appropriate textures
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;

    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        std::string number = "";
        std::string name = textures[i].type;
        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "texture_specular")
            number = std::to_string(specularNr++); // transfer unsigned int to string
        else if (name == "texture_normal")
            number = std::to_string(normalNr++); // transfer unsigned int to string
        else if (name == "texture_height")
            number = std::to_string(heightNr++); // transfer unsigned int to string

        // now set the sampler to the correct texture unit
        glUniform1i(glGetUniformLocation(shaderProgram, (name + number).c_str()), i);
        // and finally bind the texture n
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    // Get the shader variable locations and send the uniform data to the shader 
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, false, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, false, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // Bind the VAO
    glBindVertexArray(VAO);

    // Draw the points 
    glDrawElements(GL_TRIANGLES, indices.size() * 3, GL_UNSIGNED_INT, 0);

    
    // Check if vertices imported correctly
    // Set point size
    // glPointSize(10.0f);

    // Draw the points 
    // glDrawArrays(GL_POINTS, 0, vertices.size());
    // std::cout << vertices.size() << std::endl;
    

    // Unbind the VAO and shader program
    glBindVertexArray(0);
    // glUseProgram(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);

}