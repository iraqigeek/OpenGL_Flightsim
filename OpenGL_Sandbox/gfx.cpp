#include "gfx.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "lib/tiny_obj_loader.h"


std::string load_text_file(const std::string& path)
{
		std::fstream file(path);
		if (!file.is_open())
			return std::string();

		std::stringstream buffer;
		buffer << file.rdbuf();
		return buffer.str();
}

namespace gfx {

	Shader::Shader(const std::string& path) : Shader(load_text_file(path + ".vert"), load_text_file(path + ".frag")) {}
	
	Shader::Shader(const std::string& vertShader, const std::string& fragShader)
	{
		//std::cout << "create Shader\n";
		const char* vertexShaderSource = vertShader.c_str();
		const char* fragmentShaderSource = fragShader.c_str();

		unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
		glCompileShader(vertexShader);

		// check for shader compile errors
		int success;
		char infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		// fragment shader
		unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
		glCompileShader(fragmentShader);
		// check for shader compile errors
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		// link shaders
		id = glCreateProgram();
		glAttachShader(id, vertexShader);
		glAttachShader(id, fragmentShader);
		glLinkProgram(id);
		// check for linking errors
		glGetProgramiv(id, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(id, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	Shader::~Shader()
	{
		glDeleteProgram(id);
	}

	void Shader::upload_uniforms() const
	{
		glUseProgram(id);
	}

	void Shader::unbind() const
	{
		glUseProgram(0);
	}

	void Shader::set_int(const std::string& name, int value) 
	{
		glUniform1i(glGetUniformLocation(id, name.c_str()), value);
	}

	void Shader::set_float(const std::string& name, float value) 
	{
		glUniform1f(glGetUniformLocation(id, name.c_str()), value);
	}

	void Shader::set_vec3(const std::string& name, const glm::vec3& value)
	{
		glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
	}

	void Shader::set_vec4(const std::string& name, const glm::vec4& value)
	{
		glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
	}

	void Shader::set_mat4(const std::string& name, const glm::mat4& value)
	{
		glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &value[0][0]);
	}
	
	Geometry::Geometry(const std::vector<float>& vertices, const VertexLayout& layout)
		: count(static_cast<int>(vertices.size()) / (get_stride(layout)))
	{
		const int stride = get_stride(layout);
		glGenVertexArrays(1, &m_vao);
		glGenBuffers(1, &m_vbo);

		glBindVertexArray(m_vbo);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

		unsigned int index = 0;
		glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);
		glEnableVertexAttribArray(index);

		if (layout == POS_NORM || layout == POS_NORM_UV) // add normal
		{
			index++;
			glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(index);
		}

		if (layout == POS_UV || layout == POS_NORM_UV) // add uv
		{
			index++;
			glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(static_cast<int>(index) * 3 * sizeof(float)));
			glEnableVertexAttribArray(index);
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	Geometry::Geometry(const void* data, size_t size, const VertexLayout& layout)
	//	: count(static_cast<int>(vertices.size()) / (get_stride(layout)))
	{

		const int stride = get_stride(layout);
		glGenVertexArrays(1, &m_vao);
		glGenBuffers(1, &m_vbo);

		glBindVertexArray(m_vbo);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

		unsigned int index = 0;
		glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);
		glEnableVertexAttribArray(index);

		if (layout == POS_NORM || layout == POS_NORM_UV) // add normal
		{
			index++;
			glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(index);
		}

		if (layout == POS_UV || layout == POS_NORM_UV) // add uv
		{
			index++;
			glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(static_cast<int>(index) * 3 * sizeof(float)));
			glEnableVertexAttribArray(index);
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

	}

	Geometry::Geometry(const Geometry& geometry)
	{
		//std::cout << "copy Geometry\n";
		count = geometry.count;
		m_vao = geometry.m_vao;
		m_vbo = geometry.m_vbo;
	}

	Geometry::~Geometry()
	{
		//std::cout << "destroy Geometry\n";
		glDeleteVertexArrays(1, &m_vao);
		glDeleteBuffers(1, &m_vbo);
	}

	void Geometry::upload_uniforms()
	{
        glBindVertexArray(m_vao); 
	}

	int Geometry::get_stride(const VertexLayout& layout)
	{
		switch (layout)
		{
		case POS:			return 3;
		case POS_UV:		return 5;
		case POS_NORM:		return 6;
		case POS_NORM_UV:	return 8;
		}
		return 0;
	}

	Object3D::Type Camera::get_type() const
	{
		return Object3D::Type::CAMERA;
	}

	glm::mat4 Camera::get_view_matrix() const
	{
		return glm::inverse(transform);
	}

	glm::mat4 Camera::get_projection_matrix() const
	{
		return m_projection;
	}

	void Camera::look_at(const glm::vec3& target)
	{
		override_transform(glm::inverse(glm::lookAt(m_position, target, m_up)));
	}

	template<class Derived>
	std::shared_ptr<Shader> MaterialX<Derived>::shader = nullptr;
	
	void Object3D::draw(RenderContext& context)
	{
		draw_children(context);
	}

	void Object3D::draw_children(RenderContext& context)
	{
		for (auto child : children) child->draw(context);
	}

	glm::vec3 Object3D::get_position() const
	{
		return m_position;
	}

	glm::vec3 Object3D::get_rotation() const
	{
		return m_rotation;
	}

	glm::vec3 Object3D::get_scale() const
	{
		return m_scale;
	}

	void Object3D::set_scale(const glm::vec3& scale)
	{
		m_scale = scale; m_dirty_dof = true;
	}

	void Object3D::set_position(const glm::vec3& pos)
	{
		m_position = pos; m_dirty_dof = true;
	}

	void Object3D::set_rotation(const glm::vec3& rot)
	{
		m_rotation = rot; m_dirty_dof = true;
	}

	glm::mat4 Object3D::get_local_transform() const
	{
		auto S = glm::scale(glm::mat4(1.0f), m_scale);
		auto T = glm::translate(glm::mat4(1.0f), m_position);
		auto R = glm::eulerAngleYXZ(m_rotation.x, m_rotation.y, m_rotation.z);
		return T * R * S;
	}

	void Object3D::override_transform(const glm::mat4& matrix)
	{
		m_dirty_transform = true; m_dirty_dof = true;
		transform = matrix;
		// TODO: relcalculate position, rotation etc
	}

	void Object3D::update_world_matrix(bool dirtyParent)
	{
		bool dirty = m_dirty_dof || dirtyParent;

		if (dirty && !m_dirty_transform)
		{
			if (parent)
				transform = parent->transform * get_local_transform();
			else
				transform = get_local_transform();
		}
	
		for (auto child : children)
		{
			child->update_world_matrix(dirty || m_dirty_transform);
		}

		m_dirty_dof = m_dirty_transform = false;
	}
	
	Object3D& Object3D::add(Object3D* child)
	{
		child->parent = this;
		children.push_back(child);
		return (*this);
	}

	glm::vec3 Object3D::get_world_position() const
	{
		auto world = transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		return glm::vec3(world.x, world.y, world.z);
	}

	Object3D::Type Object3D::get_type() const
	{
		return Type::OBJECT3D;
	}

	Object3D::Type Light::get_type() const
	{
		return Object3D::Type::LIGHT;
	}

	glm::mat4 Light::light_space_matrix()
	{
		float near_plane = 0.1f, far_plane = 25.0f, m = 10.0f;
		glm::mat4 lightView = glm::lookAt(get_world_position(), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightProjection = glm::ortho(-m, m, -m, m, -10.0f, 20.0f);
		return lightProjection * lightView;
	}

	void Renderer::render(Camera& camera, Object3D& scene)
	{
		scene.update_world_matrix(false);

		RenderContext context;
		context.camera			= &camera;
		context.shadow_map		= m_shadowMap;
		context.shadow_caster	= nullptr;
		context.background_color = background;

		scene.traverse([&context](Object3D* obj) {
			if (obj->get_type() == Object3D::Type::LIGHT)
			{
				Light* light = dynamic_cast<Light*>(obj);
				if (light->cast_shadow)
				{
					context.shadow_caster = light;
				}
				context.lights.push_back(light);
			}

			return true;
		});

#if 1
		if (m_shadowMap && context.shadow_caster)
		{
			glViewport(0, 0, m_shadowMap->width, m_shadowMap->height);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_shadowMap->fbo);
			glClear(GL_DEPTH_BUFFER_BIT);

			context.is_shadow_pass	= true;
			scene.draw(context);
	
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_shadowMap->depth_map);
		}
#endif

		glViewport(0, 0, m_width, m_height);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		context.is_shadow_pass = false;
#if 1


#ifdef WIREFRAME
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif
		scene.draw(context);
#ifdef WIREFRAME
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

#else
		m_quad.get()->draw(context);
#endif
	}

