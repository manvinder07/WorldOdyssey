#pragma once

#include <array>
#include <utility>

#include "common.hpp"
#include "components.hpp"
#include "tiny_ecs.hpp"


// fonts
#include <ft2build.h>
#include FT_FREETYPE_H
#include <map>

// matrices
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct ImmediateTexture {
    TEXTURE_ASSET_ID id;
    vec2 position;
    vec2 scale;
    vec3 color;
};

// System responsible for setting up OpenGL and for rendering all the
// visual entities in the game
class RenderSystem {
	/**
	 * The following arrays store the assets the game will use. They are loaded
	 * at initialization and are assumed to not be modified by the render loop.
	 *
	 * Whenever possible, add to these lists instead of creating dynamic state
	 * it is easier to debug and faster to execute for the computer.
	 */
	std::array<GLuint, texture_count> texture_gl_handles;
	std::array<ivec2, texture_count> texture_dimensions;

	// Make sure these paths remain in sync with the associated enumerators.
	// Associated id with .obj path
	const std::vector < std::pair<GEOMETRY_BUFFER_ID, std::string>> mesh_paths =
	{
		  std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::PROTAGONIST, mesh_path("chicken.obj")),
		  // specify meshes of other assets here
		  std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::ROCK0, mesh_path("stones/obj/stone_1.obj")),
		  std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::ROCK1, mesh_path("stones/obj/stone_2.obj")),
		  std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::ROCK2, mesh_path("stones/obj/stone_3.obj")),
		  std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::ROCK3, mesh_path("stones/obj/stone_4.obj")),
		  std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::ROCK4, mesh_path("stones/obj/stone_5.obj")),
	};

	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, texture_count> texture_paths = {
			textures_path("bug2.png"),
			textures_path("eagle.png"),
            textures_path("simplePlayer.png"),
			textures_path("scorpion.png"),
			textures_path("snake.png"),
			textures_path("boss.png"),
			textures_path("newbackground.png"),
            textures_path("forest_background.png"),
			textures_path("foreground.png"),
			textures_path("foreground2.png"),
            textures_path("forest_foreground.png"),
            textures_path("gun.png"),
            textures_path("bullet.png"),
            textures_path("player2_1.png"),
            textures_path("player2_2.png"),
            textures_path("player2_3.png"),
            textures_path("player2_4.png"),
            textures_path("help_text_black.png"),
            textures_path("sword.png"),
            textures_path("armor.png"),
            textures_path("white.png"),
            textures_path("heart.png"),
            textures_path("grenade.png"),
            textures_path("explosion1.png"),
            textures_path("explosion2.png"),
            textures_path("explosion3.png"),
            textures_path("explosion4.png"),
            textures_path("explosion5.png"),
            textures_path("explosion6.png"),
            textures_path("explosion7.png"),
            textures_path("explosion8.png"),
            textures_path("explosion9.png"),
            textures_path("explosion10.png"),
            textures_path("dragon1.png"),
            textures_path("dragon2.png"),
            textures_path("dragon3.png"),
            textures_path("tornado1.png"),
            textures_path("tornado2.png"),
            textures_path("tornado3.png"),
            textures_path("tornado4.png"),
            textures_path("heart_l.png"),
            textures_path("spider.png"),
			textures_path("Ice-background.jpg"),
			textures_path("ice-foreground.png"),
			textures_path("iceboss.png"),
			textures_path("ice1.png"),
			textures_path("ice2.png"),
			textures_path("snowball.png")
	};

	std::array<GLuint, effect_count> effects;
	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, effect_count> effect_paths = {
		shader_path("coloured"),
		shader_path("egg"),
		shader_path("simplePlayer"),
		shader_path("textured"),
		shader_path("wind") };

	std::array<GLuint, geometry_count> vertex_buffers;
	std::array<GLuint, geometry_count> index_buffers;
	std::array<Mesh, geometry_count> meshes;

	GLuint vao;
	// fonts
	std::map<char, Character> m_ftCharacters;
	GLuint m_font_shaderProgram;
	GLuint m_font_VAO;
	GLuint m_font_VBO;

public:
	bool show_help;
	// Initialize the window
	bool init(GLFWwindow* window);

	template <class T>
	void bindVBOandIBO(GEOMETRY_BUFFER_ID gid, std::vector<T> vertices, std::vector<uint16_t> indices);

	void initializeGlTextures();

	void initializeGlEffects();

	void initializeGlMeshes();
	Mesh& getMesh(GEOMETRY_BUFFER_ID id) { return meshes[(int)id]; };

	void initializeGlGeometryBuffers();
	// Initialize the screen texture used as intermediate render target
	// The draw loop first renders to this texture, then it is used for the wind
	// shader
	bool initScreenTexture();

	// Destroy resources associated to one or all entities created by the system
	~RenderSystem();

	// Draw all entities
	void draw();

	mat3 createProjectionMatrix();
    mat3 createStaticProjectionMatrix();

    void renderInstructions();

    void renderFailScreen();

    void renderSuccessScreen();

    void renderHelp();

    void renderText(const std::string& text, float x, float y, float scale, const glm::vec3& color, const glm::mat4& trans);

    bool fontInit(const std::string& font_filename, unsigned int font_default_size);

	void drawHelpText();

    // Immidiate function to render a texture in a absolute position in the screen (use to render the inventory)
    void renderImmediateTexture(TEXTURE_ASSET_ID id, vec2 position, vec2 scale, vec3 color, mat3& projection);
    std::vector<ImmediateTexture> immediate_textures;

private:
	// Internal drawing functions for each entity type
	void drawTexturedMesh(Entity entity, const mat3& projection);
	void drawToScreen();
    void drawInventory();

	// Window handle
	GLFWwindow* window;

	// Screen texture handles
	GLuint frame_buffer;
	GLuint off_screen_render_buffer_color;
	GLuint off_screen_render_buffer_depth;

	Entity screen_state_entity;
};

bool loadEffectFromFile(
	const std::string& vs_path, const std::string& fs_path, GLuint& out_program);
