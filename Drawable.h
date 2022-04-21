#include <GL/glew.h>
#include "glm.h"

class Drawable {
	public:
		virtual void Draw(glm::mat4 view, glm::mat4 projection, GLuint shader) = 0;
		virtual void Draw(GLuint shader) = 0;
};