	void Mesh::draw(RenderContext& context)
	{
		glm::mat4 lightSpaceMatrix(1.0f);

		if (context.is_shadow_pass)
		{
			assert(context.shadow_caster);

			Shader* shader = &context.shadow_map->shader;

			shader->upload_uniforms();
			shader->set_mat4("model", transform);
			shader->set_mat4("lightSpaceMatrix", context.shadow_caster->light_space_matrix());
		}
		else {
			Shader* shader = m_material->get_shader();

			shader->upload_uniforms();
			shader->set_mat4("model", transform);
			shader->set_mat4("view", context.camera->get_view_matrix());
			shader->set_mat4("proj", context.camera->get_projection_matrix());

			if (context.shadow_caster)
				shader->set_mat4("lightSpaceMatrix", context.shadow_caster->light_space_matrix());
			
			shader->set_int("shadowMap", 0);
			shader->set_vec3("backgroundColor", context.background_color);
			shader->set_int("numLights", static_cast<int>(context.lights.size()));
			shader->set_vec3("cameraPos", context.camera->get_world_position()); 
			shader->set_int("receiveShadow", (receive_shadow && context.shadow_caster) ? 1 : 0);

			for (int i = 0; i < context.lights.size(); i++)
			{
				auto index = std::to_string(i);
				auto type = context.lights[i]->type;

				shader->set_int( "lights[" + index + "].type", type);
				shader->set_vec3("lights[" + index + "].color", context.lights[i]->rgb);
				shader->set_vec3("lights[" + index + "].position", context.lights[i]->get_world_position());
			}

			m_material->upload_uniforms();
		}

		m_geometry->upload_uniforms();
		glDrawArrays(GL_TRIANGLES, 0, m_geometry->count);

		draw_children(context);
	}

