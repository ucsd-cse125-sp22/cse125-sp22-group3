#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

class Drawable {
	public:
		virtual void draw(glm::mat4 view, glm::mat4 projection, GLuint shader) {}
};
