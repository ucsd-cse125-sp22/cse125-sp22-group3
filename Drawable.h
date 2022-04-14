#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

class Drawable {
	public:
		virtual void Draw(glm::mat4 view, glm::mat4 projection, GLuint shader) = 0;
};