	ShadowMap::ShadowMap(unsigned int shadow_width, unsigned int shadow_height)
		: width(shadow_width), height(shadow_height), shader("shaders/depth")
	{
#if 1
		glGenFramebuffers(1, &fbo);

		glGenTextures(1, &depth_map);
		glBindTexture(GL_TEXTURE_2D, depth_map);
		glTexImage2D(
			GL_TEXTURE_2D, 
			0, 
			GL_DEPTH_COMPONENT, 
			shadow_width, 
			shadow_height, 
			0, 
			GL_DEPTH_COMPONENT, 
			GL_FLOAT, 
			NULL
		);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindBuffer(GL_FRAMEBUFFER, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "failure\n";
			exit(-1);
		}
#endif
	}

	void Controller::update(Object3D& object, float dt)
	{
		const auto pos = object.get_position();
		object.set_position(pos + m_velocity * dt);
		object.override_transform(glm::inverse(glm::lookAt(pos, pos + m_front, m_up)));
		m_velocity = glm::vec3(0.0f);
	}

	void Controller::move_mouse(float x, float y)
	{
		if (!m_initialized)
		{
			m_last_pos.x = x;
			m_last_pos.y = y;
			m_initialized = true;
		}

		glm::vec2 offset(m_last_pos.x - x, m_last_pos.y - y);

		const float sensitivity = 0.1f;
		offset *= sensitivity;

		m_yaw	-= offset.x;
		m_pitch += offset.y;

		glm::vec3 front(0);
		front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		front.y = sin(glm::radians(m_pitch));
		front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		m_front = glm::normalize(front);
	}

	void Controller::move(const Direction& direction)
	{
		switch (direction)
		{
		case FORWARD: {
			m_velocity += (m_speed * m_front);
			break;
		}
		case LEFT: {
			m_velocity -= (m_speed * glm::normalize(glm::cross(m_front, m_up)));
			break;
		}
		case BACKWARD: {
			m_velocity -= (m_speed * m_front);
			break;
		}
		case RIGHT: {
			m_velocity += (m_speed * glm::normalize(glm::cross(m_front, m_up)));
			break;
		}
		default:
			break;
		}
	}

