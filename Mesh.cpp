#include "Mesh.h"

Mesh::Mesh() {}

Mesh::Mesh(std::vector<glm::vec3> v, std::vector<glm::vec3> n,
	std::vector<glm::vec2> uv, std::vector<glm::uvec3> i, std::vector<Texture> t,
    std::vector<glm::ivec4> b, std::vector<glm::vec4> w) {

    // initialize data from parameters
	vertices = v;
	normals = n;
	uvs = uv;
    indices = i;
    textures = t;
    boneIds = b;
    weights = w;

    // initialize model matrix
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

    // bone ids
    glGenBuffers(1, &VBO_BI);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_BI);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::ivec4) * this->boneIds.size(), this->boneIds.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, 4, GL_INT, 4 * sizeof(GLint), 0);
    

    // weights
    glGenBuffers(1, &VBO_W);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_W);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * this->weights.size(), this->weights.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

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

void Mesh::draw(glm::mat4 view, glm::mat4 projection, glm::mat4 parent, GLuint shaderProgram) {
    // Actiavte the shader program 
    glUseProgram(shaderProgram);

    glm::vec3 color = glm::vec3(0.5f, 0.5f, 0.5f);

    // Get textures from texture vector
            // bind appropriate textures
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;

    model = parent * model;

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
    // Does not have animations
    glUniform1i(glGetUniformLocation(shaderProgram, "hasAnimation"), 0);

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
    glUseProgram(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::draw(glm::mat4 view, glm::mat4 projection, glm::mat4 parent, std::vector<glm::mat4> transforms, GLuint shaderProgram) {
    // Actiavte the shader program 
    glUseProgram(shaderProgram);

    glm::vec3 color = glm::vec3(0.5f, 0.5f, 0.5f);

    // Get textures from texture vector
            // bind appropriate textures
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;

    model = parent* model;

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

    // Has animations
    glUniform1i(glGetUniformLocation(shaderProgram, "hasAnimation"), 1);

    // Get the shader variable locations and send the uniform data to the shader 
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, false, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, false, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // transformations
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "finalBonesMatrices"), transforms.size(), 
        GL_FALSE, glm::value_ptr(transforms[0]));

    // Camera position --- TO DO: just get eyePos from windows or just create a camera class? Inverse can be a expensive operation
        // transformations
    glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(glm::vec3(glm::inverse(view)[3])));

    // Bind the VAO
    glBindVertexArray(VAO);

    // Draw the points 
    glDrawElements(GL_TRIANGLES, indices.size() * 3, GL_UNSIGNED_INT, 0);

    //Check if vertices imported correctly, print point cloud
    // Set point size
    /*
    glPointSize(10.0f);

    // Draw the points 
    glDrawArrays(GL_POINTS, 0, vertices.size());
    */

    // Unbind the VAO and shader program
    glBindVertexArray(0);
    glUseProgram(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}