	void Phong::upload_uniforms()
	{
		Shader* shader = get_shader();
		shader->set_float("ka", ka);
		shader->set_float("kd", kd);
		shader->set_float("ks", ks);
		shader->set_float("alpha", alpha);
		shader->set_vec3("objectColor", rgb);
	}

	void Basic::upload_uniforms()
	{
		Shader* shader = get_shader();
		shader->set_float("ka", 0.6f);
		shader->set_float("kd", 0.8f);
		shader->set_float("ks", 0.2f);
		shader->set_float("alpha", 10.0f);
		shader->set_vec3("objectColor", rgb);
	}

	VertexBuffer::VertexBuffer(const void* data, size_t size)
	{
		glGenBuffers(1, &id);
		glBindBuffer(GL_ARRAY_BUFFER, id);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	}

	VertexBuffer::~VertexBuffer()
	{
		glDeleteBuffers(1, &id);
	}

	void VertexBuffer::upload_uniforms() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, id);
	}

	void VertexBuffer::unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void load_obj(const std::string path, std::vector<float>& vertices)
	{
		std::istringstream source(load_text_file(path));

		std::string warning, error;
		tinyobj::attrib_t attributes;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;

		if (!tinyobj::LoadObj(
			&attributes,
			&shapes,
			&materials,
			&warning,
			&error,
			&source))
		{
			throw std::runtime_error("loadObj::Error: " + warning + error);
		}

		printf("# of vertices  = %d\n", (int)(attributes.vertices.size()) / 3);
		printf("# of normals   = %d\n", (int)(attributes.normals.size()) / 3);
		printf("# of texcoords = %d\n", (int)(attributes.texcoords.size()) / 2);
		printf("# of materials = %d\n", (int)materials.size());
		printf("# of shapes    = %d\n", (int)shapes.size());


		for (size_t s = 0; s < shapes.size(); s++) {
			// Loop over faces(polygon)
			size_t index_offset = 0;
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {

				//hardcode loading to triangles
				int fv = 3;

				// Loop over vertices in the face.
				for (size_t v = 0; v < fv; v++) {
					// access to vertex
					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

					//vertex position
					tinyobj::real_t vx = attributes.vertices[3 * idx.vertex_index + 0];
					tinyobj::real_t vy = attributes.vertices[3 * idx.vertex_index + 1];
					tinyobj::real_t vz = attributes.vertices[3 * idx.vertex_index + 2];
					//vertex normal
					tinyobj::real_t nx = attributes.normals[3 * idx.normal_index + 0];
					tinyobj::real_t ny = attributes.normals[3 * idx.normal_index + 1];
					tinyobj::real_t nz = attributes.normals[3 * idx.normal_index + 2];

					glm::vec3 position, normal;

					position.x = vx;
					position.y = vy;
					position.z = vz;

					normal.x = nx;
					normal.y = ny;
					normal.z = nz;

					//std::cout << position.x <<  ", " << position.y  << ", " << position.z << std::endl;

					vertices.push_back(position.x);
					vertices.push_back(position.y);
					vertices.push_back(position.z);
					vertices.push_back(normal.x);
					vertices.push_back(normal.y);
					vertices.push_back(normal.z);
				}
				index_offset += fv;
			}
		}

#if 0
		int i = 0;
		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {

				glm::vec3 pos;
				glm::vec2 tex;

				indices.push_back(i++);

				pos = {
					attributes.vertices[3 * index.vertex_index + 0],
					attributes.vertices[3 * index.vertex_index + 1],
					attributes.vertices[3 * index.vertex_index + 2]
				};

				tex = {
					attributes.texcoords[2 * index.texcoord_index + 0],
					attributes.texcoords[2 * index.texcoord_index + 1]
				};


				vertices.push_back(pos.x);
				vertices.push_back(pos.y);
				vertices.push_back(pos.z);

				vertices.push_back(pos.x);
				vertices.push_back(pos.y);
				vertices.push_back(pos.z);

				std::cout << "i=" << i << ", x=" << pos.x << ", y=" << pos.y << ", z=" << pos.z << std::endl;
			}
		}
#endif
	}
}
